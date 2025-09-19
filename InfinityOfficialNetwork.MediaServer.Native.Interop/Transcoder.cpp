#include "pch.h"
#include "Transcoder.h"

#include "../InfinityOfficialNetwork.MediaServer.Native.Core/Transcoder.h"

using namespace InfinityOfficialNetwork::MediaServer::Native;
using namespace System;
using namespace System::Threading;
using namespace System::Threading::Tasks;

namespace {
	struct TaskCallback : Core::TranscoderCompletionCallback {
		gcroot<TaskCompletionSource<Interop::MediaContainer^>^> tcs;
		virtual void OnCompletion(std::shared_ptr<std::vector<unsigned char>> result) override {
			Interop::MediaContainer^ ret = gcnew Interop::NativeBackedMediaContainer(std::move(result));
			tcs->SetResult(ret);
		}

		TaskCallback(gcroot<TaskCompletionSource<Interop::MediaContainer^>^> tcs) : tcs(tcs) {}
	};
}

Interop::MediaContainer^ Interop::Transcoder::Transcode(Interop::MediaContainer^ input)
{
	unsigned char% ref = input->Data.GetPinnableReference();
	pin_ptr<System::Byte> pinnedPtr = &ref;

	std::span<unsigned char> n_data((unsigned char*)pinnedPtr, input->Data.Length);

	std::shared_ptr<std::vector<unsigned char>> result = std::move(Core::Transcoder::Transcode(n_data));



	Interop::MediaContainer^ ret = gcnew Interop::NativeBackedMediaContainer(std::move(result));

	return ret;
}

System::Threading::Tasks::Task<Interop::MediaContainer^>^ Interop::Transcoder::TranscodeAsync(Interop::MediaContainer^ input)
{
	unsigned char% ref = input->Data.GetPinnableReference();
	pin_ptr<System::Byte> pinnedPtr = &ref;

	std::span<unsigned char> n_data((unsigned char*)pinnedPtr, input->Data.Length);
	
	TaskCompletionSource<Interop::MediaContainer^>^ tcs = gcnew TaskCompletionSource<Interop::MediaContainer^>();

	Core::Transcoder::TranscodeAsync(n_data, std::move((std::shared_ptr<Core::TranscoderCompletionCallback>)std::make_shared<TaskCallback>(gcroot<TaskCompletionSource<Interop::MediaContainer^>^>(tcs))));

	return tcs->Task;
}
