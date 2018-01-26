#pragma once

#include <vector>

#include "Engine/Network/NetAddress.hpp"
#include "Engine/Network/NetMessageDefinition.hpp"

enum eSessionState
{
	SESSION_CONNECTED,
	SESSION_CONNECTING,
	SESSION_DISCONNECTED
};

constexpr uint8_t INVALID_CONNECTION_INDEX = 0xff;
constexpr unsigned int DEFAULT_MAX_CONNECTION = 10;

class NetConnection;

class NetSession
{
public:
	NetSession();
	virtual ~NetSession() {};

public:
	virtual bool Host(uint16_t port) = 0;
	virtual bool Join(const NetAddress_T& addr) = 0; // production version: session_join_info_t instead
	virtual void Leave() = 0;

	virtual void Update() = 0;

public:
	bool RegisterMessageDefinition(NetMessageDefinition& defn);
	bool RegisterMessageDefinition(uint8_t msgID, NetMessageDefinition& defn);
	// bool register_message_function( uint8_t msg_id, std::function<void(NetMessage*)> const &callback );

	NetMessageDefinition* GetMessageDefinition(uint8_t id) const;


	inline bool IsHost() const {
		return (m_myOwnConnection == m_hostConnection) && (m_hostConnection != nullptr);
	}

	inline bool IsClient() const {
		return (m_myOwnConnection != m_hostConnection) && (m_myOwnConnection != nullptr);
	}

	inline bool IsRunning() const {
		return (m_state != SESSION_DISCONNECTED);
	}

	inline bool IsReady() const {
		return (m_state == SESSION_CONNECTED);
	}

	uint8_t GetFreeConnectionIndex() const;
	void JoinConnection(uint8_t index, NetConnection* connection);
	void DestroyConnection(NetConnection* cp);
	NetConnection* GetConnection(uint8_t index);
	void SendMessageToOthers(NetMessage& msg);
	uint8_t GetMyConnectionIndex() const;
	void SendToHost(NetMessage* msg);

public:
	eSessionState m_state;

	std::vector<NetConnection*> m_connections;
	unsigned int m_maxConnectionCount;

	NetConnection* m_myOwnConnection;
	NetConnection* m_hostConnection;

	std::vector<NetMessageDefinition*> m_messageDefinition;

	unsigned int GetNumConnections();
};