#pragma once

#include "allocation.h"

#include "GlobalIoContext.h"

namespace InfinityOfficialNetwork::MediaServer::Native::Core::Shared
{
	class NativeThreadPool
	{


	public:
		void SetMinThreadCount (int64_t count);
		void SetMaxThreadCount (int64_t count);

	};


}
