#ifndef _SERVER_MANAGER_H_
#define _SERVER_MANAGER_H_

#include "raknet/RakPeerInterface.h"
#include <map>
struct ImGuiLogger;

struct PlayerInfo
{
	unsigned int playerID;
	char* playerName;
};

class ServerManager
{
public:
	ServerManager();
	~ServerManager();

	void Init(int _port);

	void Update();
	void Draw();


protected:
private:

	void ReadPackets();
	void AddPlayer(const RakNet::Packet* _packet);
	void DisconnectPlayer(const RakNet::SystemAddress _address);
	

	const bool PlayerListContainsAddress(const RakNet::SystemAddress _address, std::map<RakNet::SystemAddress, PlayerInfo>::iterator& _outInfo);


	RakNet::RakPeerInterface* m_peerInterface;



	ImGuiLogger* m_logger;

	bool m_isInitialized = false;

	std::map<RakNet::SystemAddress, PlayerInfo> m_playerInfo;
	unsigned int m_lastPlayerID = 0;

	//cached temporary variables
	RakNet::Packet* m_packet;
	std::map<RakNet::SystemAddress, PlayerInfo>::iterator m_thisPlayerInfo;

};

#endif // 
