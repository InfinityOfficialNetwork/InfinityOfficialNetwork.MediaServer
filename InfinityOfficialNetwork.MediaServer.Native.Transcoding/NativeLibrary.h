#include "pch.h"
#include <vector>
#include <string>

// Include FFmpeg headers after standard headers
#include <libavutil/log.h>
#include <stdarg.h>



namespace InfinityOfficialNetwork::MediaServer::Native::Transcoding {

	using namespace System;
	using namespace System::Runtime::InteropServices;

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
		static void defaultLogHandler(String^, AvLogLevel, String^) {}
		static Action<String^, AvLogLevel, String^>^ s_logHandler = gcnew Action<String^,AvLogLevel, String^>(defaultLogHandler);

		static NativeLibrary();

	public:
		static void SetLogging(Action<String^, AvLogLevel, String^>^ handler);

		static void SetLoggingLevel(AvLogLevel level);


		static void ClearLogging();

		static void Test(array<Byte>^ file);
	};
}


