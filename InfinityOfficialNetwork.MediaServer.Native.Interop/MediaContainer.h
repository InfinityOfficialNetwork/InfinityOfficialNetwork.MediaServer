#pragma once

#include <vector>
#include <memory>

namespace InfinityOfficialNetwork::MediaServer::Native::Interop {

	public ref class MediaContainer abstract
	{
	public:
		property System::Span<System::Byte> Data {
			virtual System::Span<System::Byte> get() = 0;
		}
	};

	public ref class ArrayBackedMediaContainer : MediaContainer
	{
		array<System::Byte>^ array_data;
		System::Runtime::InteropServices::GCHandle array_data_handle;
	public:
		property System::Span<System::Byte> Data {
			virtual System::Span<System::Byte> get() override {
				return System::Span<System::Byte>(array_data);
			}
		}

		ArrayBackedMediaContainer(array<System::Byte>^ data);

		~ArrayBackedMediaContainer() {
			this->!ArrayBackedMediaContainer();
		}

		!ArrayBackedMediaContainer() {
			array_data_handle.Free();
		}
	};

	public ref class NativeBackedMediaContainer : MediaContainer
	{
		std::shared_ptr<std::vector<unsigned char>>* native_data;
	public:
		property System::Span<System::Byte> Data {
			virtual System::Span<System::Byte> get() override {
				return System::Runtime::InteropServices::MemoryMarshal::CreateSpan(System::Runtime::CompilerServices::Unsafe::AsRef<System::Byte>(native_data->get()->data()), native_data->get()->size());
			}
		}

		NativeBackedMediaContainer(std::shared_ptr<std::vector<unsigned char>> data);

		~NativeBackedMediaContainer() {
			this->!NativeBackedMediaContainer();
		}

		!NativeBackedMediaContainer() {
			delete native_data;
		}
	};

	//public ref class NativeMemoryManager : System::Buffers::MemoryManager<System::Byte>
	//{
	//private:
	//	std::shared_ptr<std::vector<unsigned char>>* _handle;

	//public:
	//	NativeMemoryManager(std::shared_ptr<std::vector<unsigned char>> data)
	//	{
	//		_handle = new std::shared_ptr<std::vector<unsigned char>>(std::move(data));
	//	}

	//	// This method is the bridge. It returns a Span to the unmanaged memory.
	//	virtual System::Span<System::Byte> GetSpan() override
	//	{
	//		return MemoryMarshal::CreateSpan(Unsafe::AsRef<Byte>(_handle->get()->data()), _handle->get()->size());
	//	}

	//	// Pin the underlying memory
	//	virtual System::Buffers::MemoryHandle Pin(int elementIndex) override
	//	{
	//		// This is a simplified example. For a real implementation, you'd handle pinning
	//		// more carefully. In our case, the memory is unmanaged, so it's already "pinned".
	//		unsigned char* rawPtr = _handle->get()->data();
	//		unsigned char* pinnedPtr = rawPtr + elementIndex;
	//		return System::Buffers::MemoryHandle(pinnedPtr, GCHandle::FromIntPtr((System::IntPtr)pinnedPtr),nullptr);
	//	}

	//	// Unpin the memory
	//	virtual void Unpin() override
	//	{
	//	}

	//protected:
	//	~NativeMemoryManager() {
	//		this->!NativeMemoryManager();
	//	}

	//	!NativeMemoryManager() {
	//		delete _handle;
	//	}
	//};

}