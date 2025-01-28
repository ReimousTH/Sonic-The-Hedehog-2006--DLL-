#include "Socket.h"

const char* Socket::IP_ADDR;

Socket::Socket() {

	InitSockets();
	_clients = std::map<sockaddr, SocketData, SockaddrComparator>();
	_udp_clients_map = std::map<sockaddr, bool, SockaddrComparator>();

	this->MSG_HANDLE_CLIENT_JOIN_SERVER = &this->MSG_HANDLE_CLIENT_JOIN_SERVER_TEMP;
	this->MSG_HANDLE_CLIENT_LOST_CONNECTION_SERVER = &this->MSG_HANDLE_CLIENT_LOST_CONNECTION_SERVER_TEMP;
	this->MSG_HANDLE_CLIENT_MESSAGES = &this->MSG_HANDLE_CLIENT_MESSAGES_TEMP;
	this->MSG_HANDLE_SERVER_CLIENT_JOIN = &this->MSG_HANDLE_SERVER_CLIENT_JOIN_TEMP;
	this->MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION = &this->MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION_TEMP;
	this->MSG_HANDLE_SERVER_MESSAGES = &this->MSG_HANDLE_SERVER_MESSAGES_TEMP;
	this->MSG_HANDLE_SERVER_XUI_JOIN = &this->MSG_HANDLE_SERVER_XUI_JOIN_TEMP;
	this->MSG_HANDLE_SERVER_XUI_LEFT = &this->MSG_HANDLE_SERVER_XUI_LEFT_TEMP;
	this->MSG_HANDLE_CLIENT_XUI_JOIN = &this->MSG_HANDLE_CLIENT_XUI_JOIN_TEMP;
	this->MSG_HANDLE_CLIENT_XUI_LEFT = &this->MSG_HANDLE_CLIENT_XUI_LEFT_TEMP;

}

Socket::~Socket() {
	Cleanup();
}

void Socket::InitSockets() {
	_client = false;
	_server = false;
	_connection_status = -2;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
		return;
	}

	_tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_tcpSocket == INVALID_SOCKET) {
		std::cerr << "TCP socket creation failed with error: " << WSAGetLastError() << std::endl;
		return;
	}

	_udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_udpSocket == INVALID_SOCKET) {
		std::cerr << "UDP socket creation failed with error: " << WSAGetLastError() << std::endl;
		return;
	}




}

void Socket::SetBind() {


	if (bind(_tcpSocket, (struct sockaddr*)&_address_to, sizeof(_address_to)) == SOCKET_ERROR) {
		std::cerr << "TCP bind failed with error: " << WSAGetLastError() << std::endl;
		//   exit(1);
	}

	if (bind(_udpSocket, (struct sockaddr*)&_address_to, sizeof(_address_to)) == SOCKET_ERROR) {
		std::cerr << "UDP bind failed with error: " << WSAGetLastError() << std::endl;
		//  exit(1);
	}




}

void Socket::SetAddress(const char* address, short port = 27016)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(address);
	this->_address_to = *(struct sockaddr*)&addr;
}

void Socket::SetNonBlockingMode()
{
	u_long mode = 1; // 1 to enable non-blocking mode, 0 to disable
	if (ioctlsocket(_tcpSocket, FIONBIO, &mode) == SOCKET_ERROR) {
		std::cerr << "TCP socket failed to set non blocking mode " << WSAGetLastError() << std::endl;
		//   return;
	}
	if (ioctlsocket(_udpSocket, FIONBIO, &mode) == SOCKET_ERROR) {
		std::cerr << "UDP socket failed to set non blocking mode" << WSAGetLastError() << std::endl;
		// return;
	}
}

void Socket::Cleanup() {
	_connection_status = -2;
	closesocket(_tcpSocket);
	closesocket(_udpSocket);
	_tcpSocket = INVALID_SOCKET;
	_udpSocket = INVALID_SOCKET;
	_clients.clear();
	_udp_clients_map.clear();
	WSACleanup();
}

