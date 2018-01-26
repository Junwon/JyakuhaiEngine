#pragma once
#include <stdint.h>
#include <vector>
#include <functional>

class NetMessage;

typedef std::function<void(NetMessage*)> NetMessageCallback;

class NetMessageDefinition
{
public:
	uint8_t m_typeIndex;

	NetMessageCallback m_handler;
};