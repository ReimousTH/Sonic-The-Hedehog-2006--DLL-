#include "Socket.h"


void Socket::Build(bool server,const char* client_address = "127.0.0.1", const char* server_address = "127.0.0.1")
{
	this->server = server;
	this->connection_status = -1;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
		return;
	}
	else
	{
		std::cout << "WSAStartup success:  " << WSAGetLastError() << std::endl;

	}


	_tcpsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	_udpsock = socket(AF_INET, SOCK_DGRAM,  IPPROTO_UDP);

	if ((_tcpsock | _udpsock) == INVALID_SOCKET) {
		std::cerr << "_tcpsock & _udpsock error: " << WSAGetLastError() << std::endl;
	}
	else
	{
		std::cerr << "_tcpsock & _udpsock success: " << WSAGetLastError() << std::endl;
	}


	//htons
	sockaddr_in tcp_addr;
	sockaddr_in mytcp_addr;
	tcp_addr.sin_family = AF_INET; tcp_addr.sin_port = SOCKET_PORT_TCP; tcp_addr.sin_addr.s_addr = inet_addr(server_address);
	mytcp_addr.sin_family = AF_INET; mytcp_addr.sin_port = SOCKET_PORT_TCP; mytcp_addr.sin_addr.s_addr = inet_addr(client_address);

	

	sockaddr_in udp_addr;
	sockaddr_in myudp_addr;
	udp_addr.sin_family = AF_INET; udp_addr.sin_port = SOCKET_PORT_UDP; udp_addr.sin_addr.s_addr = inet_addr(server_address);
	myudp_addr.sin_family = AF_INET; myudp_addr.sin_port = SOCKET_PORT_UDP; myudp_addr.sin_addr.s_addr = inet_addr(client_address);

	*(&this->tcp_addr) = *(struct sockaddr*)&tcp_addr;
	*(&this->udp_addr) = *(struct sockaddr*)&udp_addr;
	*(&this->mytcp_addr) = *(struct sockaddr*)&mytcp_addr;
	*(&this->myudp_addr) = *(struct sockaddr*)&myudp_addr;

	int ERROR_BIND_TCP = 0;
	int ERROR_BIND_UDP = 0;
	
	if (server) {
		ERROR_BIND_TCP = bind(_tcpsock, &this->tcp_addr, sizeof(sockaddr));
		ERROR_BIND_UDP = bind(_udpsock, &this->udp_addr, sizeof(sockaddr));
	}
	else
	{
		ERROR_BIND_TCP = bind(_tcpsock, &this->mytcp_addr, sizeof(sockaddr));
		ERROR_BIND_UDP = bind(_udpsock, &this->myudp_addr, sizeof(sockaddr));
	}
	if ((ERROR_BIND_UDP | ERROR_BIND_TCP) == SOCKET_ERROR) {
		std::cout << "TCP&UDP BIND ERROR" << std::endl;
	}
	else
	{
		std::cout << "TCP&UDP BIND SUCESS" << std::endl;
	}

	//non-blocking mode (to not block anything)
	u_long mode = 1;
	int ERROR_IOCL_SOCKET_TCP = ioctlsocket(_tcpsock, FIONBIO, &mode);
	int ERROR_IOCL_SOCKET_UDP = ioctlsocket(_udpsock, FIONBIO, &mode);


}

void Socket::Demolish()
{
	connection_status = -1;
	std::vector<XUID> ObservableClientsTCP_XUID_remove;
	for (std::map<sockaddr, SOCKET,SockaddrComparator>::iterator client = ObservableClientsTCP.begin(); client != ObservableClientsTCP.end(); client++) {
		closesocket(client->second);
	}
	for (std::map<XUID, sockaddr>::iterator client = ObservableClientsTCP_XUID.begin(); client != ObservableClientsTCP_XUID.end(); client++) {
		XUID client_xuid = client->first;
		ObservableClientsTCP_XUID_remove.push_back(client_xuid);
	}
	for (std::vector<XUID>::iterator it = ObservableClientsTCP_XUID_remove.begin(); it != ObservableClientsTCP_XUID_remove.end(); it++) {
		OnCSSSXuidRemove(*it);
	}
	ObservableClientsTCP.clear();
	ObservableClientsUDP.clear();


	closesocket(_tcpsock);
	closesocket(_udpsock);
	WSACleanup();
}


