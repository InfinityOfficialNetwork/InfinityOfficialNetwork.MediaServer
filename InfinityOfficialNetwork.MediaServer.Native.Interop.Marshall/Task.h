#pragma once

#include <msclr/marshal.h>
#include <boost/asio.hpp>

namespace msclr::interop
{
	template<>
	System::Threading::Tasks::Task^ marshal_as<System::Threading::Tasks::Task^, boost::asio::awaitable<void>> (const boost::asio::awaitable<void>& from)
	{

	}

	template<>
	boost::asio::awaitable<void> marshal_as<boost::asio::awaitable<void>, System::Threading::Tasks::Task^> (System::Threading::Tasks::Task^ const& from)
	{

	}
}

namespace InfinityOfficialNetwork::MediaServer::Native::Interop::Marshall
{

}