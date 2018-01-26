#pragma once

#include "Engine/Network/NetAddress.hpp"
#include "Engine/Network/NetSession.hpp"
#include "Engine/Network/NetMessage.hpp"

class NetConnection
{
public:
	NetConnection() :
		m_owner(nullptr)
	{};
	virtual ~NetConnection() {};

	virtual void Send(NetMessage* msg) = 0;
	virtual bool Receive(NetMessage** msg) = 0;

public:
	NetSession* m_owner;
	NetAddress_T m_address;
	uint8_t m_connectionIndex; // LUID 
};