void Socket::StartUP()
{
	if (server) {
		listen(_tcpsock, 0);
		listen(_udpsock, 0);
	}
	else
	{
		
	}

}

void Socket::UpdateClient()
{
	fd_set tcp_set; FD_ZERO(&tcp_set);
	fd_set udp_set; FD_ZERO(&udp_set);
	FD_SET(_tcpsock, &tcp_set);
	FD_SET(_udpsock, &udp_set);

	//non-block can be ignore so i do it
	struct timeval timeout; memset(&timeout, 0, sizeof(timeval));
	int activity_tcp = select(0, &tcp_set, NULL, NULL, &timeout);
	int activity_udp = select(0, &udp_set, NULL, NULL, &timeout);
	if ((activity_tcp | activity_udp) < 0) return; //no need to update

	//TCP-Find New Clients(repeat somehow)
	while (FD_ISSET(_tcpsock, &tcp_set))
	{
		//Recheck
		FD_ZERO(&tcp_set); FD_SET(_tcpsock, &tcp_set);
		int activity_tcp = select(0, &tcp_set, NULL, NULL, &timeout);
		if (activity_tcp < 0) break;

		SocketMessage buffer;
		sockaddr senderAddr;
		int addrLen = sizeof(senderAddr);
		int recvdata = recvfrom(_tcpsock, (char*)&buffer, sizeof(SocketMessage), 0, (struct sockaddr*)&senderAddr, &addrLen);
		buffer.Recieved_PROTOCOL = IPPROTO_TCP;
		if (recvdata > 0) {
			OnClientMessageRecievedPreview(&buffer, recvdata);
		}
		else if (recvdata == WSAEWOULDBLOCK) {
			std::cout << "_tcpsock " << WSAEWOULDBLOCK;
		}
		else if (recvdata == INVALID_SOCKET) {
			int error = WSAGetLastError();
			switch (error)
			{
			case WSAECONNRESET:
				CSLostConnectionToServer();
				break;
			case WSAEWOULDBLOCK:
				break;
			default:
				std::cout << "UpdateClient : error :  " << error << std::endl;
				break;
			}
		}

	}


	//TCP-Find New Clients(repeat somehow)
	while (FD_ISSET(_udpsock, &udp_set))
	{
		//Recheck
		FD_ZERO(&udp_set); FD_SET(_udpsock, &udp_set);
		int activity_tcp = select(0, &udp_set, NULL, NULL, &timeout);
		if (activity_tcp < 0) break;

		SocketMessage buffer;
		sockaddr senderAddr;
		int addrLen = sizeof(senderAddr);
		int recvdata = recvfrom(_tcpsock, (char*)&buffer, sizeof(SocketMessage), 0, (struct sockaddr*)&senderAddr, &addrLen);
		buffer.Recieved_PROTOCOL = IPPROTO_UDP;
		if (recvdata > 0) {
			OnClientMessageRecievedPreview(&buffer, recvdata);
		}
		else if (recvdata == WSAEWOULDBLOCK) {
			//std::cout << "_tcpsock " << WSAEWOULDBLOCK;
		}
	}

}

int Socket::Connect()
{
	int optlen;
	int error;
	int result_tcp = connect(_tcpsock, &tcp_addr, sizeof(sockaddr));
//	int result_udp = connect(_udpsock, &udp_addr, sizeof(sockaddr));
	if (result_tcp == SOCKET_ERROR) {
		int ERROR_CODE_TCP = WSAGetLastError();
		switch (ERROR_CODE_TCP)
		{
		case WSAEINPROGRESS:
		case WSAEWOULDBLOCK:
			return -1;
			break;
		case WSAEISCONN:

			optlen = sizeof(error);
			if (getsockopt(_tcpsock, SOL_SOCKET, 0x1007, (char*)&error, &optlen) == SOCKET_ERROR) {
				return -1;
			}
			if (error == 0) {
				CSJoinToServer();
				return 1; 
			}

			return 2;
		default:
			return -1;
			break;
		}

	}
	CSJoinToServer();
	return 1;


}

