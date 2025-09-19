#include "NativeLibraryConfig.h"

#include <memory>
#include <map>
#include <string>

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

static std::unique_ptr<LoggingConfiguration> _internal_loggingConfiguration = nullptr;

static std::shared_mutex lock;

static inline void __cdecl NativeCallbackTrampoline(void* avcl, int level, const char* fmt, va_list vl) {
	if (!_internal_loggingConfiguration)
		return;

	if (level > (int)_internal_loggingConfiguration->Level)
		return;

	const char* class_name = avcl ? av_default_item_name(avcl) : "";

	auto key = _internal_loggingConfiguration->ClassLevelOverrides.find(class_name);
	if (key != _internal_loggingConfiguration->ClassLevelOverrides.end() && (int)key->second > level)
		return;

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

	static const std::string lookup = std::string(" \t\n\r").append(1, '\0');
	size_t last_pos = buffer.find_last_not_of(lookup);
	if (std::string::npos == last_pos)
		last_pos = 0;
	buffer.resize(last_pos);

	std::thread::id threadId = std::this_thread::get_id();
	int i_threadId = atoi((std::ostringstream() << threadId).str().c_str());

	{
		std::unique_lock guard(lock);
		_internal_loggingConfiguration->Logger->LogMessage((AvLogLevel)level, buffer, class_name, i_threadId);
	}
}

void NativeLibraryConfig::ConfigureLogging(LoggingConfiguration loggingConfiguration)
{
	avformat_network_init();
	_internal_loggingConfiguration = std::make_unique<LoggingConfiguration>(loggingConfiguration);
	av_log_set_callback(NativeCallbackTrampoline);
}

void NativeLibraryConfig::ClearLogging()
{
	_internal_loggingConfiguration.reset();
	av_log_set_callback(av_log_default_callback);
}

