#include "pch.h"
#include <vector>
#include <string>

// Include FFmpeg headers after standard headers
#include <libavutil/log.h>
#include <stdarg.h>

using namespace System;
using namespace System::Runtime::InteropServices;



// Declare the unmanaged static field in the global namespace.
static void* s_logCallbackPtr = nullptr;

// Forward-declare the C-style trampoline function.
extern "C" void __cdecl NativeLogCallbackEntry(void* avcl, int level, const char* fmt, va_list vl);

namespace InfinityOfficialNetwork::MediaServer::Native::Transcoding {

	// Define the managed delegate for the __stdcall function.
	[UnmanagedFunctionPointer(CallingConvention::StdCall)]
	public delegate void ManagedLogCallbackDelegate(const char* message, int level);

	public enum class AvLogLevel : SByte
	{
		AvLogQuiet = AV_LOG_QUIET,
		AvLogPanic = AV_LOG_PANIC,
		AvLogFatal = AV_LOG_FATAL,
		AvLogError = AV_LOG_ERROR,
		AvLogWarning = AV_LOG_WARNING,
		AvLogInfo = AV_LOG_INFO,
		AvLogVerbose = AV_LOG_VERBOSE,
		AvLogDebug = AV_LOG_DEBUG,
		AvLogTrace = AV_LOG_TRACE,
	};

	public ref class NativeLibrary
	{
	internal:
		// A simple, static member to hold the user's logging handler.
		static Action<String^, AvLogLevel>^ s_logHandler;

		// GCHandle to prevent the ManagedLogCallbackDelegate from being garbage collected.
		static GCHandle s_delegateHandle;

	internal:
		// This is the managed helper function that will be the body of our __stdcall delegate.
		static void __clrcall ManagedLogCallbackImpl(const char* message, int level)
		{
			if (s_logHandler != nullptr)
			{
				s_logHandler->Invoke((gcnew String(message))->TrimEnd(L'\n'), (AvLogLevel)level);
			}
		}

	public:
		static void SetLogging(Action<String^, AvLogLevel>^ handler)
		{
			s_logHandler = handler;

			// Create an instance of the managed delegate.
			ManagedLogCallbackDelegate^ callback = gcnew ManagedLogCallbackDelegate(NativeLibrary::ManagedLogCallbackImpl);

			// Allocate a GCHandle to pin the delegate and prevent it from being garbage collected.
			s_delegateHandle = GCHandle::Alloc(callback, GCHandleType::Normal);

			// Get the function pointer and store it in the unmanaged static field.
			s_logCallbackPtr = Marshal::GetFunctionPointerForDelegate(callback).ToPointer();

			// Set the C-style trampoline function as the callback.
			av_log_set_callback(NativeLogCallbackEntry);

		}

		static void SetLoggingLevel(AvLogLevel level) {
			av_log_set_level((int)level);
		}


		static void ClearLogging()
		{
			av_log_set_callback(nullptr);
			s_logHandler = nullptr;

			if (s_delegateHandle.IsAllocated)
			{
				s_delegateHandle.Free();
			}
			s_logCallbackPtr = nullptr;
		}

		static void Test() {
			// Define the filename
			const char* filename = R"(C:\Users\InfiniPLEX\Videos\den pobedy.mp4)";

			// Allocate an AVFormatContext
			AVFormatContext* fmt_ctx = NULL;

			// Open the input file and populate the AVFormatContext
			int ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL);
			if (ret < 0) {
				fprintf(stderr, "Could not open source file %s\n", filename);
				return;
			}

			// Read stream information
			ret = avformat_find_stream_info(fmt_ctx, NULL);
			if (ret < 0) {
				fprintf(stderr, "Could not find stream information\n");
				avformat_close_input(&fmt_ctx);
				return;
			}

			// Get the metadata dictionary
			AVDictionary* metadata = fmt_ctx->metadata;
			if (metadata) {
				AVDictionaryEntry* entry = NULL;
				// Iterate through all entries in the dictionary
				while ((entry = av_dict_get(metadata, "", entry, AV_DICT_IGNORE_SUFFIX))) {
					printf("%s: %s\n", entry->key, entry->value);
				}
			}

			// Clean up
			avformat_close_input(&fmt_ctx);
		}
	};
}

class managed_streambuf : std::streambuf {
	virtual int_type __CLR_OR_THIS_CALL overflow(int_type ch) { 
		Console::Out->Write(ch);
	}

	virtual int_type __CLR_OR_THIS_CALL underflow() { 
	}
};

#pragma managed(push,off)

class LogCallbackBase {
	virtual void Write(std::string str, int level) = 0;
};

#pragma managed(pop)
#pragma managed(push,on)


class LogCallbackImpl : LogCallbackBase {
	// Inherited via LogCallbackBase
	void Write(std::string str, int level) override
	{
		InfinityOfficialNetwork::MediaServer::Native::Transcoding::NativeLibrary::s_logHandler->Invoke(gcnew String(str.c_str()), InfinityOfficialNetwork::MediaServer::Native::Transcoding::AvLogLevel(level));
	}
};

static LogCallbackImpl logCallbackImpl = LogCallbackImpl();

#pragma managed(pop)

// ====================================================================
// CRITICAL: This is the unmanaged section.
// The code in this section is compiled as pure native C++.
// ====================================================================
#pragma managed(push, off)

// The unmanaged trampoline function must be defined here.
// It is in the global namespace to avoid managed scoping issues.
void __cdecl NativeLogCallbackEntry(void* avcl, int level, const char* fmt, va_list vl)
{
	if (level > av_log_get_level()) {
		return;
	}

	// Check if the pointer has been set by the managed side.
	if (!s_logCallbackPtr) {
		return;
	}

	// Perform all string formatting on the unmanaged side.
	va_list args_copy;
	va_copy(args_copy, vl);
	int size = vsnprintf(nullptr, 0, fmt, args_copy);
	va_end(args_copy);

	if (size <= 0) {
		return;
	}

	std::vector<char> buffer(size + 1);
	vsnprintf(buffer.data(), buffer.size(), fmt, vl);

	// CRITICAL CHANGE: Get the function pointer directly from the static field.
	typedef void(__stdcall* LogFuncPtr)(const char*, int);
	LogFuncPtr logFunc = (LogFuncPtr)s_logCallbackPtr;
	logFunc(buffer.data(), level);
}

#pragma managed(pop) // Restore pragma state