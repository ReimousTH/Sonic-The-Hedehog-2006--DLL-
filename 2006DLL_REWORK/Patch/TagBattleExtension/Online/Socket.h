#pragma once


#include <iostream>
#include <map>
#include <vector>
#ifdef _XBOX
#include <stdexcept>
#include <xtl.h>
#include <xmath.h>

#else
#pragma comment(lib, "Ws2_32.lib")
#include <winsock.h>
typedef unsigned long long XUID;
#endif // _XBOX




#define DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(ID,PROTOCOL) \
	static int GetID(){ \
	return ID;\
}\
	static int GetProtocol(){\
	return PROTOCOL; \
}\
	static int GetReplicate(){\
	return false;\
}\

#define DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL_REPLICATE(ID,PROTOCOL,REPLICATE) \
	static int GetID(){ \
	return ID;\
}\
	static int GetProtocol(){\
	return PROTOCOL; \
}\
	static int GetReplicate(){\
	return true;\
}\




#define DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(DATA) SocketMessage(DATA.GetID(),DATA.GetProtocol(),DATA.GetReplicate(),(void*)&DATA,sizeof(DATA))
#define EXTRACT_SOCKET_MESSAGE_FROM_MESSAGE_PTR(DATA,TYPE) (TYPE*)&DATA->_message_;
#define SM_REPLICATE 1

struct SocketMessage {
	int ID; //0x0
	int PROTOCOL; //0x4
	int replicate; //0x8
	int replicate_null; //0xC 
	XUID sender_xuid; // sender_xuid (0x10)
	XUID replicated_xuid; // replicated xuid, save  client(XUID)-server-client(XUID) (0x18)
	sockaddr address_from; //0x20
	char _message_[512]; //0x30
public:
	SocketMessage(int ID, int PROTOCOL, int REPLICATE, void* from, int size);
	SocketMessage();
};

struct SocketData {
	SOCKET TCP_SOCKET;
	XUID xuid;
	char name[64];
	float UDP_TIMEOUT;
public:
	SocketData(int TCP_SOCKET);
	SocketData();
};

struct SMDataTest {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(0x1, IPPROTO_TCP);
};
struct SMDataTest_UDP {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(0x1, IPPROTO_UDP);
};

#define SOCKET_MESSAGE_NATIVE_MESSAGES_SHIFT 24
#define SOCKET_MESSAGE_ID_JOIN_XUID 0x1 << SOCKET_MESSAGE_NATIVE_MESSAGES_SHIFT
#define SOCKET_MESSAGE_ID_LEFT_XUID 0x2 << SOCKET_MESSAGE_NATIVE_MESSAGES_SHIFT

struct SMDataJoinXUID {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(SOCKET_MESSAGE_ID_JOIN_XUID, IPPROTO_TCP);
	sockaddr address;
	XUID sender_xuid;
	char sender_name[64];
};

struct SMDataLeftXUID {
	DEFINE_SOCKET_MESSAGE_DATA_ID_PROTOCOL(SOCKET_MESSAGE_ID_LEFT_XUID, IPPROTO_TCP);
	XUID sender_xuid;
	sockaddr address;
};

struct SockaddrComparator {
	bool operator()(const sockaddr* lhs, const sockaddr* rhs) const {
		return memcmp(lhs, rhs, sizeof(*lhs)) < 0;
	}
	bool operator()(const sockaddr& lhs, const sockaddr& rhs) const {
		return memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
	}
};







class Socket {
public:
	static const char* IP_ADDR;

	Socket();               // Constructor
	~Socket();              // Destructor

	void InitSockets();     // Initialize TCP and UDP sockets
	void SetBind(); // Bind sockets to the specified IP
	void SetAddress(const char* address, short port);
	void SetNonBlockingMode(); // Bind sockets to the specified IP
	void Cleanup();         // Clean up resources
	bool IsWorks();
	bool IsClient();
	bool IsServer();
	int GetConnectStatus();

	sockaddr GetAddress();
	sockaddr GetAddressTo();


	void InitServer();
	int InitClient();

	void UpdateServer(float delta); // Handle incoming connections and data
	void UpdateClient(float delta);

	void SendTCPMessageTo(SOCKET to, SocketMessage* msg);
	void SendTCPMessageToSRCL(SocketMessage* msg);
	void SendTCPMessageToServer(SocketMessage* msg);
	void SendTCPMessageToClients(SocketMessage* msg);
	void SendTCPMessageToXUID(XUID to,SocketMessage* msg);


