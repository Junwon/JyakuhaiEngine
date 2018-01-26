#pragma once
#include <vector>
#include <stdint.h>

class NetObjectTypeDefinition;

const uint8_t INVALID_TYPE_ID = 0xff;
const uint16_t INVALID_NETWORK_ID = 0xffff;
const unsigned int LAST_SENT_SNAPSHOT_BUFFER_SIZE = 400;

class NetObject
{
public:
	NetObject(NetObjectTypeDefinition* defn);
	~NetObject();

public:
	uint8_t m_typeID;
	uint16_t m_netID;
	NetObjectTypeDefinition* m_definition;

	void* m_localObject;
	void* m_currentSnapshot; // for both client and host
	std::vector<void*> m_lastSentSnapshotForHost; // [array] host only
	void* m_lastReceivedSnapshotForClient; // client only
};