void Socket::UpdateServer()
{
	
	fd_set tcp_set; FD_ZERO(&tcp_set);
	fd_set udp_set; FD_ZERO(&udp_set);
	FD_SET(_tcpsock, &tcp_set);
	FD_SET(_udpsock, &udp_set);

	//non-block can be ignore so i do it
	struct timeval timeout; memset(&timeout, 0, sizeof(timeval));
	int activity_tcp = select(0, &tcp_set, NULL, NULL, &timeout);
	int activity_udp = select(0, &udp_set, NULL, NULL, &timeout);
	if ((activity_tcp | activity_udp) < 0) return; //no need to update

	//TCP-Find New Clients(repeat somehow)
	if (FD_ISSET(_tcpsock, &tcp_set)) {
		SocketMessage buffer; sockaddr senderAddr;
		int addrLen = sizeof(senderAddr);
		SOCKET new_socket = accept(_tcpsock, &senderAddr, &addrLen);
		if (new_socket != INVALID_SOCKET) {
			if (ObservableClientsTCP.find(senderAddr) == ObservableClientsTCP.end()) {
				OnClientPreviewAdd(senderAddr,new_socket,IPPROTO_TCP);
			}
		}
	}

	std::vector<sockaddr> ObservableClientsTCP_Remove;
	

	for (std::map<sockaddr, SOCKET, SockaddrComparator>::iterator it = ObservableClientsTCP.begin(); it != ObservableClientsTCP.end();it++) {

		SOCKET client_socket = it->second;
		fd_set client_tcp_set;FD_ZERO(&client_tcp_set); FD_SET(client_socket, &client_tcp_set);
		select(0, &client_tcp_set, NULL, NULL, &timeout);

		while (FD_ISSET(client_socket, &client_tcp_set))
		{

			FD_ZERO(&client_tcp_set); FD_SET(client_socket, &client_tcp_set);
			int activity_tcp = select(0, &client_tcp_set, NULL, NULL, &timeout);
			if (activity_tcp < 0) break;

			SocketMessage buffer = SocketMessage();
			sockaddr senderAddr;
			int addrLen = sizeof(senderAddr); //for TCP always same
			int recvdata = recvfrom(client_socket, (char*)&buffer, sizeof(SocketMessage), 0, &senderAddr, &addrLen);
			if (recvdata > 0) {
				buffer.address_from = it->first;
				buffer.replicated_from = it->first;
				buffer.Recieved_PROTOCOL = IPPROTO_TCP;

				OnServerMessageRecievedPreview(&buffer, client_socket);
			}
			else if (recvdata == WSAEWOULDBLOCK) {
				//std::cout << "_tcpsock " << WSAEWOULDBLOCK << std::endl;;
			}
			else if (recvdata == WSAECONNRESET)
			{
				std::cout << "_tcpsock " << "WSAECONNRESET" << std::endl;
			}
			else
			{
				int error = WSAGetLastError();
				if (error == WSAECONNRESET || error == 0) {
					ObservableClientsTCP_Remove.push_back(it->first);
					std::cout << "_tcpsock " << WSAGetLastError() << std::endl;
					FD_ZERO(&client_tcp_set);
				}
				else if (error == WSAEWOULDBLOCK) {
					//std::cout << "_tcpsock " << WSAEWOULDBLOCK << std::endl;;
				}
				else
				{
					std::cout << "_tcpsock " << WSAGetLastError() << std::endl;
				}
		

				
			}
		} 
		

	}

	for (std::vector<sockaddr>::iterator it = ObservableClientsTCP_Remove.begin(); it != ObservableClientsTCP_Remove.end(); it++) {
		SSLostConnectionToClient(ObservableClientsTCP[*it],*it);
	 }
	ObservableClientsTCP_Remove.clear();

	
	//Find any other messages in bufer
	while (FD_ISSET(_udpsock, &udp_set))
	{
		//Recheck
		FD_ZERO(&udp_set); FD_SET(_udpsock, &udp_set);
		int activity_tcp = select(0, &udp_set, NULL, NULL, &timeout);
		if (activity_tcp < 0) break;

		SocketMessage buffer;
		sockaddr senderAddr;
		int addrLen = sizeof(senderAddr);
		int recvdata = recvfrom(_udpsock, (char*)&buffer, sizeof(SocketMessage), 0, (struct sockaddr*)&senderAddr, &addrLen);
		if (recvdata > 0) {

			buffer.address_from = senderAddr;
			buffer.Recieved_PROTOCOL = IPPROTO_UDP;
			OnServerMessageRecievedPreview(&buffer, recvdata);

		}
		else if (recvdata == WSAEWOULDBLOCK) {
			std::cout << "_udpsock " << WSAEWOULDBLOCK;
		}

	}
	




}

