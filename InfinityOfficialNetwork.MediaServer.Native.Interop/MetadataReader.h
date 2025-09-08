#pragma once

#include "MediaContainer.h"

namespace InfinityOfficialNetwork::MediaServer::Native::Interop {

	public ref class MediaMetadata {
	public:
		property System::Collections::Generic::IDictionary<System::String^, System::String^>^ Metadata;
	};

	public ref class MetadataReader
	{
	public:
		enum class ErrorCode {
			AvioContextAllocateError,
			AvFormatContextAllocateError,
			AvFormatOpenInputError,
			AvFormatFindStreamInfoError,
		};

		ref class MetadataReaderException : System::Exception {
		public:
			property MetadataReader::ErrorCode ErrorCode;
			property int AvErrorCode;

		internal:
			MetadataReaderException(MetadataReader::ErrorCode errorCode, int avErrorCode);
		};

		static MediaMetadata^ GetMetadata(MediaContainer^ container);
	};

}