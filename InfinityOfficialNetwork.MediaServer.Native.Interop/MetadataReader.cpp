#include "pch.h"
#include "MetadataReader.h"

#include "../InfinityOfficialNetwork.MediaServer.Native.Core/MetadataReader.h"

using namespace InfinityOfficialNetwork::MediaServer::Native;
using namespace System::Runtime::InteropServices;
using namespace System;
using namespace System::Collections::Generic;

namespace {
	struct ManagedHandle : Core::MediaContainer::HandleBase {
		gcroot<Interop::MediaContainer^> container;
		gcroot<GCHandle^> pinnedData;

		ManagedHandle(gcroot<Interop::MediaContainer^> container) :
			container(container),
			pinnedData(GCHandle::Alloc(container->Data, GCHandleType::Pinned))
		{
		}
	};
}

Interop::MediaMetadata^ Interop::MetadataReader::GetMetadata(Interop::MediaContainer^ container)
{
	std::unique_ptr<ManagedHandle> n_handle = std::make_unique<ManagedHandle>(container);
	pin_ptr<Byte> data_ptr = (&n_handle->container->Data[0]);
	std::span<Byte> n_data((Byte*)data_ptr, n_handle->container->Data.Length);

	Core::MediaContainer n_mediaContainer(std::move(n_data), std::move(n_handle));

	std::variant<Core::MediaMetadata, std::tuple<Core::MetadataReader::ErrorCode, int>> result = Core::MetadataReader::GetMetadata(n_mediaContainer);

	if (result.index() == 1) {
		throw gcnew MetadataReaderException((ErrorCode)std::get<0>(std::get<1>(result)), std::get<1>(std::get<1>(result)));
	}

	Core::MediaMetadata verified_result = std::get<0>(result);

	Dictionary<String^, String^>^ m_metadata = gcnew Dictionary<String^, String^>(0);

	for (const auto& i : verified_result.Metadata) {
		m_metadata->Add(msclr::interop::marshal_as<String^>(i.first),
			msclr::interop::marshal_as<String^>(i.second));
	}

	auto ret = gcnew Interop::MediaMetadata();
	ret->Metadata = m_metadata;

	return ret;
}

InfinityOfficialNetwork::MediaServer::Native::Interop::MetadataReader::MetadataReaderException::MetadataReaderException(MetadataReader::ErrorCode errorCode, int avErrorCode)
	: System::Exception(msclr::interop::marshal_as<String^>(
		std::format("An exception occurred while reading metadata. Code: {}; AvCode {}",
			msclr::interop::marshal_as<std::string>(errorCode.ToString()),
			avErrorCode)))
{
	ErrorCode = errorCode;
	AvErrorCode = avErrorCode;
}