bool Socket::IsWorks()
{
	return _tcpSocket != INVALID_SOCKET || _udpSocket != INVALID_SOCKET;
}

bool Socket::IsClient()
{
	return _client;
}

bool Socket::IsServer()
{
	return _server;
}

int Socket::GetConnectStatus()
{
	return _connection_status;
}

sockaddr Socket::GetAddress()
{
	sockaddr addr;
	int size = sizeof(addr);
	getsockname(_tcpSocket, &addr, &size);
	return addr;
}

sockaddr Socket::GetAddressTo()
{
	return _address_to;
}

void Socket::InitServer()
{
	_server = true;
	listen(_tcpSocket, 0);
}
int Socket::InitClient() {
	_client = true;
	// TCP ONLY, because UDP is not needed
	sockaddr addr = GetAddressTo();
	int size = sizeof(addr);
	int error;
	int optlen;

	// Attempt to connect to the server
	int result = connect(_tcpSocket, &addr, size);
	if (result == SOCKET_ERROR) {
		int wsa_error = WSAGetLastError();

		switch (wsa_error) {
		case WSAEINPROGRESS:
		case WSAEWOULDBLOCK:
			// Connection is in progress
			_connection_status = -1;
			std::cerr << "Connection is in progress, handle it accordingly." << std::endl;
			return _connection_status; // Indicate that the connection is being established

		case WSAEISCONN:
			// Socket is already connected, but the connection may not be successful
			_connection_status = 0;
			std::cerr << "Socket is already connected, but the connection may not be successful." << std::endl;

			// Check the actual connection status

			optlen = sizeof(error);
			if (getsockopt(_tcpSocket, SOL_SOCKET, 0x1007, (char*)&error, &optlen) == SOCKET_ERROR) {
				_connection_status = -1;
				std::cerr << "getsockopt failed: " << WSAGetLastError() << std::endl;
				return _connection_status; // Indicate failure
			}

			if (error == 0) {
				// Connection is successful
				_connection_status = 1;
				std::cout << "Connection established successfully." << std::endl;
				this->MSG_HANDLE_CLIENT_JOIN_SERVER_BEHAVIOUR(this, result);
				return _connection_status; // Indicate success
			}
			else {
				// Connection failed
				_connection_status = -1;
				std::cerr << "Connection failed with error: " << error << std::endl;
				return _connection_status; // Indicate failure
			}
		default:
			// Handle other connection errors
			_connection_status = -1;
			std::cerr << "[WSA][Error] : " << wsa_error << " - " << wsa_error << std::endl;
			return _connection_status; // Indicate failure
		}
	}

	// Connection established successfully
	_connection_status = 1;
	std::cout << "Connection established successfully." << std::endl;
	this->MSG_HANDLE_CLIENT_JOIN_SERVER_BEHAVIOUR(this, result);




	return 1; // Indicate success
}
void Socket::UpdateServer(float delta) {
	fd_set readfds_tcp;
	fd_set readfds_upd;
	FD_ZERO(&readfds_tcp);
	FD_ZERO(&readfds_upd);
	FD_SET(_tcpSocket, &readfds_tcp);
	FD_SET(_udpSocket, &readfds_upd);

	//select ignore nonblock
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0; //

	int activity_tcp = select(0, &readfds_tcp, NULL, NULL, &timeout);
	int activity_udp = select(0, &readfds_upd, NULL, NULL, &timeout);
	if (activity_tcp < 0 && activity_udp < 0) {
		std::cerr << "Select error: " << WSAGetLastError() << std::endl;
		return;
	}



	// TCP (JOIN HANDLE)
	if (FD_ISSET(_tcpSocket, &readfds_tcp)) {
		SocketMessage buffer;
		sockaddr senderAddr;  // Use sockaddr_in for better type safety
		int addrLen = sizeof(senderAddr);
			
		SOCKET socket = accept(_tcpSocket, (struct sockaddr*)&senderAddr, &addrLen);
		if (socket != INVALID_SOCKET) {
			// Check if the client is already connected
			if (_clients.find(senderAddr) == _clients.end()) {
				sockaddr_in* _edit = (struct sockaddr_in*)&senderAddr;
				_clients[senderAddr] = SocketData(socket);
				this->MSG_HANDLE_SERVER_CLIENT_JOIN_BEHAVIOUR(this, socket);
			}
		}
	}

	std::vector< std::map<sockaddr, SocketData, SockaddrComparator>::iterator> delete_request_vector;
	// Check each client socket
	for (std::map<sockaddr, SocketData, SockaddrComparator>::iterator it = _clients.begin(); it != _clients.end();) {
		fd_set client_socket;
		FD_ZERO(&client_socket);
		FD_SET(it->second.TCP_SOCKET, &client_socket);


		SocketMessage buffer;
		sockaddr_in senderAddr;
		int addrLen = sizeof(senderAddr);
		int bytesReceived = recvfrom(it->second.TCP_SOCKET, (char*)&buffer, sizeof(SocketMessage), 0, (struct sockaddr*)&senderAddr, &addrLen);
		buffer.address_from = it->first;

		if (bytesReceived > 0) {
			// Process received TCP data
			buffer.replicated_xuid = buffer.sender_xuid; // save
			this->MSG_HANDLE_SERVER_MESSAGES_BEHAVIOUR(this, it->second.TCP_SOCKET, &buffer);
		}
		else if (bytesReceived == 0) {
			// Handle lost connection

			this->MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION_BEHAVIOUR(this, it->second.TCP_SOCKET);
			delete_request_vector.push_back(it);
			it++;
			continue; // Skip to the next iteration
		}
		else {
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK) {
				perror("recv");
				this->MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION_BEHAVIOUR(this, it->second.TCP_SOCKET);
				delete_request_vector.push_back(it);
				it++;
				continue; // Skip to the next iteration
			}
		}
		++it; // Move to the next client
	}

	//safe erase
	for (std::vector< std::map<sockaddr, SocketData, SockaddrComparator>::iterator>::iterator it = delete_request_vector.begin(); it != delete_request_vector.end(); ++it) {
		_clients.erase(*it);
	}




	// UDP
	while (FD_ISSET(_udpSocket, &readfds_upd))
	{

		FD_ZERO(&readfds_upd);
		FD_SET(_udpSocket, &readfds_upd);
		int activity_udp_l = select(0, &readfds_upd, NULL, NULL, &timeout);
		if (activity_udp_l < 0) break;

		SocketMessage buffer;
		sockaddr_in senderAddr;
		int addrLen = sizeof(senderAddr);
		int bytesReceived = recvfrom(_udpSocket, (char*)&buffer, sizeof(SocketMessage), 0, (struct sockaddr*)&senderAddr, &addrLen);
		*(struct sockaddr_in*)&buffer.address_from = senderAddr;



		if (bytesReceived > 0) {

			if (_udp_clients_map.find(*(struct sockaddr*)&senderAddr) == _udp_clients_map.end()) {
				_udp_clients_map[*(struct sockaddr*)&senderAddr] = true;
			}

			// Process received UDP data
			buffer.replicated_xuid = buffer.sender_xuid; // save
			this->MSG_HANDLE_SERVER_MESSAGES_BEHAVIOUR(this, NULL, &buffer);
		}
	}
}

