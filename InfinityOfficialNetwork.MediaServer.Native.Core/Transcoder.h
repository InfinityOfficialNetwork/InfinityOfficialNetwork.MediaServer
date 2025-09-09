#pragma once

#include "global.h"

namespace InfinityOfficialNetwork::MediaServer::Native::Core {

	struct __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API TranscodingOptions {

	};

	struct __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API TranscoderCompletionCallback {
		virtual void OnCompletion() = 0;
	};

	class __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API Transcoder
	{
	public:
		static void Transcode(std::span<unsigned char> inputFilename, const std::string& outputFilename);
		static void TranscodeAsync(std::span<unsigned char> inputFilename, const std::string& outputFilename, std::shared_ptr<TranscoderCompletionCallback>&& callback);
	};


}