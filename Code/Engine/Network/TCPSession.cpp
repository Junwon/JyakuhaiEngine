#include "Engine/Network/TCPSession.hpp"

#include "Engine/Core/EngineCommon.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

#include "Engine/Network/TCPSocket.hpp"
#include "Engine/Network/TCPConnection.hpp"
#include "Engine/Network/LoopBackConnection.hpp"

TCPSession::TCPSession() :
	m_listenSocket(nullptr)
{
	m_onJoinResponse = new NetMessageDefinition();

	m_onJoinResponse->m_typeIndex = NETMSG_JOIN_RESPONSE;
	m_onJoinResponse->m_handler = std::bind(&TCPSession::OnJoinResponse, this, std::placeholders::_1);

	RegisterMessageDefinition(NETMSG_JOIN_RESPONSE, *m_onJoinResponse);
}

TCPSession::~TCPSession()
{
	SAFE_DELETE(m_listenSocket);
}

// Hosts a session
bool TCPSession::Host(uint16_t port)
{
	ASSERT_OR_DIE(!IsRunning(), "Session is not running.");

	m_myOwnConnection = new LoopBackConnection();

	m_myOwnConnection->m_address = GetMyAddress(port);

	JoinConnection(0, m_myOwnConnection);
	m_hostConnection = m_myOwnConnection;

	SetState(SESSION_CONNECTED);

	return true;
}

// Joins another session at addr
bool TCPSession::Join(const NetAddress_T& addr)
{
	DebuggerPrintlnf("Joining: %s", NetAddressToString(const_cast<NetAddress_T&>(addr)).c_str());
	TCPConnection* host = new TCPConnection();
	m_hostConnection = host;

	host->m_address = addr;

	JoinConnection(0, host); // 0 for this class; 
	host->m_socket = new TCPSocket();

	// Try to connect to host
	if (!host->Connect()) {
		DebuggerPrintlnf("Joining failed");
		Leave();
		return false;
	}
	host->m_socket->SetBlocking(false);

	m_myOwnConnection = new LoopBackConnection();
	m_myOwnConnection->m_address = host->m_address; //GetMyAddress(host->m_address.port);

	SetState(SESSION_CONNECTING);
	return true;
}

// Leaves the current session
void TCPSession::Leave()
{
	DebuggerPrintlnf("Leaving Host");

	DestroyConnection(m_myOwnConnection);
	DestroyConnection(m_hostConnection);

	for (unsigned int i = 0; i < m_connections.size(); ++i) {
		DestroyConnection(m_connections[i]);
	}

	StopListening();

	SetState(SESSION_DISCONNECTED);
}

//
void TCPSession::Update()
{
	ListenForNewConnections();

	RetrieveMessagesFromConnections();

	DestroyDisconnectedConnections();

	LeaveHostIfNull();
}

void TCPSession::ListenForNewConnections()
{
	// Listens for new connections
	if (IsListening()) {
		TCPSocket* socket = m_listenSocket->Accept();
		if (socket != nullptr) {
			TCPConnection* newConnection = new TCPConnection();
			//newConnection->m_socket = socket;
			
			uint8_t connectionIndex = GetFreeConnectionIndex();
			if (connectionIndex == INVALID_CONNECTION_INDEX) {
				delete newConnection;
			}
			else {
				JoinConnection(connectionIndex, newConnection);
				newConnection->m_socket = socket;
				newConnection->m_address = socket->m_netAddress;
				// more to do; 
				SendJoinInfo(newConnection);
			}
		}
	}
}

void TCPSession::RetrieveMessagesFromConnections()
{
	// Receive messages from other connections
	unsigned int connectionsSize = m_connections.size();

	for (int i = 0; i < (int)connectionsSize; ++i) {
		if (m_connections[i]) {
			NetMessage* message = nullptr;
			while (m_connections[i]->Receive(&message)) {
				if (message != nullptr) {
					NetMessageDefinition* nmd = m_messageDefinition[message->m_messageTypeIndex];
					nmd->m_handler(message);
					SAFE_DELETE(message);
				}
			}
		}
	}
}

void TCPSession::DestroyDisconnectedConnections()
{
	// Disconnects disconnected connections
	for (int i = 0; i < (int)m_connections.size(); ++i) {
		NetConnection* cp = m_connections[i];
		if ((cp != nullptr) && (cp != m_myOwnConnection)) {
			TCPConnection* tcpConnection = (TCPConnection*)cp;
			if (tcpConnection->IsDisconnected()) {
				DestroyConnection(tcpConnection);
			}
		}
	}
}

void TCPSession::LeaveHostIfNull()
{
	// If not host exists, leave connection
	if (m_hostConnection == nullptr) {
		Leave();
	}
}

void TCPSession::SendJoinInfo(NetConnection* cp)
{
	NetMessage msg = NetMessage(NETMSG_JOIN_RESPONSE);

	msg.Write(cp->m_connectionIndex);
	cp->Send(&msg);
}

// Uses NETMSG_JOIN_RESPONSE
void TCPSession::OnJoinResponse(NetMessage* msg)
{
	if (msg->m_payloadBytesUsed == 0) {
		return;
	}

	uint8_t myConnectionIndex = msg->Read<uint8_t>();
	JoinConnection(myConnectionIndex, m_myOwnConnection);

	SetState(SESSION_CONNECTED);
}

bool TCPSession::StartListening()
{
	if (!IsHost()) {
		return false;
	}

	if (IsListening()) {
		return true;
	}

	m_listenSocket = new TCPSocket();
	if (m_listenSocket->Listen(m_myOwnConnection->m_address)) {
		m_listenSocket->SetBlocking(false);
		DebuggerPrintlnf("Hosting: %s", NetAddressToString(m_listenSocket->m_netAddress).c_str());
		return true;
	}
	else {
		delete m_listenSocket;
		m_listenSocket = nullptr;
		return false;
	}
}

void TCPSession::StopListening()
{
	if (IsListening()) {
		delete m_listenSocket;
		m_listenSocket = nullptr;
	}
}

bool TCPSession::IsListening() const
{
	return (nullptr != m_listenSocket);
}

void TCPSession::SetState(eSessionState state)
{
	m_state = state;
}
