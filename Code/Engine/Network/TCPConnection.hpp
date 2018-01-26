#pragma once
#include <array>

#include "Engine/Network/NetConnection.hpp"

class TCPSocket;

class TCPConnection : public NetConnection
{
public:
	TCPConnection();
	~TCPConnection();

public:
	virtual void Send(NetMessage* msg) override;
	virtual bool Receive(NetMessage** msg) override;

	bool Connect();

	bool IsDisconnected();
public:
	TCPSocket* m_socket;
	std::array<unsigned char, 1027> m_buffer;
	unsigned int m_bufferCount;
};