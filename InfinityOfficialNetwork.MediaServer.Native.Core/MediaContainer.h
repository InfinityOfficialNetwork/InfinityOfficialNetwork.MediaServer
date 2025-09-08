#pragma once

#include "global.h"

#include <span>
#include <memory>

namespace InfinityOfficialNetwork::MediaServer::Native::Core {


	class __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API MediaContainer
	{
	public:

		struct __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API HandleBase {
			virtual ~HandleBase() {};
		};

	private:
		std::unique_ptr<HandleBase> handle;


		struct __INFINITYOFFICIALNETWORK_MEDIASERVER_NATIVE_CORE_API VectorHandle : HandleBase {
			std::vector<unsigned char> data;
			VectorHandle(std::vector<unsigned char>&& v);
			VectorHandle(const std::vector<unsigned char>& v);
		};

	public:
		std::span<unsigned char> Data;

		MediaContainer(std::span<unsigned char>&& data, std::unique_ptr<HandleBase>&& handle);

		MediaContainer(std::vector<unsigned char>&& data);

		MediaContainer(const std::vector<unsigned char> data);
	};

}