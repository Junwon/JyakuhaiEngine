#include "Engine/Network/NetAddress.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

bool NetAddressFromSocketAddress(NetAddress_T* out, sockaddr* socketAddress)
{
	if (socketAddress == nullptr) {
		return false;
	}

	// #TODO : Check for IPv6
	if (socketAddress->sa_family != AF_INET) {
		return false;
	}

	sockaddr_in* ipv4Address = (sockaddr_in*)socketAddress;

	out->port = ntohs(ipv4Address->sin_port);
	out->address = ntohl(ipv4Address->sin_addr.S_un.S_addr);

	return true;
}

void SocketAddressFromNetAddress(sockaddr* out, int* outSize, const NetAddress_T& addr)
{
	sockaddr_in* outAddress = (sockaddr_in*)out;
	memset(outAddress, 0, sizeof(sockaddr_in));
	outAddress->sin_family = AF_INET;
	outAddress->sin_port = htons(addr.port);
	outAddress->sin_addr.S_un.S_addr = htonl(addr.address);

	*outSize = sizeof(sockaddr_in);
}

std::vector<NetAddress_T> GetAddressesFromHostName(const char* hostname, uint16_t port, bool bindable)
{
	// Convert to String
	char service[16];
	sprintf_s(service, "%u", port);

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;	// Filter for only IPv4
	hints.ai_socktype = SOCK_STREAM; // SOCK_DGRAM = UDP, SOCK_STREAM = TCP
	hints.ai_flags |= (bindable ? AI_PASSIVE : 0);
	//hints.ai_flags |= AI_NUMERICHOST;

	addrinfo* results = nullptr;
	int status = ::getaddrinfo(hostname, service, &hints, &results);

	std::vector<NetAddress_T> addresses;
	if (status != 0) {
		return addresses;
	}

	addrinfo* addr = results;
	while (addr != nullptr) {

		NetAddress_T netAddr;
		if (NetAddressFromSocketAddress(&netAddr, addr->ai_addr)) {
			addresses.push_back(netAddr);
		}

		addr = addr->ai_next;
	}

	::freeaddrinfo(results);
	return addresses;
}

std::string NetAddressToString(NetAddress_T& addr)
{
	unsigned char* arr = (unsigned char*)&addr.address;

	unsigned char a = arr[0];
	unsigned char b = arr[1];
	unsigned char c = arr[2];
	unsigned char d = arr[3];

	std::string stringAddr = std::to_string(d) + "." + std::to_string(c) + "." + std::to_string(b) + "." + std::to_string(a) + ":" + std::to_string(addr.port);

	return stringAddr;
}

NetAddress_T StringToNetAddress(const char* addrString)
{
	NetAddress_T result;
	memset(&result, 0, sizeof(NetAddress_T));

	std::vector<std::string> addrBuffer;
	SplitIntoBuffer(addrBuffer, addrString, ":");

	if (addrBuffer.size() > 0) {
		std::vector<NetAddress_T> addresses = GetAddressesFromHostName(addrBuffer[0].c_str(), (uint16_t)stoi(addrBuffer[1]), true);

		result.address = addresses[0].address;
		result.port = addresses[0].port;
	}

	return result;
}

NetAddress_T GetMyAddress(uint16_t port)
{
	std::vector<NetAddress_T> addresses = GetAddressesFromHostName("", port, true);

	ASSERT_OR_DIE(!addresses.empty(), "Retrieved no address. Don't forget to call NetSystemStartup().");

	return addresses[0];
}

