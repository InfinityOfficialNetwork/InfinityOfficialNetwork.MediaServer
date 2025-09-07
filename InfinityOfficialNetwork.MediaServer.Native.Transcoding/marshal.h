#pragma once

#include "pch.h"

BEGIN_MANAGED

namespace msclr {
	namespace interop {

		using namespace System;
		using namespace System::Runtime::InteropServices;

		//byte[] <-> std::vector<Byte>

		template<>
		array<Byte>^ marshal_as<array<Byte>^, std::vector<Byte>>(const std::vector<Byte>& from) {
			array<Byte>^ bytes = gcnew array<System::Byte>(from.size());
			pin_ptr<Byte> bytes_ptr = &bytes[0];
			memcpy(bytes_ptr, from.data(), from.size());
			return bytes;
		}

		template<>
		std::vector<Byte> marshal_as<std::vector<Byte>, array<Byte>^>(array<Byte>^ const& from) {
			std::vector<Byte> bytes = std::vector<Byte>(from->Length);
			pin_ptr<Byte> bytes_ptr = &from[0];
			memcpy(bytes.data(), bytes_ptr, from->Length);
			return bytes;
		}

		//byte[] -> std::span<Byte>

		template<>
		ref class context_node<std::span<Byte>, array<Byte>^> : public context_node_base
		{
			using TO = std::span<Byte>;
			using FROM = array<Byte>^;
		private:
			GCHandle pinHandle;
		public:
			context_node(TO& toObject, FROM fromObject)
			{
				pinHandle = GCHandle::Alloc(fromObject, GCHandleType::Pinned);
				void* ptr = pinHandle.AddrOfPinnedObject().ToPointer();
				toObject = std::span<System::Byte>(static_cast<System::Byte*>(ptr), fromObject->Length);
			}
			~context_node()
			{
				this->!context_node();
			}
		protected:
			!context_node()
			{
				pinHandle.Free();
			}
		};
	}
}

END_MANAGED