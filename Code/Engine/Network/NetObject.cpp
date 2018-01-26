#include "Engine/Network/NetObject.hpp"

#include "Engine/Core/Time.hpp"

NetObject::NetObject(NetObjectTypeDefinition* defn) :
	m_definition(defn),
	m_typeID(INVALID_TYPE_ID),
	m_netID(INVALID_NETWORK_ID),
	m_localObject(nullptr),
	m_currentSnapshot(nullptr),
	m_lastReceivedSnapshotForClient(nullptr)
{
	m_lastSentSnapshotForHost.resize(LAST_SENT_SNAPSHOT_BUFFER_SIZE, nullptr);
}

NetObject::~NetObject()
{
}