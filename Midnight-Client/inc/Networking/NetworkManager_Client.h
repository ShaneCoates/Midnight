#ifndef _NETWORK_MANAGER_CLIENT_H_
#define _NETWORK_MANAGER_CLIENT_H_

#include "RakPeerInterface.h"
#include "raknet\BitStream.h"
#include <functional>
#include <map>
#include "Game/GameRules.h"
class NetworkManager_Client
{
public:
	NetworkManager_Client();
	~NetworkManager_Client();

	//read all incoming packets
	void CheckForPackets();

	void AcknowledgePacket();
	void AcknowledgeLostPacket();

	void ReceiveStateDump();

	//Attempt connection with success and failure callbacks
	void AttemptConnect(const char* _serverAddress, const int _serverPort, std::function<void()> _onConnectionSuccessful = nullptr, std::function<void()> _onConnectionFailed = nullptr);
	void SetPlayerName(const char* _playerName);

	void SendInputStatus(RakNet::MessageID _ID);
	void SendInputStatus(RakNet::MessageID _ID, float _value);

	GameRules::PlayerInfo m_playerInfo;

private:

	
	RakNet::RakPeerInterface *m_peerInterface;
	RakNet::Packet *m_packet;
	RakNet::BitStream m_bitStream;
	RakNet::SystemAddress m_systemAddress;
	unsigned int m_playerID = MAXINT;
	bool m_connected = false;

	std::map<RakNet::MessageID, uint32_t> m_unacknowledgedMessages;

	std::function<void()> m_onConnectionSuccessful;
	std::function<void()> m_onConnectionFailed;

};

#endif
