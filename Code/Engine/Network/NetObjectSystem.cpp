#include "Engine/Network/NetObjectSystem.hpp"

#include "Engine/Core/Time.hpp"

#include "Engine/Network/NetObjectSystem.hpp"
#include "Engine/Network/NetObjectTypeDefinition.hpp"
#include "Engine/Network/NetSession.hpp"
#include "Engine/Network/NetObject.hpp"
#include "Engine/Network/NetMessage.hpp"
#include "Engine/Core/Interval.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"


static NetSession* s_netObjectSession = nullptr;

static std::vector<NetObject*> m_allNetObjects(LAST_SENT_SNAPSHOT_BUFFER_SIZE, nullptr);
static const unsigned int NETOBJ_TYPE_DEF_SIZE = 100;
static std::vector<NetObjectTypeDefinition*> m_netObjectTypeDefinition(NETOBJ_TYPE_DEF_SIZE, nullptr);

static uint16_t lastGivenID = 0;

static Interval m_interval;

static unsigned int m_netObjectCount = 0;

static double m_hostTime = 0;
static double m_clientTime = 0;

void ClearNetObjectsArray()
{
	for (int i = 0; i < (int)m_allNetObjects.size(); ++i) {
		delete m_allNetObjects[i];
		m_allNetObjects[i] = nullptr;
	}
}

void InitializeNetworkSystem(NetSession* session, float freq)
{
	ClearNetObjectsArray();
	SetNetObjectSystemSession(session);
	RegisterInitialNetObjectMessageDefinition(session);
	SetIntervalFrequency(freq);
}

void SetIntervalFrequency(float hz)
{
	m_interval.SetFrequency(hz);
}

void NetObjectSystemStep()
{
	if (m_interval.CheckAndReset()) {
		if (s_netObjectSession->IsHost()) {
			SendNetObjectUpdates();
		}
	}

	if (s_netObjectSession->IsClient()) {
		for (NetObject* nop : m_allNetObjects) {
			if (nop && nop->m_lastReceivedSnapshotForClient && nop->m_definition->m_applySnapshot != nullptr) {
				nop->m_definition->m_applySnapshot(nop->m_lastReceivedSnapshotForClient, nop->m_localObject, GetCurrentTimeSeconds() - m_clientTime);
			}
		}
	}
}

void SendNetObjectUpdates()
{
	for (NetObject* nop : m_allNetObjects) {
		// current snapshot only needs to be allocated once and can be over-written
		if (nop && nop->m_definition->m_getCurrentSnapshot != nullptr) {
			if(nop->m_currentSnapshot == nullptr && nop->m_definition->m_createSnapshot != nullptr) {
				nop->m_currentSnapshot = nop->m_definition->m_createSnapshot();
			}
			nop->m_definition->m_getCurrentSnapshot(nop->m_currentSnapshot, nop->m_localObject);
		}
	}

	for (NetConnection* cp : s_netObjectSession->m_connections) {
		if (cp && cp != s_netObjectSession->m_myOwnConnection) {
			SendNetObjectUpdateTo(cp);
		}
	}
}

void SendNetObjectUpdateTo(NetConnection *cp)
{
	for (NetObject* nop : m_allNetObjects) {
		if (nop) {
			if (nop->m_lastSentSnapshotForHost[cp->m_connectionIndex] != nop->m_currentSnapshot && nop->m_definition->m_appendSnapshot != nullptr) {
				NetMessage updateMsg = NetMessage(NETMSG_UPDATE_OBJECT);
				updateMsg.Write(nop->m_netID);
				nop->m_definition->m_appendSnapshot(&updateMsg, nop->m_currentSnapshot, &nop->m_lastSentSnapshotForHost[cp->m_connectionIndex]);
				if (updateMsg.m_payloadBytesUsed > sizeof(uint16_t)) {
					double hostRefTimeSent = GetCurrentTimeSeconds();
					updateMsg.Write(hostRefTimeSent);
					cp->Send(&updateMsg);
				}
			}
		}
	}
}

