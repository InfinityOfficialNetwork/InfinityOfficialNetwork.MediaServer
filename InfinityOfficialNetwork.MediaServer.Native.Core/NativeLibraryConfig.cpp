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

	size_t last_pos = buffer.find_last_not_of(" \t\n\r");
	if (std::string::npos != last_pos)
		buffer.resize(last_pos + 1);

	_internal_loggingConfiguration->Logger->LogMessage((AvLogLevel)level, buffer, class_name);
}

void NativeLibraryConfig::ConfigureLogging(LoggingConfiguration loggingConfiguration)
{
	_internal_loggingConfiguration = std::make_unique<LoggingConfiguration>(loggingConfiguration);
	av_log_set_callback(NativeCallbackTrampoline);
}

void NativeLibraryConfig::ClearLogging()
{
	_internal_loggingConfiguration.reset();
	av_log_set_callback(av_log_default_callback);
}

