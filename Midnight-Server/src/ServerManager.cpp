#include "ServerManager.h"
#include "Server.h"
#include "Utilities/ImGuiLogger.h"
#include "Utilities/RakNetUtils.h"
#include "raknet/RakPeerInterface.h"
#include "raknet/MessageIdentifiers.h"
#include "raknet/BitStream.h"

ServerManager::ServerManager()
{
	m_logger = new ImGuiLogger();

	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	if(m_peerInterface != nullptr)
	{
		m_logger->AddLog("[ServerManager] Peer Interface Initialized\n");
	}
}

ServerManager::~ServerManager()
{
	RakNet::RakPeerInterface::DestroyInstance(m_peerInterface);
}

void ServerManager::Init(int _port)
{
	m_peerInterface->SetMaximumIncomingConnections(4);
	m_peerInterface->Startup(4, &RakNet::SocketDescriptor(_port, 0), 1);
	m_logger->AddLog("[ServerManager] Server started on socket: %i\n", _port);
	m_isInitialized = true;

}

void ServerManager::Update()
{
	if (!m_isInitialized)
		return;

	
}

void ServerManager::Draw()
{
	m_logger->Draw("Log");
}

void ServerManager::ReadPackets()
{
	for (m_packet = m_peerInterface->Receive(); m_packet; m_peerInterface->DeallocatePacket(m_packet), m_packet = m_peerInterface->Receive())
	{
		switch (m_packet->data[0])
		{
		case ID_NEW_INCOMING_CONNECTION:
			m_logger->AddLog("[ServerManager] Incoming connection\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			m_logger->AddLog("[ServerManager] A client has disconnected\n");
			DisconnectPlayer(m_packet->systemAddress);
			break;
		case ID_CONNECTION_LOST:
			m_logger->AddLog("[ServerManager] Connection lost\n");
			DisconnectPlayer(m_packet->systemAddress);
			break;
		case ID_SET_PLAYER_NAME:
			AddPlayer(m_packet);
			break;
		}
	}
}

void ServerManager::AddPlayer(const RakNet::Packet* _packet)
{
	const char* name = RakNetUtils::ReadStringFromPacket(m_packet);

	if (PlayerListContainsAddress(m_packet->systemAddress, m_thisPlayerInfo))
	{
		m_logger->AddLog("[ServerManager] Player already connected - updating name to: %s\n", name);
		m_thisPlayerInfo->second.playerName = (char*)name;
	}
	else
	{
		m_logger->AddLog("[ServerManager] Player set name to: %s\n", name);
		std::pair<RakNet::SystemAddress, PlayerInfo> newPlayerInfo;
		newPlayerInfo.first = m_packet->systemAddress;
		newPlayerInfo.second.playerName = (char*)name;
		newPlayerInfo.second.playerID = ++m_lastPlayerID;
		m_playerInfo.insert(newPlayerInfo);
	}
}

void ServerManager::DisconnectPlayer(const RakNet::SystemAddress _address)
{
	if (PlayerListContainsAddress(m_packet->systemAddress, m_thisPlayerInfo))
	{
		const char* name = m_thisPlayerInfo->second.playerName;
		m_logger->AddLog("Removing %s from client list\n", name);
		m_playerInfo.erase(m_thisPlayerInfo);
	}
}

const bool ServerManager::PlayerListContainsAddress(const RakNet::SystemAddress _address, std::map<RakNet::SystemAddress, PlayerInfo>::iterator & _outInfo)
{
	for (auto itr = m_playerInfo.begin(); itr != m_playerInfo.end(); itr++)
	{
		if(itr->first == _address)
		{
			_outInfo = itr;
			return true;
		}
	}
	
	return false;
}