void Socket::UpdateClient(float delta)
{
	fd_set readfds_tcp;
	fd_set readfds_upd;
	FD_ZERO(&readfds_tcp);
	FD_ZERO(&readfds_upd);
	FD_SET(_tcpSocket, &readfds_tcp);
	FD_SET(_udpSocket, &readfds_upd);

	//select ignore nonblock
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0; //

	int activity_tcp = select(0, &readfds_tcp, NULL, NULL, &timeout);
	int activity_udp = select(0, &readfds_upd, NULL, NULL, &timeout);
	if (activity_tcp < 0 && activity_udp < 0) {
		std::cerr << "Select error: " << WSAGetLastError() << std::endl;
		return;
	}

	while (FD_ISSET(_tcpSocket, &readfds_tcp)) {
		FD_ZERO(&readfds_tcp);
		FD_SET(_tcpSocket, &readfds_tcp);
		int activity_udp_l = select(0, &readfds_tcp, NULL, NULL, &timeout);
		if (activity_udp_l < 0) break;


		SocketMessage buffer;
		int bytesReceived = recv(_tcpSocket, (char*)&buffer, sizeof(SocketMessage), 0);
		if (bytesReceived > 0) {
			// Process received TCP data
			this->MSG_HANDLE_CLIENT_MESSAGES_BEHAVIOUR(this, _tcpSocket, &buffer);
		}
		else if (bytesReceived == 0) {
			// Handle lost connection
			_connection_status = 0;
			this->MSG_HANDLE_CLIENT_LOST_CONNECTION_SERVER(this, _tcpSocket);
			Cleanup();
		}
		else {
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK) {
				perror("recv");
				_connection_status = 0;
				this->MSG_HANDLE_CLIENT_LOST_CONNECTION_SERVER(this, _tcpSocket);
				Cleanup();
			}
		}
	}



	while (FD_ISSET(_udpSocket, &readfds_upd))
	{
		FD_ZERO(&readfds_upd);
		FD_SET(_udpSocket, &readfds_upd);
		int activity_udp_l = select(0, &readfds_upd, NULL, NULL, &timeout);
		if (activity_udp_l < 0) break;

		SocketMessage buffer;
		int addrLen = sizeof(sockaddr);
		int bytesReceived = recvfrom(_udpSocket, (char*)&buffer, sizeof(SocketMessage), 0, (struct sockaddr*)&buffer.address_from, &addrLen);
		if (bytesReceived > 0) {
			// Process received UDP data
			this->MSG_HANDLE_CLIENT_MESSAGES_BEHAVIOUR(this, NULL, &buffer);
		}

	}


}

