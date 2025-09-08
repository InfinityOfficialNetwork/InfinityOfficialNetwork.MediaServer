#include "MediaContainer.h"

using namespace InfinityOfficialNetwork::MediaServer::Native::Core;


MediaContainer::VectorHandle::VectorHandle(std::vector<unsigned char>&& v) : data(std::move(v)) {}

MediaContainer::VectorHandle::VectorHandle(const std::vector<unsigned char>& v) : data(v) {}

MediaContainer::MediaContainer(std::span<unsigned char>&& data, std::unique_ptr<HandleBase>&& handle)
	: handle(std::move(handle)), Data(std::move(data))
{
}

MediaContainer::MediaContainer(std::vector<unsigned char>&& data)
	: handle(std::move(std::make_unique<VectorHandle>(std::move(data)))),
	Data(((VectorHandle*)handle.get())->data.data(), ((VectorHandle*)handle.get())->data.size())
{
}

MediaContainer::MediaContainer(const std::vector<unsigned char> data)
	: handle(std::move(std::make_unique<VectorHandle>(data))),
	Data(((VectorHandle*)handle.get())->data.data(), ((VectorHandle*)handle.get())->data.size())
{
}
