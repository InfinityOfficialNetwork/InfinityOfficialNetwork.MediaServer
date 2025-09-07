#include "pch.h"
#include "NativeLibrary.h"
#include "marshal.h"



using namespace System;

#pragma managed(push,off)

struct LogCallbackBase {
	virtual void Write(std::string str, int level, std::string class_name) = 0;
};

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

#pragma managed(pop)
#pragma managed(push,on)


struct LogCallbackImpl : LogCallbackBase {
	// Inherited via LogCallbackBase
	void Write(std::string str, int level, std::string class_name) override
	{
		String^ string = msclr::interop::marshal_as<String^, std::string>(str)->TrimEnd('\n');
		String^ m_class_name = msclr::interop::marshal_as<String^, std::string>(class_name);
		InfinityOfficialNetwork::MediaServer::Native::Transcoding::NativeLibrary::s_logHandler->Invoke(string, (InfinityOfficialNetwork::MediaServer::Native::Transcoding::AvLogLevel)level, m_class_name);
	}
};

static LogCallbackImpl logCallbackImpl = LogCallbackImpl();
static int logLevel = AV_LOG_INFO;

#pragma managed(pop)
#pragma managed(push,off)


void __cdecl NativeCallbackTrampoline(void* avcl, int level, const char* fmt, va_list vl) {
	if (level > logLevel)
		return;

	const char* class_name = avcl ? av_default_item_name(avcl) : "";

	va_list args_copy;
	va_copy(args_copy, vl);
	int size = vsnprintf(nullptr, 0, fmt, args_copy);
	va_end(args_copy);

	if (size <= 0) {
		return;
	}

	std::string buffer;
	buffer.resize(size + 1);
	vsnprintf(buffer.data(), buffer.size(), fmt, vl);

	logCallbackImpl.Write(buffer, level, class_name);
}

#pragma managed(pop)


using namespace System;
using namespace System::Runtime::InteropServices;
using namespace InfinityOfficialNetwork::MediaServer::Native::Transcoding;

void InfinityOfficialNetwork::MediaServer::Native::Transcoding::NativeLibrary::SetLogging(Action<String^, AvLogLevel, String^>^ handler)
{
	s_logHandler = handler;
	av_log_set_callback(&NativeCallbackTrampoline);
}

void InfinityOfficialNetwork::MediaServer::Native::Transcoding::NativeLibrary::SetLoggingLevel(AvLogLevel level) {
	av_log_set_level((int)level);
	logLevel = (int)level;
}

void InfinityOfficialNetwork::MediaServer::Native::Transcoding::NativeLibrary::ClearLogging()
{
	s_logHandler = gcnew Action<String^, AvLogLevel, String^>(defaultLogHandler);
}

BEGIN_MANAGED

struct StdOut {
	static void Write(String^ str) {
		Console::Out->Write(str);
	}
};

struct StdErr {
	static void Write(String^ str) {
		Console::Error->Write(str);
	}
};

template <typename TOut, typename TChar>
class CustomStreambuf : public std::basic_streambuf<TChar> {
public:

	// Constructor.
	// Initializes the internal buffers for reading and writing.
	explicit CustomStreambuf(size_t buffer_size = 1024) : read_buffer(buffer_size), write_buffer(buffer_size + 1) {
		// Initialize the read buffer pointers.
		// `setg(begin, current, end)`
		// The get area is initially empty, so all pointers are null.
		std::basic_streambuf<TChar>::setg(nullptr, nullptr, nullptr);

		// Initialize the write buffer pointers.
		// `setp(begin, end)`
		// `pbase()` points to the beginning of the buffer.
		// `epptr()` points one past the end of the buffer.
		TChar* base = write_buffer.data();
		TChar* end = base + write_buffer.size();
		std::basic_streambuf<TChar>::setp(base, end);
	}

private:
	// Internal buffers.
	std::vector<TChar> read_buffer;
	std::vector<TChar> write_buffer;

	// This function is called when the input buffer (get area) is exhausted.
	// It is responsible for filling the buffer from the data source.
	std::basic_streambuf<TChar>::int_type underflow() override {
		// If the get area is not exhausted, we can just return the next character.
		if (std::basic_streambuf<TChar>::gptr() < std::basic_streambuf<TChar>::egptr()) {
			return std::basic_streambuf<TChar>::traits_type::to_int_type(*std::basic_streambuf<TChar>::gptr());
		}

		// The get area is exhausted, so we read a single character from the source.
		int c = Console::Read();
		if (c == EOF) {
			return std::basic_streambuf<TChar>::traits_type::eof(); // No more data to read.
		}

		// Set the new pointers for the get area to a single character.
		read_buffer[0] = static_cast<TChar>(c);
		TChar* base = read_buffer.data();
		std::basic_streambuf<TChar>::setg(base, base, base + 1);

		// Return the first character from the new buffer.
		return std::basic_streambuf<TChar>::traits_type::to_int_type(*std::basic_streambuf<TChar>::gptr());
	}