void Socket::SendTCPMessageTo(SOCKET to, SocketMessage* msg)
{
	msg->sender_xuid = GetXUID(0);
	if (msg->PROTOCOL == IPPROTO_TCP) {
		send(to, (char*)msg, sizeof(*msg), 0);
	}
}

void Socket::SendTCPMessageToSRCL(SocketMessage* msg)
{
	if (IsClient()) {
		this->SendTCPMessageToServer(msg);
	}
	else if (IsServer()) {
		this->SendTCPMessageToClients(msg);
	}
}

void Socket::SendTCPMessageToServer(SocketMessage* msg)
{
	SendTCPMessageTo(_tcpSocket, msg);
}

void Socket::SendTCPMessageToClients(SocketMessage* msg)
{
	for (std::map<sockaddr, SocketData, SockaddrComparator>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		SendTCPMessageTo(it->second.TCP_SOCKET, msg);
	}
}

void Socket::SendTCPMessageToXUID(XUID to, SocketMessage* msg)
{
	SendTCPMessageTo(MatchClientXUIDToTCPSocket(to), msg);
}

void Socket::SendUDPMessageTo(sockaddr to, SocketMessage* msg)
{
	msg->sender_xuid = GetXUID(0);
	if (msg->PROTOCOL == IPPROTO_UDP) {
		sendto(_udpSocket, (char*)msg, sizeof(*msg), 0, (struct sockaddr*)&to, sizeof(to));
	}
}

void Socket::SendUDPMessageToSRCL(SocketMessage* msg)
{
	if (IsClient()) {
		this->SendUDPMessageToServer(msg);
	}
	else if (IsServer()) {
		this->SendUDPMessageToClients(msg);
	}

}

void Socket::SendUDPMessageToServer(SocketMessage* msg)
{
	SendUDPMessageTo(GetAddressTo(), msg);
}

void Socket::SendUDPMessageToClients(SocketMessage* msg)
{
	for (std::map<sockaddr, SocketData, SockaddrComparator>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		SendUDPMessageTo(MatchClientTcpToUdpAddress(it->first), msg);
	}
}

