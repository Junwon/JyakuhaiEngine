#pragma once

#include <stdint.h>

#include "Engine/Network/NetAddress.hpp"

const int MAX_QUEUED = 8; // The amount of clients in queue.

class TCPSocket
{
public:
	TCPSocket() :
		m_socket(INVALID_SOCKET),
		m_isListeningSocket(false)
	{}
	~TCPSocket();

public:
	// CLIENT
	bool Join(const char* addr);
	bool Join(NetAddress_T addr);
	
	void Close();
	bool IsValid() const;

	// HOST
	bool Host(uint16_t port);
	bool Listen(uint16_t port);
	bool Listen(const NetAddress_T& addr);
	bool IsListening() const;
	TCPSocket* Accept();

	// CLIENT & HOST
	unsigned int Send(const void* payload, unsigned int payloadSize);
	unsigned int Receive(void* payload, unsigned int maxPayloadSize);

	void CheckForDisconnect();
	void SetBlocking(bool blocking);
	void EnableNagle(bool enabled);
public:
	SOCKET m_socket;
	NetAddress_T m_netAddress;
	bool m_isListeningSocket;
};