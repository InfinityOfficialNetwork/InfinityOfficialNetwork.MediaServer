#pragma once

#include "global.h"

#include <string>
#include <map>
#include <variant>

#include "MediaContainer.h"

namespace InfinityOfficialNetwork::MediaServer::Native::Core {


	struct __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API MediaMetadata {
		std::map<std::string, std::string> Metadata;
	};


	class __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API MetadataReader
	{
	public:
		enum class __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API ErrorCode {
			AvioContextAllocateError,
			AvFormatContextAllocateError,
			AvFormatOpenInputError,
			AvFormatFindStreamInfoError,
		};

		static std::variant<MediaMetadata, std::tuple<ErrorCode, int>> GetMetadata(const MediaContainer& container);

	};

}