sockaddr Socket::MatchClientTcpToUdpAddress(sockaddr tcp_address)
{
	sockaddr_in in_address = *(struct sockaddr_in*)&tcp_address;
	for (std::map<sockaddr, bool, SockaddrComparator>::iterator it = _udp_clients_map.begin(); it != _udp_clients_map.end(); it++) {
		sockaddr_in in_udp_address = *(struct sockaddr_in*)&it->first;
		if (memcmp(&in_address.sin_addr.S_un.S_un_b, &in_udp_address.sin_addr.S_un.S_un_b, 4) == 0) {
			return *(struct sockaddr*)&in_udp_address;
		}
	}
	return tcp_address;
}

SOCKET Socket::MatchClientXUIDToTCPSocket(XUID xuid)
{

	for (std::map<sockaddr, SocketData, SockaddrComparator>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second.xuid == xuid) {
			return it->second.TCP_SOCKET;
		}

	}
	return -1;
}


XUID Socket::MatchClientXUIDByTCPSocket(SOCKET tcp_socket)
{

	for (std::map<sockaddr, SocketData, SockaddrComparator>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second.TCP_SOCKET == tcp_socket) {
			return it->second.xuid;
		}

	}
	return -1;
}



XUID Socket::GetXUID(int index)
{
	XUID xuid;
#ifdef _XBOX
	XUserGetXUID(index, &xuid);
#else
	FILETIME lpCreationTime, lpExitTime, lpKernelTime, lpUserTim;
	GetProcessTimes(GetCurrentProcess(), &lpCreationTime, &lpExitTime, &lpKernelTime, &lpUserTim);
	return (XUID)lpCreationTime.dwLowDateTime | (XUID)lpCreationTime.dwHighDateTime << 32;
#endif
	return xuid;
}

void Socket::GetName(char* buffer)
{


#ifdef _XBOX
	XUserGetName(0, buffer, 64);
#else
	memcpy(buffer, "test", 5);
#endif

}

std::string Socket::XUIDToName(XUID xuid)
{
	for (std::map<sockaddr, SocketData, SockaddrComparator>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second.xuid == xuid) {
			return std::string(it->second.name);
		}
	}
	return std::string("NULL");
}


void Socket::MSG_HANDLE_SERVER_MESSAGES_BEHAVIOUR(Socket* _1, SOCKET sock, SocketMessage* msg)
{
	//TCP (client - send his xuid to server)
	if (msg->ID == SMDataJoinXUID::GetID()) {
		SMDataJoinXUID* _data_ = EXTRACT_SOCKET_MESSAGE_FROM_MESSAGE_PTR(msg, SMDataJoinXUID);

		if (_clients.find(msg->address_from) != _clients.end() && _clients[msg->address_from].xuid == -1) {
			//XUIADDED_SERVER
			//AskOthersUpdateInfo
			SMDataJoinXUID* f = EXTRACT_SOCKET_MESSAGE_FROM_MESSAGE_PTR(msg, SMDataJoinXUID);
			_clients[msg->address_from].xuid = f->sender_xuid;
			memcpy(&_clients[msg->address_from].name, &f->sender_name, 64);
			this->MSG_HANDLE_SERVER_XUI_JOIN_BEHAVIOUR(_1, sock, f->sender_xuid);
		}
	}
	//replicateable messages
	if (msg->replicate == SM_REPLICATE) {
		if (msg->PROTOCOL == IPPROTO_TCP) {
			SendTCPMessageToClients(msg);
		}
		else
		{
			SendUDPMessageToClients(msg);
		}

	}
	//Lost XUID for CLIENT_LEFT_FROM_SERVER;
	//Replicate ALL EXISTRING XUIDS




_NAH:
	this->MSG_HANDLE_SERVER_MESSAGES(_1, sock, msg);
}

void Socket::MSG_HANDLE_SERVER_MESSAGES_TEMP(Socket*, SOCKET, SocketMessage* msg)
{

	if (msg->PROTOCOL == IPPROTO_UDP) {
		printf("MSG_HANDLE_SERVER_MESSAGES_TEMP_UDP\n");
	}
	else {
		printf("MSG_HANDLE_SERVER_MESSAGES_TEMP\n");
	}
}

