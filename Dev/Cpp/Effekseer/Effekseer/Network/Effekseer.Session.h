#ifndef __EFFEKSEER_SESSION_H__
#define __EFFEKSEER_SESSION_H__

#if !(defined(__EFFEKSEER_NETWORK_DISABLED__))

#include <stdint.h>
#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include "Effekseer.Socket.h"

namespace Effekseer
{

class Session
{
public:
	struct ByteData
	{
		const uint8_t* data = nullptr;
		size_t size = 0;

		ByteData(const uint8_t* data, size_t size)
			: data(data), size(size) {}
		ByteData(const std::vector<uint8_t>& vec)
			: data(vec.data()), size(vec.size()) {}
	};

	struct Message
	{
		ByteData payload;
	};
	struct Request
	{
		uint16_t responseID;
		ByteData payload;
	};
	struct Response
	{
		int32_t code;
		ByteData payload;
	};

	using Payload = std::vector<uint8_t>;
	using MessageHandler = std::function<void(const Message& message)>;
	using RequestHandler = std::function<void(const Request& request)>;
	using ResponseHandler = std::function<void(const Response& response)>;

public:
	Session();
	~Session();

	void Open(Socket* socket);

	void Close();

	void Update();

	bool Send(uint16_t messageID, const ByteData& payload);

	void OnReceived(uint16_t messageID, MessageHandler messageHandler);

	bool SendRequest(uint16_t requestID, const ByteData& payload, ResponseHandler responseHandler);

	bool SendResponse(uint16_t responseID, int32_t code, const ByteData& payload);

	void OnRequested(uint16_t requestID, RequestHandler requestHandler);

	bool IsActive() const { return socket_ != nullptr; }

private:
	void RecvThread();

	Socket* socket_ = nullptr;
	uint16_t sequenceCount_ = 0;

	std::thread thread_;
	std::mutex mutex_;

	std::unordered_map<uint16_t, MessageHandler> messageHandlers_;
	std::unordered_map<uint16_t, ResponseHandler> responseHandlers_;
	std::unordered_map<uint16_t, RequestHandler> requestHandlers_;

	enum class PacketType : uint8_t
	{
		Message, Request, Response,
	};
	struct PacketHeader
	{
		uint8_t signature[2];
		PacketType type;
		uint8_t reserved;
		uint16_t targetID;
		uint16_t sourceID;
		int32_t code;
		uint32_t payloadSize;

		void Set(PacketType inType, 
			uint16_t inTargetID, uint16_t inSourceID, 
			int32_t inCode, uint32_t inPayloadSize);
		bool IsValid() const;
	};

	enum class State
	{
		SearchPacket, ReceivePayload,
	};
	State state_ = State::SearchPacket;
	uint8_t packetBuffer_[4096];
	size_t packetBufferFilled_ = 0;
	Payload payloadBuffer_;
	PacketHeader currentHeader_;

	std::vector<std::tuple<PacketHeader, Payload>> queuedPackets_;
};

} // namespace Effekseer

#endif

#endif // __EFFEKSEER_SERVER_H__
