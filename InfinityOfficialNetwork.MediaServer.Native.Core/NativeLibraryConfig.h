#pragma once

#include "global.h"

#include <memory>
#include <map>
#include <string>



namespace InfinityOfficialNetwork::MediaServer::Native::Core {


	enum class __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API AvLogLevel
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


	class __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API LoggingProvider
	{
	public:
		virtual void LogMessage(AvLogLevel level, std::string message, std::string source) = 0;
	};


	struct __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API LoggingConfiguration
	{
		std::shared_ptr<LoggingProvider> Logger;
		AvLogLevel Level;
		std::map<std::string, AvLogLevel> ClassLevelOverrides;
	};

	class __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API NativeLibraryConfig
	{
	public:
		static void ConfigureLogging(LoggingConfiguration loggingConfiguration);
		static void ClearLogging();
	};

}