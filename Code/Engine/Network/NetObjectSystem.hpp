#pragma once

#include <stdint.h>
#include <vector>

class NetObjectTypeDefinition;
class NetSession;
class NetObject;
class NetConnection;
class NetMessage;

void InitializeNetworkSystem(NetSession* session, float freq = 60.f);

void SetIntervalFrequency(float hz);

void NetObjectSystemStep();
void SendNetObjectUpdates();
void SendNetObjectUpdateTo(NetConnection *cp);
void OnNetObjectUpdateRecieved(NetMessage* updateMsg);
unsigned int GetNetObjectCount();
void SetNetObjectSystemSession(NetSession* session);
bool RegisterNetObjectMessageDefinition(uint8_t msgID, NetObjectTypeDefinition& defn);
uint16_t NetObjectGetUnusedID();
NetObjectTypeDefinition* NetObjectFindDefinition(uint8_t typeID);
void NetObjectRegister(NetObject* netObjPointer);
NetSession* NetObjectGetSession();
NetObject* NetObjectFind(uint16_t netID);
void NetObjectUnregister(NetObject* netObjPointer);
void SetHostTime(double time);
void SetClientTime(double time);
double GetHostTime();
double GetClientTime();
std::vector<NetObject*> GetVectorOfNetObjects();