#include "Engine/Network/TCPConnection.hpp"

#include "Engine/Network/TCPSocket.hpp"

TCPConnection::TCPConnection() :
	m_socket(nullptr),
	m_bufferCount(0)
{
	m_buffer = { 0 };
}

TCPConnection::~TCPConnection()
{
}

void TCPConnection::Send(NetMessage* msg)
{
	unsigned short msgLengthAndIndex = (unsigned short)(msg->m_payloadBytesUsed + 1);
	msg->m_sender = this;
	m_socket->Send(&msgLengthAndIndex, sizeof(msgLengthAndIndex)); // Sends message length
	m_socket->Send(&msg->m_messageTypeIndex, sizeof(msg->m_messageTypeIndex)); // Sends message type index
	m_socket->Send(msg->m_payload, msg->m_payloadBytesUsed); // Sends payload
}

bool TCPConnection::Receive(NetMessage** msg)
{
	if (m_bufferCount < 2) {
		unsigned int receiveSize = m_socket->Receive(m_buffer.data() + m_bufferCount, 2 - m_bufferCount);

		if (receiveSize == 0) {
			return false;
		}
		else {
			m_bufferCount += receiveSize;
			return true;
		}
	}
	else {
		uint16_t bufferBytesUsed = *(uint16_t*)m_buffer.data();

		// 2u = 2 unsigned char
		if (m_bufferCount < (2u + bufferBytesUsed)) {
			unsigned int receiveSize = m_socket->Receive(m_buffer.data() + m_bufferCount, (2u + bufferBytesUsed) - m_bufferCount);
			if (receiveSize == 0) {
				return false;
			}
			m_bufferCount += receiveSize;
			return true;
		}
		else {
			*msg = new NetMessage(m_buffer[2]);
			std::vector<unsigned char> buffer;
			buffer.resize(bufferBytesUsed - 1);
			std::memcpy(buffer.data(), m_buffer.data() + 3, bufferBytesUsed - 1);
			(*msg)->WriteBytes(buffer.data(), buffer.size());
			(*msg)->m_sender = this;
			m_bufferCount = 0;
			return true;
		}
	}

	return false;
}

bool TCPConnection::Connect()
{
	return m_socket->Join(m_address);
}

bool TCPConnection::IsDisconnected()
{
	if (!m_socket->IsValid()) {
		return true;
	}

	return false;
}
