#ifndef __EFFEKSEER_SESSION_H__
#define __EFFEKSEER_SESSION_H__

#if !(defined(__EFFEKSEER_NETWORK_DISABLED__))

#include <stdint.h>
#include <functional>
#include <thread>
#include <mutex>
#include "Effekseer.Socket.h"

namespace Effekseer
{

class Session
{
public:
	struct Request
	{
		uint32_t code = 0;
		std::vector<uint8_t> payload;
	};

	struct Response
	{
		uint32_t code = 0;
		std::vector<uint8_t> payload;
	};

	using Payload = std::vector<uint8_t>;
	using RequestHandler = std::function<void(const Request& req, Response& res)>;
	using ResponseHandler = std::function<void(const Response& res)>;

public:
	Session();
	~Session();

	void Open(Socket* socket);

	void Close();

	void Update();

	bool SendRequest(uint16_t requestID, const Request& req, ResponseHandler responceHandler);

	void OnRequested(uint16_t requestID, RequestHandler requestHandler);

	bool IsActive() const { return socket_ != nullptr; }

private:
	void RecvThread();

	Socket* socket_ = nullptr;
	uint16_t sequenceCount_ = 0;

	std::thread thread_;
	std::mutex mutex_;

	std::unordered_map<uint16_t, ResponseHandler> responseHandlers_;
	std::unordered_map<uint16_t, RequestHandler> requestHandlers_;

	enum class PacketType : uint8_t
	{
		Request, Responce,
	};
	struct PacketHeader
	{
		uint8_t signature[2];
		PacketType type;
		uint8_t reserved;
		uint16_t requestID;
		uint16_t transactionID;
		uint32_t code;
		uint32_t payloadSize;

		void Set(PacketType inType, 
			uint16_t inRequestID, uint16_t inTransactionID, 
			uint32_t inCode, uint32_t inPayloadSize);
		bool IsValid(PacketType inType) const;
	};

	enum class State
	{
		SearchPacket, RecieveRequest, RecieveResponce,
	};
	State state_ = State::SearchPacket;
	uint8_t packetBuffer_[4096];
	size_t packetBufferFilled_ = 0;
	Payload payloadBuffer_;
	PacketHeader currentHeader_;

	std::vector<std::tuple<PacketHeader, Payload>> queuedRequests_;
	std::vector<std::tuple<PacketHeader, Payload>> queuedResponses_;
};

} // namespace Effekseer

#endif

#endif // __EFFEKSEER_SERVER_H__
