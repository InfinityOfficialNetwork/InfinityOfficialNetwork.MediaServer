#include "NativeLibraryConfig.h"

#include <stdio.h>
#include <libavutil/log.h>
#include <libavformat/avformat.h>
#include <cstdarg>
#include <msclr/gcroot.h>

using namespace InfinityOfficialNetwork::MediaServer::Native::Interop;
using namespace System::Runtime::InteropServices;
using namespace System;

inline static void loggerTrampoline2 (void* avcl, int level, const char* str);

inline static std::unique_ptr<gcroot<NativeLibraryLoggingConfiguration^>> managedLogger = nullptr;

inline static bool loggerSet = false;

inline static void __cdecl loggerTrampoline (void* avcl, int level, const char* fmt, va_list list)
{
	loggerTrampoline2 (avcl, level, fmt);
}

inline static void loggerTrampoline2 (void* avcl, int level, const char* str)
{

}


void InfinityOfficialNetwork::MediaServer::Native::Interop::NativeLibraryConfig::SetLoggingConfiguration (NativeLibraryLoggingConfiguration^ config)
{
	managedLogger = std::make_unique<gcroot<NativeLibraryLoggingConfiguration^>> (config);

	if (!loggerSet)
	{
		av_log_set_callback (loggerTrampoline);
	}
}
