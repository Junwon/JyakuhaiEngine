#include "Engine/Network/NetMessage.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

NetMessage::NetMessage() :
	m_messageTypeIndex(0),
	m_sender(nullptr),
	m_payloadBytesUsed(0),
	m_payloadByteRead(0)
{
}

NetMessage::NetMessage(uint8_t typeIndex) :
	m_messageTypeIndex(typeIndex),
	m_sender(nullptr),
	m_payloadBytesUsed(0),
	m_payloadByteRead(0)
{
}

unsigned int NetMessage::ReadBytes(void* outBuffer, const unsigned int count)
{
	byte_t* arr = (byte_t*)outBuffer;

	for (int index = 0; index < (int)count; ++index) {
		arr[index] = m_payload[m_payloadByteRead];
		m_payloadByteRead++;

		ASSERT_OR_DIE(m_payloadByteRead <= m_payloadBytesUsed, "Byte read index is greater than byte used.");
	}

	return count;
}

unsigned int NetMessage::WriteBytes(const void* buffer, const unsigned int count)
{
	const byte_t* arr = (byte_t*)buffer;
	
	for (unsigned int index = 0; index < count; ++index) {
		m_payload[m_payloadBytesUsed] = arr[index];
		m_payloadBytesUsed++;
	}


	return count;
}

std::string NetMessage::ReadString()
{
	uint16_t messageSize = Read<uint16_t>();

	std::string string("");
	string.resize(messageSize);
	std::memcpy((void*)string.data(), (void*)(m_payload + m_payloadByteRead), messageSize);
	m_payloadByteRead += messageSize;

	return string;
}

void NetMessage::WriteString(const std::string& string)
{
	uint16_t messageSize = (uint16_t)string.size();
	if (messageSize > 0xffff) {
		messageSize = 0xfffe;
	}

	//*(uint16_t*)m_payload = messageSize; //bit math black magic
	Write(messageSize);

	std::memcpy((void*)(m_payload + m_payloadBytesUsed), string.data(), messageSize);
	m_payloadBytesUsed += messageSize; //offset for message length
}

unsigned int NetMessage::GetRemainingBytes() const
{
	return m_payloadBytesUsed - m_payloadByteRead;
}

bool NetMessage::IsEmpty() const
{
	return m_payloadBytesUsed == 0;
}