	// This function is called when the output buffer is full.
	std::basic_streambuf<TChar>::int_type overflow(std::basic_streambuf<TChar>::int_type c = std::basic_streambuf<TChar>::traits_type::eof()) override {
		// Get the number of characters in the buffer.
		std::streamsize count = std::basic_streambuf<TChar>::pptr() - std::basic_streambuf<TChar>::pbase();

		if (count > 0) {
			// Write the entire buffer's content in a single call.
			//WriteToSource(pbase(), count);

			std::basic_string<TChar> outStr(std::basic_streambuf<TChar>::pbase(), count);
			TOut::Write(msclr::interop::marshal_as<String^>(outStr));
		}

		// Reset the put area pointers.
		std::basic_streambuf<TChar>::setp(write_buffer.data(), write_buffer.data() + write_buffer.size());

		// If c is not EOF, place it in the now-empty buffer.
		if (!std::basic_streambuf<TChar>::traits_type::eq_int_type(c, std::basic_streambuf<TChar>::traits_type::eof())) {
			*std::basic_streambuf<TChar>::pptr() = std::basic_streambuf<TChar>::traits_type::to_char_type(c);
			std::basic_streambuf<TChar>::pbump(1); // Advance the put pointer.
			return c;
		}

		// Return a non-EOF value to indicate success.
		return std::basic_streambuf<TChar>::traits_type::not_eof(c);
	}

	// This function is called to flush the contents of the output buffer.
	int sync() override {
		return overflow(std::basic_streambuf<TChar>::traits_type::eof());
	}
};

static InfinityOfficialNetwork::MediaServer::Native::Transcoding::NativeLibrary::NativeLibrary() {
	std::cout.set_rdbuf(new CustomStreambuf<StdOut, char>());
	std::cerr.set_rdbuf(new CustomStreambuf<StdErr, char>());
	std::clog.set_rdbuf(new CustomStreambuf<StdErr, char>());
	std::cin.set_rdbuf(new CustomStreambuf<StdOut, char>());
	std::wcout.set_rdbuf(new CustomStreambuf<StdOut, wchar_t>());
	std::wcerr.set_rdbuf(new CustomStreambuf<StdErr, wchar_t>());
	std::wclog.set_rdbuf(new CustomStreambuf<StdErr, wchar_t>());
	std::wcin.set_rdbuf(new CustomStreambuf<StdOut, wchar_t>());
}

END_MANAGED

inline void InfinityOfficialNetwork::MediaServer::Native::Transcoding::NativeLibrary::Test(array<Byte>^ filedata) {
	msclr::interop::marshal_context context;

	std::span<Byte> bytes = context.marshal_as<std::span<Byte>>(filedata);

	//std::vector<Byte> buffer = msclr::interop::marshal_as<std::vector<Byte>>(filedata);

	//std::ifstream file("C:\\Users\\InfiniPLEX\\Videos\\den pobedy.mp4", std::ios::binary);
	//if (!file) {
	//	fprintf(stderr, "Could not open file to load into memory.\n");
	//	return;
	//}
	//std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
	//	std::istreambuf_iterator<char>());

	//file.close();

	// Setup the MemoryBuffer struct
	MemoryBuffer mem_buffer;
	mem_buffer.data = reinterpret_cast<const uint8_t*>(bytes.data());
	mem_buffer.size = bytes.size();
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
		std::cerr << "Failed to allocate AVIO context" << std::endl;
		return;
	}

	// Allocate and configure the AVFormatContext
	AVFormatContext* fmt_ctx = avformat_alloc_context();
	if (!fmt_ctx) {
		std::cerr << "Failed to allocate AVFormatContext" << std::endl;
		avio_context_free(&avio_ctx);
		return;
	}

	// Set the AVFormatContext's I/O context to our custom one
	fmt_ctx->pb = avio_ctx;

	// Open the input (pass NULL as filename since we are using a custom I/O context)
	int ret = avformat_open_input(&fmt_ctx, NULL, NULL, NULL);
	if (ret < 0) {
		char buffer[64];
		av_make_error_string(buffer, sizeof(buffer), ret);
		std::cerr << "Could not open source from memory: %s" << std::endl;
		avformat_close_input(&fmt_ctx); // This also frees the AVIOContext and its buffer
		return;
	}

	// Rest of your code for finding stream info and processing metadata...
	// Read stream information
	ret = avformat_find_stream_info(fmt_ctx, NULL);
	if (ret < 0) {
		std::cerr << "Could not find stream information" << std::endl;
		avformat_close_input(&fmt_ctx);
		return;
	}

	std::map<std::string, std::string> metadataDict;

	// Get the metadata dictionary
	AVDictionary* metadata = fmt_ctx->metadata;
	if (metadata) {
		AVDictionaryEntry* entry = NULL;
		// Iterate through all entries in the dictionary
		while ((entry = av_dict_get(metadata, "", entry, AV_DICT_IGNORE_SUFFIX))) {
			std::cout << entry->key << ": " << entry->value << std::endl;
			metadataDict.insert({ std::string(entry->key), std::string(entry->value) });
		}
	}

	// Clean up. Note: avformat_close_input automatically frees the custom AVIOContext.
	// If you need to manually free it, use avio_context_free and av_free
	avformat_close_input(&fmt_ctx);
	av_free(avio_ctx->buffer);
	avio_context_free(&avio_ctx);
}
