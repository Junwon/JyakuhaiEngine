#pragma once

#include "Engine/Network/NetSession.hpp"

class TCPSocket;
class LoopBackConnection;

class TCPSession : public NetSession
{
public:
	TCPSession();
	virtual ~TCPSession();

public:
	// create a loopback - add it to connections, set my and host connection to it;
	virtual bool Host(uint16_t port) override;

	virtual bool Join(const NetAddress_T& addr) override; // production version: session_join_info_t instead
														   // if I have a connection - destroy it.
	virtual void Leave() override;

	// Process all connections; 
	virtual void Update() override;

	void ListenForNewConnections();

	void RetrieveMessagesFromConnections();

	void DestroyDisconnectedConnections();

	void LeaveHostIfNull();

	void SendJoinInfo(NetConnection* cp);
	void OnJoinResponse(NetMessage* msg);
	bool StartListening();
	void StopListening();
	bool IsListening() const;
	void SetState(eSessionState state);
public:
	TCPSocket* m_listenSocket;

	NetMessageDefinition* m_onJoinResponse;
};
