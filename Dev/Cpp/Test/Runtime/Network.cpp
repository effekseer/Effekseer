#include <algorithm>
#include "../../Effekseer/Effekseer.h"
#include "../../Effekseer/Effekseer/Network/Effekseer.Session.h"
#include "../TestHelper.h"

namespace EffekseerRenderer
{

} // namespace EffekseerRenderer

void NetworkTest_Session()
{
	using namespace Effekseer;

	const uint16_t PORT = 40000;

	Socket listenSocket;
	listenSocket.Listen(PORT, 1);
	EXPECT_TRUE(listenSocket.IsValid());
	
	Socket clientSocket;
	clientSocket.Connect("127.0.0.1", PORT);
	EXPECT_TRUE(clientSocket.IsValid());

	Socket serverSocket = listenSocket.Accept();
	EXPECT_TRUE(serverSocket.IsValid());

	Session clientSession, serverSession;

	clientSession.Open(&clientSocket);
	EXPECT_TRUE(clientSession.IsActive());

	serverSession.Open(&serverSocket);
	EXPECT_TRUE(serverSession.IsActive());

	bool received = false;
	serverSession.OnReceived(10, [&](const Session::Message& msg){
		received = true;
		EXPECT_TRUE(msg.payload.size == 5);
		EXPECT_TRUE(std::equal(msg.payload.data, msg.payload.data + msg.payload.size, std::vector<uint8_t>{1, 2, 3, 4, 5}.begin()));
	});
	clientSession.Send(10, std::vector<uint8_t>{1, 2, 3, 4, 5});

	bool requested = false, respond = false;
	serverSession.OnRequested(10, [&](const Session::Request& req){
		requested = true;
		EXPECT_TRUE(req.payload.size == 5);
		EXPECT_TRUE(std::equal(req.payload.data, req.payload.data + req.payload.size, std::vector<uint8_t>{1, 2, 3, 4, 5}.begin()));
		serverSession.SendResponse(req.responseID, 300, std::vector<uint8_t>{6, 7, 8, 9, 10});
	});
	clientSession.SendRequest(10, std::vector<uint8_t>{1, 2, 3, 4, 5}, [&](const Session::Response& res){
		respond = true;
		EXPECT_TRUE(res.code == 300);
		EXPECT_TRUE(res.payload.size == 5);
		EXPECT_TRUE(std::equal(res.payload.data, res.payload.data + res.payload.size, std::vector<uint8_t>{6, 7, 8, 9, 10}.begin()));
	});

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

	const uint16_t PORT = 60000;
	const char16_t* PATH = u"../../../../TestData/Effects/Update_17x/Sprite.efkefc";

	auto server = Server::Create();
	auto client = Client::Create();

	server->Start(PORT);
	client->Start("127.0.0.1", PORT);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_TRUE(client->IsConnected());
	EXPECT_TRUE(server->IsConnected());

	auto manager = Manager::Create(100);
	auto effect = Effect::Create(manager, PATH);

	server->Register(u"Sprite", effect);
	client->Reload(manager, PATH, u"Sprite");

	for (int i = 0; i < 10; i++)
	{
		server->Update(&manager, 1);
		client->Update();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	server->Stop();
	client->Stop();
}

void NetworkTest()
{
	NetworkTest_Session();
	NetworkTest_ClientAndServer();
}
TestRegister Runtime_NetworkTest("Runtime.Network", []() -> void { NetworkTest(); });