//Fire, when Neww Add new Client (Preview, Because it semi-method, need UDP also), UDP-Done
void Socket::OnClientPreviewAdd(sockaddr addr,SOCKET new_socket,int protocol)
{
	std::map<sockaddr, SOCKET, SockaddrComparator>& ObservableClients = protocol == IPPROTO_TCP ? ObservableClientsTCP : ObservableClientsUDP;
	ObservableClients[addr] = new_socket;
	std::cout << "OnClientPreviewAdd :" << protocol << " :" << new_socket << std::endl;;

}

void Socket::OnServerMessageRecievedPreview(SocketMessage* message, SOCKET sock)
{

	if (message->EqualID<JoinMessage>()) {
		JoinMessage* jm = message->GetDataAs<JoinMessage>();
		std::map<XUID, sockaddr>& ObservableClients = message->Recieved_PROTOCOL == IPPROTO_TCP ? ObservableClientsTCP_XUID : ObservableClientsUDP_XUID;
		//add-new
		if (ObservableClients.find(message->replicated_xuid) == ObservableClients.end()) {
			OnServerXuidAdd(message->replicated_xuid,message->_player_name_, message->address_from, message->Recieved_PROTOCOL);
			JoinMessage jm;	SocketMessage msg = SocketMessageFromConst(jm);

			//send to client back about server :) (ALL PLAYERS , GOSH) (LATER)

			for (std::map<XUID, sockaddr>::iterator client = ObservableClientsTCP_XUID.begin(); client != ObservableClientsTCP_XUID.end(); client++) {
				msg.replicated_xuid = client->first;
				msg.replicated_from = client->second;
				//player-name
				this->SendToClientsTUD(&msg);
			}

		

			msg.replicated_xuid = GetXUID();
			GetName(msg._player_name_); //send host info

			SOCKET tudsock = message->Recieved_PROTOCOL == IPPROTO_TCP ? sock : _udpsock;
			this->Send(tudsock, &message->address_from, &msg);


		}
	}
	else if (message->Recieved_PROTOCOL == IPPROTO_UDP){

		if (ObservableClientsUDP_XUID.find(message->replicated_xuid) == ObservableClientsUDP_XUID.end()) {
			OnServerXuidAdd(message->replicated_xuid, (const char*)&message->_player_name_, message->address_from, message->Recieved_PROTOCOL);
		}

	}
	if(message->EqualID<TestMessage>()) {
		//std::cout << "TestMessage &? from : "  << message->replicated_xuid << std::endl;;
	}
	if (message->replicate) {
		message->replicated = true;
		SendToClientsTUD(message);
	}
	FireEvent(CSSS_MESSAGE_RECIEVED)(message, sock);
}

void Socket::OnClientMessageRecievedPreview(SocketMessage* message, SOCKET sock)
{
	if (message->EqualID<JoinMessage>()) {
		JoinMessage* jm = message->GetDataAs<JoinMessage>();
		std::map<XUID, sockaddr>& ObservableClients = message->Recieved_PROTOCOL == IPPROTO_TCP ? ObservableClientsTCP_XUID : ObservableClientsUDP_XUID;
		//add-new
		if (ObservableClients.find(message->replicated_xuid) == ObservableClients.end()) {
			OnClientXuidAdd(message->replicated_xuid,(const char*) & message->_player_name_, message->address_from, message->Recieved_PROTOCOL);
		}
	}
	if (message->EqualID<LeftMessage>()) {

		LeftMessage* jm = message->GetDataAs<LeftMessage>();
		std::map<XUID, sockaddr>& ObservableClients = message->Recieved_PROTOCOL == IPPROTO_TCP ? ObservableClientsTCP_XUID : ObservableClientsUDP_XUID;
		//add-new
		if (ObservableClients.find(message->replicated_xuid) != ObservableClients.end()) {
			OnCSSSXuidRemove(message->replicated_xuid);
		}

	}
	if (message->EqualID<TestMessage>()) {
		std::cout << "TestMessage &? from : " << message->replicated_xuid << std::endl;;
	}
	FireEvent(CSSS_MESSAGE_RECIEVED)(message, sock);
}

