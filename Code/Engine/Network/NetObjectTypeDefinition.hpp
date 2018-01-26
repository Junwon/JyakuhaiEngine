#pragma once

#include <stdint.h>

class NetMessage;
class NetObject;
class NetSession;

typedef void(*AppendCreateInfoCallback)(NetMessage*, void*);
typedef void*(*ProcessCreateInfoCallback)(NetMessage*, NetObject*);
typedef void(*AppendDestroyInfoCallback)(NetMessage*, void*);
typedef void(*ProcessDestroyInfoCallback)(NetMessage*, void*);

// Create snapshot
typedef void*(*CreateSnapshotCallback)();

// Delete snapshot
typedef void(*DestroySnapshotCallback)(void*);

// Get snapshot
typedef void(*GetCurrentSnapshotCallback)(void*, void*);

// Send snapshot
typedef void(*AppendSnapshotCallback)(NetMessage*, void*, void**);

// Receive snapshot
typedef void(*ProcessSnapshotCallback)(NetMessage*, void*, void**);

// Apply snapshot
typedef void(*ApplySnapshotCallback)(void*, void*, double);

class NetObjectTypeDefinition
{
public:
	AppendCreateInfoCallback m_appendCreateInfo;
	ProcessCreateInfoCallback m_processCreateInfo;
	AppendDestroyInfoCallback m_appendDestroyInfo;
	ProcessDestroyInfoCallback m_processDestroyInfo;

	CreateSnapshotCallback m_createSnapshot;
	DestroySnapshotCallback m_destroySnapshot;
	ApplySnapshotCallback m_applySnapshot;
	GetCurrentSnapshotCallback m_getCurrentSnapshot;
	AppendSnapshotCallback m_appendSnapshot;
	ProcessSnapshotCallback m_processSnapshot;
};

void RegisterInitialNetObjectMessageDefinition(NetSession* session);

NetObject* NetObjectReplicate(void* objectPtr, uint8_t typeID);
void OnReceiveNetObjectCreate(NetMessage* msg);

void NetObjectStopRelication(uint16_t netID);
void OnReceiveNetObjectDestroy(NetMessage* msg);