void OnNetObjectUpdateRecieved(NetMessage* updateMsg)
{
	double currentClientTime = GetCurrentTimeSeconds();

	// Reads Net ID
	uint16_t netID = updateMsg->Read<uint16_t>();

	NetObject* nop = NetObjectFind(netID);
	if (nullptr == nop) {
		return;
	}

	// like current snapshot - we can just overwrite here, so only allocate this memory once.
	if (nop->m_currentSnapshot == nullptr && nop->m_definition->m_createSnapshot != nullptr) {
		nop->m_currentSnapshot = nop->m_definition->m_createSnapshot();
	}

	if (nop->m_definition->m_processSnapshot != nullptr && nop->m_definition->m_applySnapshot != nullptr) {
		nop->m_definition->m_processSnapshot(updateMsg, nop->m_currentSnapshot, &nop->m_lastReceivedSnapshotForClient);

		// Reads in host time
		double recvdHostTime = updateMsg->Read<double>();

		// Calculate relative time for client
		double hostTimeDifference = recvdHostTime - m_hostTime;
		m_clientTime = hostTimeDifference + m_clientTime;
		m_hostTime = recvdHostTime;
		double deltaTime = currentClientTime - m_clientTime;
		nop->m_definition->m_applySnapshot(nop->m_lastReceivedSnapshotForClient, nop->m_localObject, deltaTime);
	}
}

unsigned int GetNetObjectCount()
{
	return m_netObjectCount;
}

void SetNetObjectSystemSession(NetSession* session)
{
	s_netObjectSession = session;
}

bool RegisterNetObjectMessageDefinition(uint8_t msgID, NetObjectTypeDefinition& defn)
{
	if (m_netObjectTypeDefinition[msgID] != nullptr) {
		ASSERT_OR_DIE(false, "Double registering net object message definition.");
		return false;
	}

	m_netObjectTypeDefinition[msgID] = &defn;
	return true;
}

uint16_t NetObjectGetUnusedID()
{
	for (int i = lastGivenID; i < (int)m_allNetObjects.size(); ++i) {
		if (m_allNetObjects[i] == nullptr) {
			lastGivenID = (uint16_t)i;
			return lastGivenID;
		}
	}

	for (int i = 0; i < (int)lastGivenID; ++i) {
		if (m_allNetObjects[i] == nullptr) {
			lastGivenID = (uint16_t)i;
			return lastGivenID;
		}
	}

	m_allNetObjects.resize(m_allNetObjects.size() + 1);
	lastGivenID = (uint16_t)m_allNetObjects.size() - 1;
	return lastGivenID;
}

NetObjectTypeDefinition* NetObjectFindDefinition(uint8_t typeID)
{
	return m_netObjectTypeDefinition[typeID];
}

void NetObjectRegister(NetObject* netObjPointer)
{
	uint16_t index = netObjPointer->m_netID;

	// Inserts into system's array
	
	ASSERT_OR_DIE(m_allNetObjects[index] == nullptr, Stringf("Double registering NetObject pointer at index %d.", index));
	m_allNetObjects[index] = netObjPointer;

	m_netObjectCount++;
}

NetSession* NetObjectGetSession()
{
	return s_netObjectSession;
}

NetObject* NetObjectFind(uint16_t netID)
{
	return netID >= m_allNetObjects.size() ? nullptr : m_allNetObjects[netID];
}

void NetObjectUnregister(NetObject* netObjPointer)
{
	uint16_t index = netObjPointer->m_netID;

	if (index >= m_allNetObjects.size()) {
		return;
	}

	if (netObjPointer->m_currentSnapshot) {
		delete netObjPointer->m_currentSnapshot;
		netObjPointer->m_currentSnapshot = nullptr;
	}

	if (NetObjectGetSession()->IsHost()) {
		for (int i = 0; i < (int)netObjPointer->m_lastSentSnapshotForHost.size(); ++i) {
			if (netObjPointer->m_lastSentSnapshotForHost[i]) {
				delete netObjPointer->m_lastSentSnapshotForHost[i];
				netObjPointer->m_lastSentSnapshotForHost[i] = nullptr;
			}
		}
	}
	else {
		if (netObjPointer->m_lastReceivedSnapshotForClient) {
			delete netObjPointer->m_lastReceivedSnapshotForClient;
			netObjPointer->m_lastReceivedSnapshotForClient = nullptr;
		}
	}

	m_allNetObjects[index] = nullptr;
	m_netObjectCount--;
}

void SetHostTime(double time)
{
	m_hostTime = time;
}

void SetClientTime(double time)
{
	m_clientTime = time;
}

double GetHostTime()
{
	return m_hostTime;
}

double GetClientTime()
{
	return m_clientTime;
}

std::vector<NetObject*> GetVectorOfNetObjects()
{
	return m_allNetObjects;
}