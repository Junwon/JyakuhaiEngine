#include "Engine/Network/TCPSocket.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

TCPSocket::~TCPSocket()
{
	Close();
}

bool TCPSocket::Join(const char* addr)
{
	return Join(StringToNetAddress(addr));
}

bool TCPSocket::Join(NetAddress_T addr)
{
	if (IsValid()) {
		return false;
	}

	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		return false;
	}

	sockaddr_storage internalAddress;
	int addressSize = 0;
	SocketAddressFromNetAddress((sockaddr*)&internalAddress, &addressSize, addr);

	int result = ::connect(sock, (sockaddr*)&internalAddress, addressSize);

	if (result != 0) {
		::closesocket(sock);
		return false;
	}
	else {
		m_socket = sock;
		m_netAddress = addr;
		return true;
	}
}

void TCPSocket::Close()
{
	if (IsValid()){
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		m_isListeningSocket = false;
	}
}

bool TCPSocket::IsValid() const
{
	if ((m_socket != INVALID_SOCKET) == false && WSAGetLastError() != 0) {
		DebuggerPrintlnf("Socket Error: %d", WSAGetLastError());
	}
	return (m_socket != INVALID_SOCKET);
}

bool TCPSocket::Host(uint16_t port)
{
	bool connection = Listen(port);
	SetBlocking(false);

	return connection;
}

bool TCPSocket::Listen(const NetAddress_T& addr)
{
	return Listen(addr.port);
}

bool TCPSocket::Listen(uint16_t port)
{
	if (IsValid()) {
		return false;
	}

	std::vector<NetAddress_T> addresses = GetAddressesFromHostName("", port, true);
	if (addresses.empty()) {
		return false;
	}

	SOCKET listenSocket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		return false;
	}
	
	sockaddr_storage listenAddress;
	int addressSize = 0;
	SocketAddressFromNetAddress((sockaddr*)&listenAddress, &addressSize, addresses[0]);

	int result = ::bind(listenSocket, (sockaddr*)&listenAddress, (int)addressSize);
	if (result == SOCKET_ERROR) {
		::closesocket(listenSocket);
		return false;
	}

	result = ::listen(listenSocket, (int)MAX_QUEUED);
	if (result == SOCKET_ERROR) {
		::closesocket(listenSocket);
		return false;
	}

	m_socket = listenSocket;
	m_isListeningSocket = true;
	m_netAddress = addresses[0];
	return true;
}

bool TCPSocket::IsListening() const
{
	return m_isListeningSocket;
}

TCPSocket* TCPSocket::Accept()
{
	if (!m_isListeningSocket) {
		return nullptr;
	}
	
	sockaddr_storage connectionAddress;
	int connectionAddressLength = sizeof(connectionAddress);

	SOCKET acceptedSocket = ::accept(m_socket, (sockaddr*)&connectionAddress, &connectionAddressLength);
	if (acceptedSocket == INVALID_SOCKET) {
		return nullptr;
	}

	NetAddress_T netAddress;
	if (!NetAddressFromSocketAddress(&netAddress, (sockaddr*)&connectionAddress)) {
		::closesocket(acceptedSocket);
		return nullptr;
	}

	TCPSocket* acceptedTCPSocket = new TCPSocket();
	acceptedTCPSocket->m_socket = acceptedSocket;
	acceptedTCPSocket->m_netAddress = netAddress;

	return acceptedTCPSocket;
}

unsigned int TCPSocket::Send(const void* payload, unsigned int payloadSize)
{
	if (!IsValid()) {
		return 0;
	}

	if (IsListening()) {
		DebuggerPrintlnf("Error: Sent as listening socket;");
		return 0;
	}

	if (payloadSize == 0) {
		return 0;
	}

	int bytesSent = ::send(m_socket, (const char*)payload, (int)payloadSize, 0);
	if (bytesSent <= 0) {
		int error = WSAGetLastError();
		DebuggerPrintlnf("Error: %d", error);
		Close();
		return 0;
	}

	ASSERT_OR_DIE(bytesSent == (int)payloadSize, "Byte sent does not equal net data size.");

	return bytesSent;
}

unsigned int TCPSocket::Receive(void* payload, unsigned int maxPayloadSize)
{
	if (!IsValid() || (maxPayloadSize == 0)) {
		return 0;
	}

	if (IsListening()) {
		DebuggerPrintlnf("Error: Received as listening socket.");
		return 0;
	}

	ASSERT_OR_DIE(payload != nullptr, "Payload data is null.");

	int bytesRead = ::recv(m_socket, (char*)payload, (int)maxPayloadSize, 0);
	if (bytesRead <= 0) {
		if (bytesRead == SOCKET_ERROR) {
			int error = WSAGetLastError();
			if (error == WSAEWOULDBLOCK) {
				return 0;
			}
			else {
				Close();
				return 0;
			}
		}
		else {
			CheckForDisconnect();
			return 0;
		}
	}
	else {
		return (unsigned int)bytesRead;
	}
}

void TCPSocket::CheckForDisconnect()
{
	if (IsValid()) {
		WSAPOLLFD fileDescriptor;
		fileDescriptor.fd = m_socket;
		fileDescriptor.events = POLLRDNORM;

		if (SOCKET_ERROR == WSAPoll(&fileDescriptor, 1, 0)) {
			return;
		}

		if ((fileDescriptor.revents & POLLHUP) != 0) {
			Close();
		}
	}
}

void TCPSocket::SetBlocking(bool blocking)
{
	if (IsValid()) {
		u_long nonBlocking = blocking ? 0 : 1;
		::ioctlsocket(m_socket, FIONBIO, &nonBlocking);
	}
}

void TCPSocket::EnableNagle(bool enabled)
{
	if (!IsValid()) {
		return;
	}

	int flag = enabled ? 1 : 0;
	SOCKET sock = (SOCKET)m_isListeningSocket;
	int result = ::setsockopt(sock,
							  IPPROTO_TCP,
							  TCP_NODELAY,
							  (char *)&flag,
							  sizeof(int));

	ASSERT_OR_DIE(result != SOCKET_ERROR, Stringf("Failed to disable Nagle's Algorithm. Error: %u", WSAGetLastError()));
}