void Socket::MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION_BEHAVIOUR(Socket* _1, SOCKET sock)
{
	XUID left_client = MatchClientXUIDByTCPSocket(sock);
	this->MSG_HANDLE_SERVER_XUI_LEFT_BEHAVIOUR(_1, sock, left_client);
	this->MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION(_1, sock);
}

void Socket::MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION_TEMP(Socket*, SOCKET)
{
	printf("MSG_HANDLE_SERVER_CLIENT_LOST_CONNECTION_TEMP\n");

}

void Socket::MSG_HANDLE_SERVER_CLIENT_JOIN_BEHAVIOUR(Socket* _1, SOCKET sock)
{
	this->MSG_HANDLE_SERVER_CLIENT_JOIN(_1, sock);
}

void Socket::MSG_HANDLE_SERVER_CLIENT_JOIN_TEMP(Socket*, SOCKET)
{
	printf("MSG_HANDLE_SERVER_CLIENT_JOIN_TEMP\n");
}

void Socket::MSG_HANDLE_CLIENT_MESSAGES_BEHAVIOUR(Socket* _1, SOCKET tsocket, SocketMessage* msg)
{
	if (msg->replicated_xuid == GetXUID(0)) return; // ignore my 

	//TCP
	if (msg->ID == SMDataJoinXUID::GetID()) {
		SMDataJoinXUID* _data_ = EXTRACT_SOCKET_MESSAGE_FROM_MESSAGE_PTR(msg, SMDataJoinXUID);
		if (_data_->sender_xuid == this->GetXUID(0)) goto _NAH;
		if (_clients.find(_data_->address) == _clients.end()) {
			_clients[_data_->address].xuid = _data_->sender_xuid;
			memcpy(&_clients[_data_->address].name, &_data_->sender_name, 64);
			//OnXUIDJoin//
			this->MSG_HANDLE_CLIENT_XUI_JOIN_BEHAVIOUR(_1, tsocket, _data_->sender_xuid);
			//////////////
		}
	}

	//TCP
	if (msg->ID == SMDataLeftXUID::GetID()) {
		SMDataLeftXUID* _data_ = EXTRACT_SOCKET_MESSAGE_FROM_MESSAGE_PTR(msg, SMDataLeftXUID);
		if (_data_->sender_xuid == this->GetXUID(0)) goto _NAH;
		if (_clients.find(_data_->address) != _clients.end()) {
			_clients.erase(_clients.find(_data_->address));
			//OnXUIDLeft//
			this->MSG_HANDLE_CLIENT_XUI_LEFT_BEHAVIOUR(_1, tsocket, _data_->sender_xuid);
			//////////////
		}
	}
_NAH:
	this->MSG_HANDLE_CLIENT_MESSAGES(_1, tsocket, msg);
}

void Socket::MSG_HANDLE_CLIENT_MESSAGES_TEMP(Socket*, SOCKET, SocketMessage* msg)
{
	if (msg->PROTOCOL == IPPROTO_UDP) {
		printf("MSG_HANDLE_CLIENT_MESSAGES_TEMP_UDP\n");
	}
	else {
		printf("MSG_HANDLE_SERVER_MESSAGES_TEMP\n");
	}
}



void Socket::MSG_HANDLE_CLIENT_JOIN_SERVER_BEHAVIOUR(Socket* _1, SOCKET sock)
{

	//Send Client XUID to Host
	{
		SMDataJoinXUID _msg_data;
		_msg_data.sender_xuid = this->GetXUID(0);
		this->GetName((char*)&_msg_data.sender_name);
		SocketMessage msg = DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_msg_data);
		this->SendTCPMessageToServer(&msg);
	}
	this->MSG_HANDLE_CLIENT_JOIN_SERVER(_1, sock);
}

void Socket::MSG_HANDLE_CLIENT_JOIN_SERVER_TEMP(Socket*, SOCKET)
{
	printf("MSG_HANDLE_CLIENT_JOIN_SERVER_TEMP\n");
}

