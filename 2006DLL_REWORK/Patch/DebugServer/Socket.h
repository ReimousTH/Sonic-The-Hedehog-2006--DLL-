#pragma once

#ifdef _XBOX
#include <xtl.h>
#else
#pragma comment(lib, "Ws2_32.lib")
#include <winsock.h>		
typedef unsigned long long XUID;
#endif // 

#include <iostream>
#include <vector>
#include <map>

#define min(a, b) ((a) < (b) ? (a) : (b))

#define SOCKET_PORT_TCP 27015
#define SOCKET_PORT_UDP 27016
#define MESSAGE_BUFFER_SIZE 256
#define SOCKET_PLAYER_NAME_SIZE 64



struct SockaddrComparator {
	bool operator()(const sockaddr* lhs, const sockaddr* rhs) const {
		return memcmp(lhs, rhs, sizeof(*lhs)) < 0;
	}
	bool operator()(const sockaddr& lhs, const sockaddr& rhs) const {
		return memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
	}
};




struct CoreMessageData{}; //Just Empty, for explicit


struct SocketMessage {
	
	int ID; //0x0
	short Recieved_PROTOCOL; //0x4  
	short Target_Protocol; //0x6
	XUID sender_xuid; //0x8
	XUID replicated_xuid; //0x10 if replicated != 0
	int replicate; //0x18 //flag-to server
	int replicated; //0x1C , more like bool to client if need
	sockaddr address_from; // FROM SEND //0x20
	sockaddr replicated_from; //CROS SEND //0x30
	char _player_name_[SOCKET_PLAYER_NAME_SIZE];
	char _message_data[MESSAGE_BUFFER_SIZE]; //BUFFER

	template <typename T> T* GetDataAs() {
		return (T*)&_message_data;
	}
	template <typename T> bool EqualID() {
		return T::GetID() == ID ;
	}
	SocketMessage(void* data,size_t size,int ID,int Protocol,int Replicate) {
		memset((char*)this, 0, sizeof(SocketMessage));
		this->ID = ID;
		this->Target_Protocol = Protocol;
		this->replicate = Replicate;
		memcpy(&_message_data, data, min(MESSAGE_BUFFER_SIZE,size));
	}


	SocketMessage() {
		memset((char*)this, 0, sizeof(SocketMessage));
	}
};


#define CUSTOM_STRUCT_MESSAGE(StructName, ID, PROTOCOL,REPLICATE) \
struct StructName { \
    static int GetID() { return ID; } \
    static int GetProtocol() { return PROTOCOL; } \
    static int GetReplicate() { return REPLICATE; } \



//Helper Method
#define SocketMessageFromConst(ptr) SocketMessage((void*)&ptr,sizeof(ptr),ptr.GetID(),ptr.GetProtocol(),ptr.GetReplicate()) 

#define FireEvent(evt) if (evt) evt




CUSTOM_STRUCT_MESSAGE(JoinMessage, -1, IPPROTO_TCP,1)
	XUID CLIENT_ID;
};

CUSTOM_STRUCT_MESSAGE(LeftMessage, -2, IPPROTO_TCP, 1)
XUID CLIENT_ID;
};

CUSTOM_STRUCT_MESSAGE(TestMessage, -3, IPPROTO_UDP, 1)
};





CUSTOM_STRUCT_MESSAGE(JoinMessageUDP, -1, IPPROTO_UDP, 0)
XUID CLIENT_ID;
};







//Implement ERROR CHECK FOR EVERYTHING
class Socket
{
public:
	//Settings(Xbox/Xenia)
	static const char* IP_ADDR; // H
private:
	SOCKET _tcpsock;
	SOCKET _udpsock;
	sockaddr tcp_addr;
	sockaddr udp_addr;
	sockaddr mytcp_addr;
	sockaddr myudp_addr;

	std::map<sockaddr, SOCKET, SockaddrComparator> ObservableClientsTCP;
	std::map<sockaddr, SOCKET, SockaddrComparator> ObservableClientsUDP;
	std::map<XUID, sockaddr> ObservableClientsTCP_XUID;
	std::map<XUID, sockaddr> ObservableClientsUDP_XUID;

public:

	bool server;
	int connection_status;

	void Build(bool server,const char* client_address,const char* server_address );
	void Demolish();

	void StartUP();
	void UpdateClient();
	int Connect();
	void UpdateServer();
private:
	//EVENTS
	void OnClientPreviewAdd(sockaddr addr,SOCKET,int); // 
	void OnServerMessageRecievedPreview(SocketMessage* message, SOCKET sock); //sock-work for tcp only
	void OnClientMessageRecievedPreview(SocketMessage* message,SOCKET sock); //sock-work for tcp only
	void OnServerXuidAdd(XUID xuid,const char* name,sockaddr addr,int PROTOCOL);
	void OnClientXuidAdd(XUID xuid,const char* name,sockaddr addr, int PROTOCOL);
	// CS = Client Sided, SS- Server Sided
	void CSJoinToServer();
	void CSLostConnectionToServer();
	void SSLostConnectionToClient(SOCKET socket,sockaddr addr);
	void OnCSSSXuidRemove(XUID xuid);

public:

	//Replaceable Events :)
	void(*CSSS_XUID_REMOVE)(XUID xuid);
	void(*CSSS_XUID_ADD)(XUID xuid,const char* name);
	void(*CSSS_MESSAGE_RECIEVED)(SocketMessage* message, SOCKET sock);

	//Methods 

	void Send(SOCKET,sockaddr*, SocketMessage*);
	void SendCliservTUD(SocketMessage*);

	//SendToServerdsa=

	void SendToServerTCP(SocketMessage*);
	void SendToServerUDP(SocketMessage*);
	void SendToServerTUD(SocketMessage*);


	//SendToClients
	void SendToClientsTCP(SocketMessage*);
	void SendToClientsUDP(SocketMessage*);
	void SendToClientsTUD(SocketMessage*);


	XUID GetXUID(int index = 0);
	void GetName(char buffer[SOCKET_PLAYER_NAME_SIZE]);





};

