#include "Utilities/RakNetUtils.h"
#include "raknet/BitStream.h"

const char* RakNetUtils::ReadStringFromPacket(const RakNet::Packet* _packet)
{
	RakNet::BitStream* bitStream = new RakNet::BitStream(_packet->data, sizeof(_packet->data), true);
	RakNet::RakString rs;
	RakNet::BitStream bsIn(_packet->data, _packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	bsIn.Read(rs);
	return rs.C_String();
}

