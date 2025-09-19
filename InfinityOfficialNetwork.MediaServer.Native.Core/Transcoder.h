#pragma once

#include "global.h"

namespace InfinityOfficialNetwork::MediaServer::Native::Core {

	struct __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API TranscodingOptions {

	};

	struct __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API TranscoderCompletionCallback {
		virtual void OnCompletion(std::shared_ptr<std::vector<unsigned char>> ret) = 0;
	};

	class __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API Transcoder
	{
	public:
		static std::shared_ptr<std::vector<unsigned char>> Transcode(std::span<unsigned char> inputFilename);
		static void TranscodeAsync(std::span<unsigned char> inputFile, std::shared_ptr<TranscoderCompletionCallback> callback);
	};


}