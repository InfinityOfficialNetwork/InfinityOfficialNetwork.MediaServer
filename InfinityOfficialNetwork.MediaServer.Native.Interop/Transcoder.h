#pragma once

#include "MediaContainer.h"

namespace InfinityOfficialNetwork::MediaServer::Native::Interop {

	public ref class Transcoder
	{
	public:
		static MediaContainer^ Transcode(Interop::MediaContainer^ input);
		static System::Threading::Tasks::Task<MediaContainer^>^ TranscodeAsync(Interop::MediaContainer^ input);
	};

}