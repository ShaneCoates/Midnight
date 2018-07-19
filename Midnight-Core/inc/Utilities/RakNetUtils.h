#ifndef _RACKNET_UTILS_H_
#define _RACKNET_UTILS_H_

#include "raknet/RakNetTypes.h"
class RakNetUtils
{
public:
	static const char* ReadStringFromPacket(const RakNet::Packet* _packet);
};

#endif
