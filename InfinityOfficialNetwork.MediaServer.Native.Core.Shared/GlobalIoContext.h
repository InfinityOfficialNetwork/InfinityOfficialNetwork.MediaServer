#pragma once

#include "allocation.h"

#include "global.h"

#include <boost/asio.hpp>

namespace InfinityOfficialNetwork::MediaServer::Native::Core::Shared
{
	class __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_SHARED_API GlobalIoContext
	{
	public:
		static boost::asio::io_context IoContext;
	};
}

