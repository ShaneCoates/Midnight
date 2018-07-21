#ifndef _SERVER_MANAGER_H_
#define _SERVER_MANAGER_H_

#include "raknet/RakPeerInterface.h"
#include "Game/GameRules.h"
#include <map>
struct ImGuiLogger;

class ServerManager
{
public:
	ServerManager();
	~ServerManager();

	void Init(int _port);

	void Update(double _dt);
	void Draw();


protected:
private:

	void ReadPackets();

	void AddPlayer(const RakNet::Packet* _packet);
	void DisconnectPlayer(const RakNet::SystemAddress _address);
	
	GameRules::PlayerInfo UpdatePlayerInput(GameRules::PlayerInfo _playerInfo, RakNet::MessageID _messageID);
	GameRules::PlayerInfo UpdatePlayer(GameRules::PlayerInfo _playerInfo, double _dt);

	void SendStateDump();

	const bool PlayerListContainsAddress(const RakNet::SystemAddress _address, std::map<RakNet::SystemAddress, GameRules::PlayerInfo>::iterator& _outInfo);

	const double k_timeBetweenStateDumps = 0.3f;
	double m_timeSinceStateDump = 0.0f;

	RakNet::RakPeerInterface* m_peerInterface;

	ImGuiLogger* m_logger;

	bool m_isInitialized = false;

	std::map<RakNet::SystemAddress, GameRules::PlayerInfo> m_playerInfo;
	unsigned int m_lastPlayerID = 0;

	//cached temporary variables
	RakNet::Packet* m_packet;
	std::map<RakNet::SystemAddress, GameRules::PlayerInfo>::iterator m_thisPlayerInfo;



};

#endif // 
