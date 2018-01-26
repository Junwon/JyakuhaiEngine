#pragma once

#include "Engine/Network/NetworkCommon.hpp"

#include <vector>
#include <stdint.h>

#include "Engine/Core/StringUtils.hpp"

struct NetAddress_T
{
	// Host Name

	// Port
	unsigned int address;
	uint16_t port;
};

bool NetAddressFromSocketAddress(NetAddress_T* out, sockaddr* socketAddress);

void SocketAddressFromNetAddress(sockaddr* out, int* outSize, const NetAddress_T& addr);

std::vector<NetAddress_T> GetAddressesFromHostName(const char* hostname, uint16_t port, bool bindable);

std::string NetAddressToString(NetAddress_T& addr);

NetAddress_T StringToNetAddress(const char* addrString);

NetAddress_T GetMyAddress(uint16_t port);