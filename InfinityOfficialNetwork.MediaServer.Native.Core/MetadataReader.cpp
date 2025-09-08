#include "MetadataReader.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/timestamp.h>
#include <libavutil/imgutils.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/channel_layout.h>
#include <libavutil/mem.h>
#include <libavutil/pixdesc.h>
}

using namespace InfinityOfficialNetwork::MediaServer::Native::Core;

namespace {
	struct MemoryBuffer {
		const uint8_t* data;
		size_t size;
		size_t pos;
	};

	// Custom read function
	static int __cdecl read_packet(void* opaque, uint8_t* buf, int buf_size) {
		MemoryBuffer* mem_buffer = static_cast<MemoryBuffer*>(opaque);

		// Check if we have reached the end of the buffer
		if (mem_buffer->pos >= mem_buffer->size) {
			return AVERROR_EOF;
		}

		// Determine how many bytes to read
		size_t bytes_to_read = mem_buffer->size - mem_buffer->pos;
		if (bytes_to_read > buf_size) {
			bytes_to_read = buf_size;
		}

		// Copy data from the memory buffer to FFmpeg's buffer
		memcpy(buf, mem_buffer->data + mem_buffer->pos, bytes_to_read);
		mem_buffer->pos += bytes_to_read;

		return static_cast<int>(bytes_to_read);
	}

	// Custom seek function
	static int64_t __cdecl seek_packet(void* opaque, int64_t offset, int whence) {
		MemoryBuffer* mem_buffer = static_cast<MemoryBuffer*>(opaque);

		switch (whence) {
		case SEEK_SET:
			if (offset >= 0 && static_cast<size_t>(offset) <= mem_buffer->size) {
				mem_buffer->pos = static_cast<size_t>(offset);
				return mem_buffer->pos;
			}
			break;
		case SEEK_CUR:
			if (static_cast<size_t>(offset + mem_buffer->pos) <= mem_buffer->size) {
				mem_buffer->pos += offset;
				return mem_buffer->pos;
			}
			break;
		case SEEK_END:
			if (static_cast<size_t>(mem_buffer->size + offset) <= mem_buffer->size) {
				mem_buffer->pos = mem_buffer->size + offset;
				return mem_buffer->pos;
			}
			break;
		case AVSEEK_SIZE:
			return mem_buffer->size;
		}

		return AVERROR(EINVAL);
	}


}



std::variant<MediaMetadata, std::tuple<MetadataReader::ErrorCode, int>> MetadataReader::GetMetadata(const MediaContainer& container)
{

	// Setup the MemoryBuffer struct
	MemoryBuffer mem_buffer;
	mem_buffer.data = reinterpret_cast<const uint8_t*>(container.Data.data());
	mem_buffer.size = container.Data.size();
	mem_buffer.pos = 0;

	// Allocate AVIOContext
	// The internal buffer size should be sufficiently large, e.g., 4096 bytes
	// Pass the opaque data (our MemoryBuffer struct) and custom functions
	AVIOContext* avio_ctx = avio_alloc_context(
		(unsigned char*)av_malloc(4096),
		4096,
		0,
		&mem_buffer,
		read_packet,
		nullptr, // No write function
		seek_packet);

	if (!avio_ctx) {
		return std::tuple(ErrorCode::AvioContextAllocateError, 0);
	}

	// Allocate and configure the AVFormatContext
	AVFormatContext* fmt_ctx = avformat_alloc_context();
	if (!fmt_ctx) {
		avio_context_free(&avio_ctx);
		return std::tuple(ErrorCode::AvFormatContextAllocateError, 0);
	}

	// Set the AVFormatContext's I/O context to our custom one
	fmt_ctx->pb = avio_ctx;

	// Open the input (pass NULL as filename since we are using a custom I/O context)
	int ret = avformat_open_input(&fmt_ctx, NULL, NULL, NULL);
	if (ret < 0) {
		avformat_close_input(&fmt_ctx); // This also frees the AVIOContext and its buffer
		return std::tuple(ErrorCode::AvFormatOpenInputError, ret);
	}

	// Rest of your code for finding stream info and processing metadata...
	// Read stream information
	ret = avformat_find_stream_info(fmt_ctx, NULL);
	if (ret < 0) {
		avformat_close_input(&fmt_ctx);
		return std::tuple(ErrorCode::AvFormatFindStreamInfoError, ret);
	}

	std::map<std::string, std::string> metadataDict;

	// Get the metadata dictionary
	AVDictionary* metadata = fmt_ctx->metadata;
	if (metadata) {
		AVDictionaryEntry* entry = NULL;
		// Iterate through all entries in the dictionary
		while ((entry = av_dict_get(metadata, "", entry, AV_DICT_IGNORE_SUFFIX))) {
			metadataDict.insert({ std::string(entry->key), std::string(entry->value) });
		}
	}

	// Clean up. Note: avformat_close_input automatically frees the custom AVIOContext.
	// If you need to manually free it, use avio_context_free and av_free
	avformat_close_input(&fmt_ctx);
	av_free(avio_ctx->buffer);
	avio_context_free(&avio_ctx);

	return MediaMetadata{ .Metadata = metadataDict };
}