void Socket::MSG_HANDLE_CLIENT_LOST_CONNECTION_SERVER_TEMP(Socket*, SOCKET)
{
	printf("MSG_HANDLE_CLIENT_LOST_CONNECTION_SERVER_TEMP\n");
}

void Socket::MSG_HANDLE_SERVER_XUI_JOIN_BEHAVIOUR(Socket* _1, SOCKET sock, XUID xuid)
{
	//Send Host XUID
	{
		SMDataJoinXUID _msg_data;
		_msg_data.address = this->GetAddress();
		_msg_data.sender_xuid = this->GetXUID(0);
		this->GetName((char*)&_msg_data.sender_name);
		SocketMessage msg = DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_msg_data);
		this->SendTCPMessageToClients(&msg);
	}

	for (std::map<sockaddr, SocketData, SockaddrComparator>::iterator it = _clients.begin(); it != _clients.end(); it++) {
		SMDataJoinXUID _msg_data;
		_msg_data.address = it->first;
		_msg_data.sender_xuid = it->second.xuid;

		memcpy(&_msg_data.sender_name, &it->second.name, 64);



		SocketMessage msg = DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_msg_data);
		this->SendTCPMessageToClients(&msg);
	}


	this->MSG_HANDLE_SERVER_XUI_JOIN(_1, sock, xuid);
}

void Socket::MSG_HANDLE_SERVER_XUI_JOIN_TEMP(Socket*, SOCKET, XUID xuid)
{
	printf("MSG_HANDLE_SERVER_XUI_JOIN_TEMP %x\n", xuid);
}

void Socket::MSG_HANDLE_SERVER_XUI_LEFT_BEHAVIOUR(Socket* _1, SOCKET sock, XUID xuid)
{
	//Ask Players To Remove XUID
	{
		SMDataLeftXUID _msg_data;
		_msg_data.sender_xuid = xuid;
		SocketMessage msg = DEFINE_SOCKET_MESSAGE_FROM_CONST_DATA(_msg_data);
		this->SendTCPMessageToClients(&msg);
	}


	this->MSG_HANDLE_SERVER_XUI_LEFT(_1, sock, xuid);
}

void Socket::MSG_HANDLE_SERVER_XUI_LEFT_TEMP(Socket*, SOCKET, XUID xuid)
{
	printf("MSG_HANDLE_SERVER_XUI_LEFT_TEMP %x\n ", xuid);
}

void Socket::MSG_HANDLE_CLIENT_XUI_JOIN_BEHAVIOUR(Socket* _1, SOCKET sock, XUID xuid)
{
	this->MSG_HANDLE_CLIENT_XUI_JOIN(_1, sock, xuid);
}

void Socket::MSG_HANDLE_CLIENT_XUI_JOIN_TEMP(Socket*, SOCKET, XUID xuid)
{
	printf("MSG_HANDLE_CLIENT_XUI_JOIN_TEMP %x\n ", xuid);
}

void Socket::MSG_HANDLE_CLIENT_XUI_LEFT_BEHAVIOUR(Socket* _1, SOCKET sock, XUID xuid)
{
	this->MSG_HANDLE_CLIENT_XUI_LEFT(_1, sock, xuid);
}

void Socket::MSG_HANDLE_CLIENT_XUI_LEFT_TEMP(Socket*, SOCKET, XUID xuid)
{
	printf("MSG_HANDLE_CLIENT_XUI_LEFT_TEMP %x\n ", xuid);
}

SocketData::SocketData(int TCP_SOCKET)
{
	this->xuid = -1;
	this->TCP_SOCKET = TCP_SOCKET;
}

SocketData::SocketData()
{
	this->xuid = -1;
}



SocketMessage::SocketMessage(int ID, int PROTOCOL,int REPLICATE,void* from, int size)
{
	this->ID = ID;
	this->PROTOCOL = PROTOCOL;
	this->replicate = REPLICATE;
	memset(&this->_message_, 0, 256);
	memcpy((void*)&this->_message_, from, size);
}

SocketMessage::SocketMessage()
{

}
