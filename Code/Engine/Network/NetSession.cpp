#include "Engine/Network/NetSession.hpp"

#include "Engine/Network/NetConnection.hpp"

#include <limits>
#include <type_traits>

#include "Engine/Core/ErrorWarningAssert.hpp"

NetSession::NetSession() :
	m_hostConnection(nullptr),
	m_myOwnConnection(nullptr),
	m_state(SESSION_DISCONNECTED),
	m_maxConnectionCount(DEFAULT_MAX_CONNECTION),
	m_messageDefinition(100, nullptr)
{};

bool NetSession::RegisterMessageDefinition(NetMessageDefinition& defn)
{
	uint8_t msgID = defn.m_typeIndex;
	if (m_messageDefinition[msgID] != nullptr) {
		ASSERT_OR_DIE(false, "Double registering message definition.")
		return false;
	}

	m_messageDefinition[msgID] = &defn;
	return true;
}

bool NetSession::RegisterMessageDefinition(uint8_t msgID, NetMessageDefinition& defn)
{
	if (m_messageDefinition[msgID] != nullptr) {
		ASSERT_OR_DIE(false, "Double registering message definition.")
		return false;
	}

	m_messageDefinition[msgID] = &defn;
	return true;
}

NetMessageDefinition* NetSession::GetMessageDefinition(uint8_t id) const
{
	if (id > m_messageDefinition.size()) {
		return m_messageDefinition[id];
	}

	return nullptr;
}

uint8_t NetSession::GetFreeConnectionIndex() const
{
	unsigned int index = 0;
	for (index = 0; index < m_connections.size(); ++index) {
		if (m_connections[index] == nullptr) {
			return (uint8_t)index;
		}
	}

	if (index < m_maxConnectionCount) {
		return (uint8_t)index;
	}
	else {
		return INVALID_CONNECTION_INDEX;
	}
}

void NetSession::JoinConnection(uint8_t index, NetConnection* connection)
{
	connection->m_connectionIndex = index;

	ASSERT_OR_DIE((index >= m_connections.size()) || (m_connections[index] == nullptr), "Connection Error");

	if (index >= m_connections.size()) {
		m_connections.resize(index + 1);
	}

	m_connections[index] = connection;
}

void NetSession::DestroyConnection(NetConnection* cp)
{
	if (nullptr == cp) {
		return;
	}

	if (m_myOwnConnection == cp) {
		m_myOwnConnection = nullptr;
	}

	if (m_hostConnection == cp) {
		m_hostConnection = nullptr;
	}

	if (cp->m_connectionIndex != INVALID_CONNECTION_INDEX) {
		m_connections[cp->m_connectionIndex] = nullptr;
		cp->m_connectionIndex = INVALID_CONNECTION_INDEX;
	}


	delete cp;
}

NetConnection* NetSession::GetConnection(uint8_t index)
{
	if (index < m_connections.size()) {
		return m_connections[index];
	}

	return nullptr;
}

void NetSession::SendMessageToOthers(NetMessage& msg)
{
	for (NetConnection* cp : m_connections) {
		if (cp != m_myOwnConnection && cp != nullptr) {
			cp->Send(&msg);
		}
	}
}

uint8_t NetSession::GetMyConnectionIndex() const
{
	if (m_myOwnConnection) {
		return m_myOwnConnection->m_connectionIndex;
	}

	return 0xf;
}

void NetSession::SendToHost(NetMessage* msg)
{
	m_hostConnection->Send(msg);
}

unsigned int NetSession::GetNumConnections()
{
	unsigned int count = 0;

	for (int i = 0; (unsigned int)i < m_connections.size(); ++i) {
		if (m_connections[i] != nullptr) {
			count++;
		}
	}
	return count;
}
