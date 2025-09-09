#include "pch.h"
#include "Transcoder.h"

#include "../InfinityOfficialNetwork.MediaServer.Native.Core/Transcoder.h"

using namespace InfinityOfficialNetwork::MediaServer::Native;
using namespace System;
using namespace System::Threading;
using namespace System::Threading::Tasks;

namespace {
	struct TaskCallback : Core::TranscoderCompletionCallback {
		gcroot<TaskCompletionSource^> tcs;
		virtual void OnCompletion() override {
			tcs->SetResult();
		}

		TaskCallback(gcroot<TaskCompletionSource^> tcs) : tcs(tcs) {}
	};
}

void Interop::Transcoder::Transcode(System::Span<System::Byte> input, System::String^ output)
{
	unsigned char% ref = input.GetPinnableReference();
	pin_ptr<System::Byte> pinnedPtr = &ref;

	std::span<unsigned char> n_data((unsigned char*)pinnedPtr, input.Length);

	std::string n_output = msclr::interop::marshal_as<std::string>(output);

	Core::Transcoder::Transcode(n_data, n_output);
}

//System::Threading::Tasks::Task^ Interop::Transcoder::TranscodeAsync(System::String^ input, System::String^ output)
//{
//	std::string n_input = msclr::interop::marshal_as<std::string>(input),
//		n_output = msclr::interop::marshal_as<std::string>(output);
//	
//	TaskCompletionSource^ tcs = gcnew TaskCompletionSource();
//
//	Core::Transcoder::TranscodeAsync(n_input, n_output, std::move((std::shared_ptr<Core::TranscoderCompletionCallback>)std::make_shared<TaskCallback>(gcroot<TaskCompletionSource^>(tcs))));
//
//	return tcs->Task;
//}
