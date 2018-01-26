#include "Engine/Network/LoopBackConnection.hpp"

LoopBackConnection::~LoopBackConnection()
{

}

void LoopBackConnection::Send(NetMessage* msg)
{
	m_messageQueue.push(msg);
}

bool LoopBackConnection::Receive(NetMessage** msg)
{
	if (!m_messageQueue.empty()) {
		*msg = m_messageQueue.front();
 		m_messageQueue.pop();
		return true;
	}

	return false;
}
