#pragma once

#include "marshal.h"

namespace InfinityOfficialNetwork::MediaServer::Native::Transcoding {

	using namespace System;
	using namespace System::Threading::Tasks;
	using namespace System::IO;
	using namespace System::Threading;

	ref class AvContainer
	{
	private:

	internal:
		msclr::interop::marshal_context context;
		std::span<Byte>* data;

	public:
		AvContainer(array<Byte>^ data)
		{
			this->data = new std::span<Byte>(context.marshal_as<std::span<Byte>>(data));
		}

		static Task<AvContainer^>^ FromStreamAsync(Stream^ stream) {
			throw gcnew System::NotImplementedException();

			
		}

		static AvContainer^ FromStream(Stream^ stream) {
			msclr::interop::marshal_context inner_context;

			array<Byte>^ buffer_backing = gcnew array<Byte>(1024 * 1024);
			Span<Byte> buffer;

			std::deque<Byte> data;

			try {
				while (int count = stream->Read(buffer)) {
					std::span<Byte> span = inner_context.marshal_as<std::span<Byte>>(buffer_backing);
					std::copy(span.begin(), span.end(), std::back_inserter(data));
				}
			}
			catch (EndOfStreamException^) {}

			array<Byte>^ bytes = gcnew array<Byte>(data.size());

			for (long i = 0, size = data.size(); i < size; ++i) {
				bytes[i] = data[i];
			}

			return gcnew AvContainer(bytes);
		}

		!AvContainer() {
			delete data;
		}

		~AvContainer() {
			this->!AvContainer();
		}
	};
}
