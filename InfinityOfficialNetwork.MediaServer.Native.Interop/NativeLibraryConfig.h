#pragma once

namespace InfinityOfficialNetwork::MediaServer::Native::Interop
{
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

	delegate void NativeLibraryLoggingProvider (AvLogLevel level, System::String^ message, System::String^ source, int thread);

	public ref class NativeLibraryLoggingConfiguration
	{
	public:
		NativeLibraryLoggingProvider^ Logger;
		AvLogLevel MinimumLogLevel;
		System::Collections::Generic::IDictionary<System::String^, AvLogLevel>^ ClassLevelOverrides;
	};

	public ref class NativeLibraryConfig
	{
	private:
		static System::Object^ libraryLock = gcnew System::Object ();

	public:
		static property System::Object^ LibraryLock {
			System::Object^ get ()
			{
				return libraryLock;
			}
		}

		static void SetLoggingConfiguration (NativeLibraryLoggingConfiguration^ config);
	};

}

