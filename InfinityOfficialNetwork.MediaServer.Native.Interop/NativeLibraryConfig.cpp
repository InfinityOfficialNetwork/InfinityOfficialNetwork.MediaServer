#include "pch.h"
#include "NativeLibraryConfig.h"

#include <memory>

#pragma managed(push,off)
#include "../InfinityOfficialNetwork.MediaServer.Native.Core/NativeLibraryConfig.h"
#pragma managed(pop)

using namespace System;
using namespace InfinityOfficialNetwork::MediaServer::Native;

#pragma managed

namespace {
	struct ManagedLoggingProvider : Core::LoggingProvider {
		gcroot<Action<Interop::AvLogLevel, String^, String^,int>^> logger;
		
		// Inherited via LoggingProvider
		void LogMessage(Core::AvLogLevel level, std::string message, std::string source, int thread) override
		{
			String^ m_message = msclr::interop::marshal_as<String^>(message),
				^ m_source = msclr::interop::marshal_as<String^>(source);
			Interop::AvLogLevel m_level = (Interop::AvLogLevel)level;

			logger->Invoke(m_level, m_message, m_source, thread);
		}

		ManagedLoggingProvider(gcroot<Action<Interop::AvLogLevel, String^, String^,int>^> logger) : logger(logger) {}
	};
}


void InfinityOfficialNetwork::MediaServer::Native::Interop::NativeLibraryConfig::OnProcessExit(System::Object^ sender, System::EventArgs^ e) {
	ClearLogging();
}

static InfinityOfficialNetwork::MediaServer::Native::Interop::NativeLibraryConfig::NativeLibraryConfig() {
	System::AppDomain::CurrentDomain->ProcessExit += gcnew System::EventHandler(&OnProcessExit);
}

void Interop::NativeLibraryConfig::ConfigureLogging(Interop::LoggingConfiguration^ loggingConfiguration)
{
	std::shared_ptr<Core::LoggingProvider> n_logger = std::make_shared<ManagedLoggingProvider>(gcroot<Action<Interop::AvLogLevel, String^, String^,int>^>(loggingConfiguration->Logger));

	Core::AvLogLevel n_level = Core::AvLogLevel(loggingConfiguration->Level);
	std::map<std::string, Core::AvLogLevel> n_classLevelOverride;
	for each (auto i in loggingConfiguration->ClassLevelOverrides) {
		std::string key = msclr::interop::marshal_as<std::string>(i.Key);
		n_classLevelOverride.insert({ key,Core::AvLogLevel(i.Value)});
	}

	Core::LoggingConfiguration n_loggingConfiguration = { .Logger = n_logger, .Level = n_level, .ClassLevelOverrides = n_classLevelOverride };

	Core::NativeLibraryConfig::ConfigureLogging(n_loggingConfiguration);
}

void Interop::NativeLibraryConfig::ClearLogging()
{
	Core::NativeLibraryConfig::ClearLogging();
}
