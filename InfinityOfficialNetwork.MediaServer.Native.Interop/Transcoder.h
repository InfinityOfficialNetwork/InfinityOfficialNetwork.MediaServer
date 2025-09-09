#pragma once

namespace InfinityOfficialNetwork::MediaServer::Native::Interop {

	public ref class Transcoder
	{
	public:
		static void Transcode(System::Span<System::Byte> input, System::String^ output);
		//static System::Threading::Tasks::Task^ TranscodeAsync(System::String^ input, System::String^ output);
	};

}