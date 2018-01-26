#include "Engine/Network/Net.hpp"

bool NetSystemStartup()
{
	//Pick the WinSock Version
	WORD version = MAKEWORD(2, 2);

	WSADATA data;
	int error = ::WSAStartup(version, &data);
	return (error == 0);
}

void NetSystemShutdown()
{
	::WSACleanup();
}