void Socket::OnServerXuidAdd(XUID xuid,const char* name, sockaddr addr, int PROTOCOL)
{
	std::map<XUID, sockaddr>& ObservableClients = PROTOCOL == IPPROTO_TCP ? ObservableClientsTCP_XUID : ObservableClientsUDP_XUID;
	ObservableClients[xuid] = addr;
	std::cout << "OnServerXuidAdd :" << "Name : " << name  << " XUID : " << xuid << std::endl;
	if (PROTOCOL == IPPROTO_TCP) FireEvent(CSSS_XUID_ADD)(xuid,name);

}

void Socket::OnClientXuidAdd(XUID xuid,const char* name, sockaddr addr, int PROTOCOL)
{
	std::map<XUID, sockaddr>& ObservableClients = PROTOCOL == IPPROTO_TCP ? ObservableClientsTCP_XUID : ObservableClientsUDP_XUID;
	ObservableClients[xuid] = addr;
	std::cout << "OnClientXuidAdd :" << "Name : " << name << " XUID : " << xuid << std::endl;
	if (PROTOCOL == IPPROTO_TCP) FireEvent(CSSS_XUID_ADD)(xuid,name);

}

//TCP-only
void Socket::CSJoinToServer()
{
	if (connection_status == 1) return;
	JoinMessage jm;	SocketMessage msg = SocketMessageFromConst(jm); 
	SendToServerTCP(&msg);
	SendToServerUDP(&msg);
	connection_status = 1;

}

//TCP-only
void Socket::CSLostConnectionToServer()
{
	connection_status = -1;
	std::cout << "CSLostConnectionToServer :" << std::endl;

	std::vector<XUID> ObservableClientsTCP_XUID_remove;
	for (std::map<XUID, sockaddr>::iterator client = ObservableClientsTCP_XUID.begin(); client != ObservableClientsTCP_XUID.end(); client++) {
		XUID client_xuid = client->first;
		ObservableClientsTCP_XUID_remove.push_back(client_xuid);
	}
	for (std::vector<XUID>::iterator it = ObservableClientsTCP_XUID_remove.begin(); it != ObservableClientsTCP_XUID_remove.end(); it++) {
		OnCSSSXuidRemove(*it);
	}

}

void Socket::SSLostConnectionToClient(SOCKET socket, sockaddr addr)
{
	closesocket(socket);

	std::vector<XUID> ObservableClientsTCP_XUID_remove;
	for (std::map<XUID, sockaddr>::iterator client = ObservableClientsTCP_XUID.begin(); client != ObservableClientsTCP_XUID.end(); client++) {
		sockaddr client_addr = client->second;
		XUID client_xuid = client->first;
		if (memcmp(&addr, &client_addr, sizeof(sockaddr)) == 0) {
			LeftMessage messsage; SocketMessage msg = SocketMessageFromConst(messsage); msg.replicate = 1; msg.replicated_xuid = client_xuid;
			SendToClientsTCP(&msg);
			ObservableClientsTCP_XUID_remove.push_back(client_xuid);
		}
	}
	for (std::vector<XUID>::iterator it = ObservableClientsTCP_XUID_remove.begin(); it != ObservableClientsTCP_XUID_remove.end(); it++) {
		OnCSSSXuidRemove(*it);
	}
	ObservableClientsTCP_XUID_remove.clear();
	ObservableClientsTCP.erase(addr);
}

void Socket::OnCSSSXuidRemove(XUID xuid)
{
	ObservableClientsTCP_XUID.erase(xuid);
	ObservableClientsUDP_XUID.erase(xuid);
	//EVENT-PLACE
	std::cout << (server ? "[Server]" :"[Client]") << " XUID Removed " << xuid << std::endl;
	FireEvent(CSSS_XUID_REMOVE)(xuid);
	
}


void Socket::Send(SOCKET sock, sockaddr* addr, SocketMessage* msg) 
{
	msg->sender_xuid = GetXUID();
	msg->address_from = msg->Recieved_PROTOCOL == IPPROTO_TCP ? mytcp_addr : myudp_addr; 
	sendto(sock, (char*)msg, sizeof(SocketMessage), 0,addr,sizeof(sockaddr));
}

//Cliserv (ClientServer)
void Socket::SendCliservTUD(SocketMessage* message)
{
	if (this->server) {
		this->SendToClientsTUD(message);
	}
	else
	{
		this->SendToServerTUD(message);
	}
}


