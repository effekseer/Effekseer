#if !(defined(__EFFEKSEER_NETWORK_DISABLED__))

#include "Effekseer.Session.h"

namespace Effekseer
{

Session::Session()
{
}

Session::~Session()
{
	Close();
}

void Session::Open(Socket* socket)
{
	socket_ = socket;
	thread_ = std::thread([this](){ RecvThread(); });
}

void Session::Close()
{
	if (socket_)
	{
		socket_->Close();
		socket_ = nullptr;
	}
	if (thread_.joinable())
	{
		thread_.join();
	}

	responseHandlers_.clear();
}

void Session::Update()
{
	std::lock_guard<std::mutex> lock(mutex_);

	for (auto& data : queuedRequests_)
	{
		PacketHeader& header = std::get<0>(data);
		Payload& payload = std::get<1>(data);

		auto it = requestHandlers_.find(header.requestID);
		if (it != requestHandlers_.end())
		{
			Request req{ header.code, std::move(payload) };
			Response res{};

			it->second(req, res);

			PacketHeader resHeader{};
			resHeader.Set(PacketType::Responce, header.requestID, header.transactionID, res.code, (uint32_t)res.payload.size());
			socket_->Send(&resHeader, (int32_t)sizeof(resHeader));
			socket_->Send(res.payload.data(), (int32_t)res.payload.size());
		}
	}
	queuedRequests_.clear();

	for (auto& data : queuedResponses_)
	{
		PacketHeader& header = std::get<0>(data);
		Payload& payload = std::get<1>(data);

		auto it = responseHandlers_.find(header.transactionID);
		if (it != responseHandlers_.end())
		{
			Response res{ header.code, std::move(payload) };

			it->second(res);
		}
	}
	queuedResponses_.clear();
}

void Session::RecvThread()
{
	while (socket_ != nullptr)
	{
		int32_t recvSize = socket_->Recv(&packetBuffer_[packetBufferFilled_], (int32_t)(sizeof(packetBuffer_) - packetBufferFilled_));

		if (recvSize <= 0)
		{
			break;
		}
		else
		{
			packetBufferFilled_ += recvSize;
		}

		size_t offset = 0;
		while (offset < packetBufferFilled_)
		{
			const uint8_t* dataPtr = &packetBuffer_[offset];
			const size_t dataSize = packetBufferFilled_ - offset;

			if (state_ == State::SearchPacket)
			{
				if (dataSize < sizeof(PacketHeader))
				{
					break;
				}

				PacketHeader header{};
				memcpy(&header, dataPtr, sizeof(PacketHeader));

				if (header.IsValid(PacketType::Request))
				{
					state_ = State::RecieveRequest;
					currentHeader_ = header;
					payloadBuffer_.resize(0);
					offset += sizeof(PacketHeader);
				}
				else if (header.IsValid(PacketType::Responce))
				{
					state_ = State::RecieveResponce;
					currentHeader_ = header;
					payloadBuffer_.resize(0);
					offset += sizeof(PacketHeader);
				}
				else
				{
					offset++;
				}
			}
			else if (state_ == State::RecieveRequest)
			{
				size_t size = std::min(dataSize, currentHeader_.payloadSize - payloadBuffer_.size());
				payloadBuffer_.insert(payloadBuffer_.end(), dataPtr, dataPtr + size);
				offset += size;

				if (payloadBuffer_.size() >= currentHeader_.payloadSize)
				{
					std::lock_guard<std::mutex> lock(mutex_);
					queuedRequests_.emplace_back(currentHeader_, std::move(payloadBuffer_));
					payloadBuffer_ = std::vector<uint8_t>();
					state_ = State::SearchPacket;
				}
			}
			else if (state_ == State::RecieveResponce)
			{
				size_t size = std::min(dataSize, currentHeader_.payloadSize - payloadBuffer_.size());
				payloadBuffer_.insert(payloadBuffer_.end(), dataPtr, dataPtr + size);
				offset += size;

				if (payloadBuffer_.size() >= currentHeader_.payloadSize)
				{
					std::lock_guard<std::mutex> lock(mutex_);
					queuedResponses_.emplace_back(currentHeader_, std::move(payloadBuffer_));
					payloadBuffer_ = std::vector<uint8_t>();
					state_ = State::SearchPacket;
				}
			}
		}

		if (offset < packetBufferFilled_)
		{
			memmove(&packetBuffer_[0], &packetBuffer_[offset], packetBufferFilled_ - offset);
		}
		packetBufferFilled_ -= offset;
	}
}

bool Session::SendRequest(uint16_t requestID, const Request& req, ResponseHandler responceHandler)
{
	if (socket_ == nullptr)
	{
		return false;
	}

	uint16_t transactionID = sequenceCount_++;

	if (responceHandler)
	{
		responseHandlers_.emplace(transactionID, std::move(responceHandler));
	}

	PacketHeader reqHeader{};
	reqHeader.Set(PacketType::Request, requestID, transactionID, req.code, (uint32_t)req.payload.size());

	int32_t ret = 0;
	ret = socket_->Send(&reqHeader, sizeof(reqHeader));
	ret = socket_->Send(req.payload.data(), (int32_t)req.payload.size());
	if (ret <= 0)
	{
		Close();
		return false;
	}

	return true;
}

void Session::OnRequested(uint16_t requestID, RequestHandler requestHandler)
{
	requestHandlers_.emplace(requestID, std::move(requestHandler));
}

void Session::PacketHeader::Set(PacketType inType, 
	uint16_t inRequestID, uint16_t inTransactionID, 
	uint32_t inCode, uint32_t inPayloadSize)
{
	signature[0] = 0xAA;
	signature[1] = 0x55;
	type = inType;
	reserved = 0;
	requestID = inRequestID;
	transactionID = inTransactionID;
	code = inCode;
	payloadSize = inPayloadSize;
}

bool Session::PacketHeader::IsValid(PacketType inType) const
{
	return signature[0] == 0xAA
		&& signature[1] == 0x55
		&& reserved == 0
		&& type == inType;
}

}

#endif
