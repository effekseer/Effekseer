#include "../../Effekseer/Effekseer.h"
#include "../../Effekseer/Effekseer/Network/Effekseer.Session.h"
#include "../TestHelper.h"
#include <algorithm>

namespace EffekseerRenderer
{

} // namespace EffekseerRenderer

namespace
{

constexpr uint16_t SessionPortBegin = 40000;
constexpr uint16_t ServerPortBegin = 41000;
constexpr uint16_t PortCandidateCount = 32;

uint16_t ListenOnAvailablePort(Effekseer::Socket& socket, uint16_t portBegin, int32_t backlog)
{
	for (uint16_t offset = 0; offset < PortCandidateCount; offset++)
	{
		const auto port = static_cast<uint16_t>(portBegin + offset);
		if (socket.Listen(port, backlog))
		{
			return port;
		}
	}

	return 0;
}

uint16_t StartServerOnAvailablePort(const Effekseer::ServerRef& server, uint16_t portBegin)
{
	for (uint16_t offset = 0; offset < PortCandidateCount; offset++)
	{
		const auto port = static_cast<uint16_t>(portBegin + offset);
		if (server->Start(port))
		{
			return port;
		}
	}

	return 0;
}

bool WaitUntilServerConnected(const Effekseer::ServerRef& server, std::chrono::milliseconds timeout)
{
	const auto deadline = std::chrono::steady_clock::now() + timeout;
	while (std::chrono::steady_clock::now() < deadline)
	{
		if (server->IsConnected())
		{
			return true;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return server->IsConnected();
}

} // namespace

void NetworkTest_Session()
{
	using namespace Effekseer;

	Socket listenSocket;
	const auto port = ListenOnAvailablePort(listenSocket, SessionPortBegin, 1);
	EXPECT_TRUE(port != 0);

	Socket clientSocket;
	EXPECT_TRUE(clientSocket.Connect("127.0.0.1", port));

	Socket serverSocket = listenSocket.Accept();
	EXPECT_TRUE(serverSocket.IsValid());

	Session clientSession, serverSession;

	clientSession.Open(&clientSocket);
	EXPECT_TRUE(clientSession.IsActive());

	serverSession.Open(&serverSocket);
	EXPECT_TRUE(serverSession.IsActive());

	bool received = false;
	serverSession.OnReceived(10, [&](const Session::Message& msg)
							 {
		received = true;
		EXPECT_TRUE(msg.payload.size == 5);
		EXPECT_TRUE(std::equal(msg.payload.data, msg.payload.data + msg.payload.size, std::vector<uint8_t>{1, 2, 3, 4, 5}.begin())); });
	EXPECT_TRUE(clientSession.Send(10, std::vector<uint8_t>{1, 2, 3, 4, 5}));

	bool requested = false, respond = false;
	serverSession.OnRequested(10, [&](const Session::Request& req)
							  {
		requested = true;
		EXPECT_TRUE(req.payload.size == 5);
		EXPECT_TRUE(std::equal(req.payload.data, req.payload.data + req.payload.size, std::vector<uint8_t>{1, 2, 3, 4, 5}.begin()));
		serverSession.SendResponse(req.responseID, 300, std::vector<uint8_t>{6, 7, 8, 9, 10}); });
	EXPECT_TRUE(clientSession.SendRequest(10, std::vector<uint8_t>{1, 2, 3, 4, 5}, [&](const Session::Response& res)
									 {
		respond = true;
		EXPECT_TRUE(res.code == 300);
		EXPECT_TRUE(res.payload.size == 5);
		EXPECT_TRUE(std::equal(res.payload.data, res.payload.data + res.payload.size, std::vector<uint8_t>{6, 7, 8, 9, 10}.begin())); }));

	for (int i = 0; i < 10; i++)
	{
		serverSession.Update();
		clientSession.Update();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	EXPECT_TRUE(received);
	EXPECT_TRUE(requested);
	EXPECT_TRUE(respond);
}

void NetworkTest_ClientAndServer()
{
	using namespace Effekseer;

	const auto path = GetDirectoryPathAsU16(__FILE__) + u"../../../../TestData/Effects/Update_17x/Sprite.efkefc";

	auto server = Server::Create();
	auto client = Client::Create();

	const auto port = StartServerOnAvailablePort(server, ServerPortBegin);
	EXPECT_TRUE(port != 0);
	EXPECT_TRUE(client->Start("127.0.0.1", port));

	EXPECT_TRUE(client->IsConnected());
	EXPECT_TRUE(WaitUntilServerConnected(server, std::chrono::seconds(2)));

	auto manager = Manager::Create(100);
	auto effect = Effect::Create(manager, path.c_str());
	EXPECT_TRUE(effect != nullptr);

	server->Register(u"Sprite", effect);
	client->Reload(manager, path.c_str(), u"Sprite");

	for (int i = 0; i < 10; i++)
	{
		server->Update(&manager, 1);
		client->Update();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	server->Stop();
	client->Stop();
}

TestRegister Runtime_NetworkSessionTest("Runtime.Network.Session", []() -> void
									{ NetworkTest_Session(); });
TestRegister Runtime_NetworkClientAndServerTest("Runtime.Network.ClientAndServer", []() -> void
										 { NetworkTest_ClientAndServer(); });
