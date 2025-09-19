#include "pch.h"
#include "MediaContainer.h"

using namespace InfinityOfficialNetwork::MediaServer::Native;
using namespace System::Runtime::InteropServices;
using namespace System::Runtime::CompilerServices;
using namespace System;

Interop::ArrayBackedMediaContainer::ArrayBackedMediaContainer(array<System::Byte>^ data) {
	array_data = data;
	array_data_handle = GCHandle::Alloc(data, GCHandleType::Pinned);

	pin_ptr<System::Byte> pinnedPtr = &array_data[0];
}

InfinityOfficialNetwork::MediaServer::Native::Interop::NativeBackedMediaContainer::NativeBackedMediaContainer(std::shared_ptr<std::vector<unsigned char>> data) {
	native_data = new std::shared_ptr<std::vector<unsigned char>>(std::move(data));
}