	void SendUDPMessageTo(sockaddr to, SocketMessage* msg);
	void SendUDPMessageToSRCL(SocketMessage* msg);
	void SendUDPMessageToServer(SocketMessage* msg);
	void SendUDPMessageToClients(SocketMessage* msg);

	sockaddr MatchClientTcpToUdpAddress(sockaddr tcp_address);
	XUID MatchClientXUIDByTCPSocket(SOCKET tcp_socket);
	SOCKET MatchClientXUIDToTCPSocket(XUID xuid);


	XUID GetXUID(int);
	void GetName(char* buffer);
	std::string XUIDToName(XUID xuid);

private:
	SOCKET _tcpSocket;      // TCP socket
	SOCKET _udpSocket;      // UDP socket
	std::map<sockaddr, SocketData, SockaddrComparator> _clients;
	std::map<sockaddr, bool, SockaddrComparator> _udp_clients_map;

	sockaddr _address_to;
	// -2 (Not Active)
	// -1 (Active >> ERROR)
	// 0 (Almost Done )
	// 0  

	int _connection_status;
	bool _client;
	bool _server;

	typedef void (SocketMSGCommon)(Socket*, SOCKET, SocketMessage*);
	typedef void (SocketCommonInfo)(Socket*, SOCKET);
	typedef void (SocketCommonInfoEx)(Socket*, SOCKET, XUID);

public:
	SocketMSGCommon* MSG_HANDLE_SERVER_MESSAGES;
	SocketCommonInfo* MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION;
	SocketCommonInfo* MSG_HANDLE_SERVER_CLIENT_JOIN;


	SocketMSGCommon* MSG_HANDLE_CLIENT_MESSAGES;
	SocketCommonInfo* MSG_HANDLE_CLIENT_JOIN_SERVER;
	SocketCommonInfo* MSG_HANDLE_CLIENT_LOST_CONNECTION_SERVER;
	SocketCommonInfoEx* MSG_HANDLE_SERVER_XUI_JOIN;
	SocketCommonInfoEx* MSG_HANDLE_SERVER_XUI_LEFT;
	SocketCommonInfoEx* MSG_HANDLE_CLIENT_XUI_JOIN;
	SocketCommonInfoEx* MSG_HANDLE_CLIENT_XUI_LEFT;

	void MSG_HANDLE_SERVER_MESSAGES_BEHAVIOUR(Socket*, SOCKET, SocketMessage*);
	static void MSG_HANDLE_SERVER_MESSAGES_TEMP(Socket*, SOCKET, SocketMessage*);

	void MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION_BEHAVIOUR(Socket*, SOCKET);
	static void MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION_TEMP(Socket*, SOCKET);

	void MSG_HANDLE_SERVER_CLIENT_JOIN_BEHAVIOUR(Socket*, SOCKET);
	static void MSG_HANDLE_SERVER_CLIENT_JOIN_TEMP(Socket*, SOCKET);

	void MSG_HANDLE_CLIENT_MESSAGES_BEHAVIOUR(Socket*, SOCKET, SocketMessage*);
	static void MSG_HANDLE_CLIENT_MESSAGES_TEMP(Socket*, SOCKET, SocketMessage*);

	void MSG_HANDLE_CLIENT_JOIN_SERVER_BEHAVIOUR(Socket*, SOCKET);
	static void MSG_HANDLE_CLIENT_JOIN_SERVER_TEMP(Socket*, SOCKET);
	static void MSG_HANDLE_CLIENT_LOST_CONNECTION_SERVER_TEMP(Socket*, SOCKET);

	void MSG_HANDLE_SERVER_XUI_JOIN_BEHAVIOUR(Socket*, SOCKET, XUID);
	static void MSG_HANDLE_SERVER_XUI_JOIN_TEMP(Socket*, SOCKET, XUID);

	void MSG_HANDLE_SERVER_XUI_LEFT_BEHAVIOUR(Socket*, SOCKET, XUID);
	static void MSG_HANDLE_SERVER_XUI_LEFT_TEMP(Socket*, SOCKET, XUID);

	void MSG_HANDLE_CLIENT_XUI_JOIN_BEHAVIOUR(Socket*, SOCKET, XUID);
	static void MSG_HANDLE_CLIENT_XUI_JOIN_TEMP(Socket*, SOCKET, XUID);

	void MSG_HANDLE_CLIENT_XUI_LEFT_BEHAVIOUR(Socket*, SOCKET, XUID);
	static void MSG_HANDLE_CLIENT_XUI_LEFT_TEMP(Socket*, SOCKET, XUID);





};

