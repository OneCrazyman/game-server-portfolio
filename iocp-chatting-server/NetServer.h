#include "LanServer.h"

class NetServer : public LanServer
{
public:
	NetServer(NetworkEvents* networkEvents, DWORD numberOfConcurrentThreads, uint32_t numberOfWorkerThreads) : 
		LanServer(networkEvents, numberOfConcurrentThreads, numberOfWorkerThreads) {}
};