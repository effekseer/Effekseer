#if (defined(__EFFEKSEER_NETWORK_ENABLED__))

#include <string.h>
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

	for (auto& data : queuedPackets_)
	{
		const PacketHeader& header = std::get<0>(data);
		const Payload& payload = std::get<1>(data);

		if (header.type == PacketType::Message)
		{
			auto it = messageHandlers_.find(header.targetID);
			if (it != messageHandlers_.end())
			{
				it->second(Message{ ByteData(payload) });
			}
		}
		else if (header.type == PacketType::Request)
		{
			auto it = requestHandlers_.find(header.targetID);
			if (it != requestHandlers_.end())
			{
				it->second(Request{ header.sourceID, ByteData(payload) });
			}
		}
		else if (header.type == PacketType::Response)
		{
			auto it = responseHandlers_.find(header.targetID);
			if (it != responseHandlers_.end())
			{
				it->second(Response{ header.code, ByteData(payload) });
				responseHandlers_.erase(it);
			}
		}
	}
	queuedPackets_.clear();
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
		while (offset < packetBufferFilled_ ||
			(state_ == State::ReceivePayload && currentHeader_.payloadSize == 0))
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

				if (header.IsValid())
				{
					state_ = State::ReceivePayload;
					currentHeader_ = header;
					payloadBuffer_.resize(0);
					offset += sizeof(PacketHeader);
				}
				else
				{
					offset++;
				}
			}
			else if (state_ == State::ReceivePayload)
			{
				size_t size = std::min(dataSize, currentHeader_.payloadSize - payloadBuffer_.size());
				payloadBuffer_.insert(payloadBuffer_.end(), dataPtr, dataPtr + size);
				offset += size;

				if (payloadBuffer_.size() >= currentHeader_.payloadSize)
				{
					std::lock_guard<std::mutex> lock(mutex_);
					queuedPackets_.emplace_back(currentHeader_, std::move(payloadBuffer_));
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

bool Session::Send(uint16_t messageID, const ByteData& payload)
{
	if (socket_ == nullptr)
	{
		return false;
	}

	uint16_t packetID = sequenceCount_++;

	PacketHeader msgHeader{};
	msgHeader.Set(PacketType::Message, messageID, packetID, 0, (uint32_t)payload.size);

	int32_t ret = 0;
	ret = socket_->Send(&msgHeader, sizeof(msgHeader));
	if (ret <= 0)
	{
		Close();
		return false;
	}

	if (payload.size > 0)
	{
		ret = socket_->Send(payload.data, (int32_t)payload.size);
		if (ret <= 0)
		{
			Close();
			return false;
		}
	}

	return true;
}

void Session::OnReceived(uint16_t messageID, MessageHandler messageHandler)
{
	if (messageHandler != nullptr)
	{
		messageHandlers_.emplace(messageID, std::move(messageHandler));
	}
	else
	{
		messageHandlers_.erase(messageID);
	}
}

bool Session::SendRequest(uint16_t requestID, const ByteData& payload, ResponseHandler responseHandler)
{
	if (socket_ == nullptr)
	{
		return false;
	}

	uint16_t packetID = sequenceCount_++;

	if (responseHandler)
	{
		responseHandlers_.emplace(packetID, std::move(responseHandler));
	}

	PacketHeader reqHeader{};
	reqHeader.Set(PacketType::Request, requestID, packetID, 0, (uint32_t)payload.size);

	int32_t ret = 0;
	ret = socket_->Send(&reqHeader, sizeof(reqHeader));
	if (ret <= 0)
	{
		Close();
		return false;
	}

	if (payload.size > 0)
	{
		ret = socket_->Send(payload.data, (int32_t)payload.size);
		if (ret <= 0)
		{
			Close();
			return false;
		}
	}

	return true;
}

bool Session::SendResponse(uint16_t responseID, int32_t code, const ByteData& payload)
{
	if (socket_ == nullptr)
	{
		return false;
	}

	uint16_t packetID = sequenceCount_++;

	PacketHeader resHeader{};
	resHeader.Set(PacketType::Response, responseID, packetID, code, (uint32_t)payload.size);

	int32_t ret = 0;
	ret = socket_->Send(&resHeader, sizeof(resHeader));
	if (ret <= 0)
	{
		Close();
		return false;
	}

	if (payload.size > 0)
	{
		ret = socket_->Send(payload.data, (int32_t)payload.size);
		if (ret <= 0)
		{
			Close();
			return false;
		}
	}

	return true;
}

void Session::OnRequested(uint16_t requestID, RequestHandler requestHandler)
{
	if (requestHandler != nullptr)
	{
		requestHandlers_.emplace(requestID, std::move(requestHandler));
	}
	else
	{
		requestHandlers_.erase(requestID);
	}
}

void Session::PacketHeader::Set(PacketType inType, 
	uint16_t inTargetID, uint16_t inSourceID, 
	int32_t inCode, uint32_t inPayloadSize)
{
	signature[0] = 0xAA;
	signature[1] = 0x55;
	type = inType;
	reserved = 0;
	targetID = inTargetID;
	sourceID = inSourceID;
	code = inCode;
	payloadSize = inPayloadSize;
}

bool Session::PacketHeader::IsValid() const
{
	return signature[0] == 0xAA
		&& signature[1] == 0x55
		&& (type == PacketType::Message || type == PacketType::Request || type == PacketType::Response)
		&& reserved == 0;
}

}

#endif
