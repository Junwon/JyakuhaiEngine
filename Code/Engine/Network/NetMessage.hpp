#pragma once

#include "Engine/Core/BinaryStream.hpp"
#include "Engine/Network/NetConnection.hpp"

enum eCoreNetMessage : uint8_t
{
	NETMSG_JOIN_RESPONSE = 0,
	NETMSG_PING,
	NETMSG_PONG,
	NETMSG_DESTROY_OBJECT,
	NETMSG_UPDATE_OBJECT,
	NETMSG_CREATE_OBJECT,
	NETMSG_CORE_COUNT = 32
};

class NetMessage : public BinaryStream
{
public:
	NetMessage();
	NetMessage(uint8_t typeIndex);

	virtual unsigned int ReadBytes(void* outBuffer, const unsigned int count) override;

	virtual unsigned int WriteBytes(const void* buffer, const unsigned int count) override;

	std::string ReadString();

	void WriteString(const std::string& string);

	unsigned int GetRemainingBytes() const;

	bool IsEmpty() const;
public:

	uint8_t m_messageTypeIndex;
	NetConnection* m_sender;

	byte_t m_payload[1024];
	unsigned int m_payloadBytesUsed;
	unsigned int m_payloadByteRead;
};