void Socket::SendToServerTCP(SocketMessage* msg)
{
	
	msg->Recieved_PROTOCOL = IPPROTO_TCP;
	msg->Target_Protocol = IPPROTO_TCP;
	msg->replicated_from = mytcp_addr;
	msg->replicated_xuid = GetXUID();
	GetName(msg->_player_name_);

	this->Send(this->_tcpsock, &this->tcp_addr, msg);
}

void Socket::SendToServerUDP(SocketMessage* msg)
{
	msg->Recieved_PROTOCOL = IPPROTO_UDP;
	msg->Target_Protocol = IPPROTO_UDP;
	msg->replicated_from = myudp_addr;
	msg->replicated_xuid = GetXUID();
	GetName(msg->_player_name_);
	this->Send(this->_udpsock, &this->udp_addr, msg);
}

void Socket::SendToServerTUD(SocketMessage* msg)
{
	msg->Recieved_PROTOCOL = msg->Target_Protocol;
	msg->replicated_from = msg->address_from = msg->Recieved_PROTOCOL == IPPROTO_TCP ? mytcp_addr : myudp_addr;
	msg->replicated_xuid = GetXUID();
	GetName(msg->_player_name_);
	sockaddr* tud_addr = msg->Recieved_PROTOCOL == IPPROTO_TCP ? &tcp_addr : &udp_addr;
	SOCKET tudsock = msg->Recieved_PROTOCOL == IPPROTO_TCP ? _tcpsock : _udpsock;
	this->Send(tudsock, tud_addr, msg);
}

void Socket::SendToClientsTCP(SocketMessage* msg)
{
	for (std::map<XUID, sockaddr>::iterator client = ObservableClientsTCP_XUID.begin(); client != ObservableClientsTCP_XUID.end(); client++) {
		sockaddr client_addr = client->second;
		XUID client_xuid = client->first;
		SOCKET client_socket = ObservableClientsTCP[client_addr];
		Send(client_socket, &client_addr, msg);
	}
}

void Socket::SendToClientsUDP(SocketMessage* msg)
{
	for (std::map<XUID, sockaddr>::iterator client = ObservableClientsUDP_XUID.begin(); client != ObservableClientsUDP_XUID.end(); client++) {
		sockaddr client_addr = client->second;
		XUID client_xuid = client->first;
		Send(_udpsock, &client_addr, msg);
	}
}

//not fininshed (send & if replicated then not to send who was in first place)
void Socket::SendToClientsTUD(SocketMessage* msg)
{
	std::map<XUID, sockaddr>& ObservableClients = msg->Target_Protocol == IPPROTO_TCP ? ObservableClientsTCP_XUID : ObservableClientsUDP_XUID;
	std::map<sockaddr, SOCKET, SockaddrComparator>& ObservableClients_NX = msg->Target_Protocol == IPPROTO_TCP ? ObservableClientsTCP : ObservableClientsUDP;

	for (std::map<XUID, sockaddr>::iterator client = ObservableClients.begin(); client != ObservableClients.end(); client++) {
		
		if (msg->replicate && memcmp((char*)&msg->replicated_from, (char*)&client->second, sizeof(sockaddr)) == 0) {
			continue;
		};
		sockaddr client_addr = client->second;
		XUID client_xuid = client->first;
		SOCKET client_socket = ObservableClients_NX[client_addr];
		Send(client_socket, &client_addr, msg);
	}

}

XUID Socket::GetXUID(int index)
{
#ifdef _XBOX
	XUID xuid;
	XUserGetXUID(index, &xuid);
	return xuid;
#else

	HANDLE hProcess = GetCurrentProcess();
	FILETIME creationTime, exitTime, kernelTime, userTime;

	if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime)) {
	}


	return creationTime.dwLowDateTime;
#endif
}

void Socket::GetName(char buffer[SOCKET_PLAYER_NAME_SIZE])
{
	memset(buffer, 0, SOCKET_PLAYER_NAME_SIZE);
#ifdef _XBOX
	XUserGetName(0, buffer, SOCKET_PLAYER_NAME_SIZE);
#elif _SERVER
	memcpy(buffer, "_SERVER", 8);
#else
	memcpy(buffer, "_CLIENT", 8);
#endif
}

