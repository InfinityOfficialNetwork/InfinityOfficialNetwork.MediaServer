#pragma once

namespace InfinityOfficialNetwork::MediaServer::Native::Interop {
	public enum class AvLogLevel
	{
		AvLogQuiet = -8,
		AvLogPanic = 0,
		AvLogFatal = 8,
		AvLogError = 16,
		AvLogWarning = 24,
		AvLogInfo = 32,
		AvLogVerbose = 40,
		AvLogDebug = 48,
		AvLogTrace = 56,
	};

	public ref class LoggingConfiguration
	{
	public:
		System::Action<AvLogLevel, System::String^, System::String^,int>^ Logger;
		AvLogLevel Level;
		System::Collections::Generic::IDictionary<System::String^, AvLogLevel>^ ClassLevelOverrides;
	};

	public ref class NativeLibraryConfig
	{
		static void OnProcessExit(System::Object^ sender, System::EventArgs^ e);

		static NativeLibraryConfig();

	public:
		static void ConfigureLogging(LoggingConfiguration^ loggingConfiguration);
		static void ClearLogging();
	};

}


