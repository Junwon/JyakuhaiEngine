#include "Engine/Network/NetObjectTypeDefinition.hpp"

#include "Engine/Core/Time.hpp"

#include "Engine/Network/NetObject.hpp"
#include "Engine/Network/NetMessage.hpp"
#include "Engine/Network/NetObjectSystem.hpp"
#include "Engine/Network/NetSession.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

void RegisterInitialNetObjectMessageDefinition(NetSession* session)
{
	NetMessageDefinition* onDestroyResponse = new NetMessageDefinition();
	onDestroyResponse->m_typeIndex = NETMSG_DESTROY_OBJECT;
	onDestroyResponse->m_handler = OnReceiveNetObjectDestroy;

	session->RegisterMessageDefinition(*onDestroyResponse);

	NetMessageDefinition* onUpdateResponse = new NetMessageDefinition();
	onUpdateResponse->m_typeIndex = NETMSG_UPDATE_OBJECT;
	onUpdateResponse->m_handler = OnNetObjectUpdateRecieved;

	session->RegisterMessageDefinition(*onUpdateResponse);

	NetMessageDefinition* onCreateResponse = new NetMessageDefinition();
	onCreateResponse->m_typeIndex = NETMSG_CREATE_OBJECT;
	onCreateResponse->m_handler = OnReceiveNetObjectCreate;

	session->RegisterMessageDefinition(*onCreateResponse);
}

// Host function
NetObject* NetObjectReplicate(void* objectPtr, uint8_t typeID)
{
	NetObjectTypeDefinition* defn = NetObjectFindDefinition(typeID);
	if (defn == nullptr) {
		return nullptr;
	}

	NetSession* sp = NetObjectGetSession();
	NetObject* nop = new NetObject(defn);

	nop->m_localObject = objectPtr;
	nop->m_typeID = typeID;
	nop->m_netID = NetObjectGetUnusedID();

	NetMessage createMsg = NetMessage(NETMSG_CREATE_OBJECT);
	createMsg.Write(typeID);
	createMsg.Write(nop->m_netID);

 	defn->m_appendCreateInfo(&createMsg, objectPtr);

	double hostTime = GetCurrentTimeSeconds();
	createMsg.Write(hostTime);
	sp->SendMessageToOthers(createMsg);

	NetObjectRegister(nop);

	return nop;
}

void OnReceiveNetObjectCreate(NetMessage* msg)
{
	// Reads in time when client first receives message
	SetClientTime(GetCurrentTimeSeconds());

	uint8_t typeID = msg->Read<uint8_t>();
	uint16_t netID = msg->Read<uint16_t>();

	NetObjectTypeDefinition* defn = NetObjectFindDefinition(typeID);
	ASSERT_OR_DIE(defn != nullptr, "Net Object Definition could not be found.");

	NetObject* nop = new NetObject(defn);
	nop->m_typeID = typeID;
	nop->m_netID = netID;

	void* localObject = defn->m_processCreateInfo(msg, nop);
	ASSERT_OR_DIE(localObject != nullptr, "Could not locate local Net Object.");

	NetObjectRegister(nop);

	// Reads in initial reference time from host
	double hostTime = msg->Read<double>();
	SetHostTime(hostTime);

	// Registers object with system
 	nop->m_localObject = localObject;
}

void NetObjectStopRelication(uint16_t netID)
{
	// remove from our system
	NetObject* nop = NetObjectFind(netID);
	if (nop == nullptr) {
		return;
	}

	// tell everyone 
	NetMessage msg = NetMessage(NETMSG_DESTROY_OBJECT);
	msg.Write(nop->m_netID);

	// usually does nothing - no-op.
	if (nop->m_definition->m_appendDestroyInfo) {
		nop->m_definition->m_appendDestroyInfo(&msg, nop->m_localObject);
	}
	
	NetSession* sp = NetObjectGetSession();
	sp->SendMessageToOthers(msg);

	NetObjectUnregister(nop);
	delete nop;
}

void OnReceiveNetObjectDestroy(NetMessage* msg)
{
	uint16_t netID = msg->Read<uint16_t>();
	NetObject* nop = NetObjectFind(netID);

	if (nop == nullptr) {
		return;
	}

	NetObjectUnregister(nop);

	// THIS is critical; Need to run callback to destroy process once unregistered.
	nop->m_definition->m_processDestroyInfo(msg, (NetObject*)(nop->m_localObject));

	delete nop;
}
