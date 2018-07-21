#include "ServerManager.h"
#include "Server.h"
#include "Utilities/ImGuiLogger.h"
#include "Utilities/RakNetUtils.h"
#include "raknet/RakPeerInterface.h"
#include "raknet/MessageIdentifiers.h"
#include "raknet/BitStream.h"
#include "glm/gtx/transform.hpp"

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

void ServerManager::Update(double _dt)
{
	if (!m_isInitialized)
		return;	

	ReadPackets();

	for each (auto player in m_playerInfo)
	{
		m_playerInfo[player.first] = UpdatePlayer(player.second, _dt);
	}

	m_timeSinceStateDump += _dt;
	if(m_timeSinceStateDump > k_timeBetweenStateDumps)
	{
		m_timeSinceStateDump -= k_timeBetweenStateDumps;
		SendStateDump();
	}
}

void ServerManager::Draw()
{
	m_logger->Draw("Log");
}

void ServerManager::ReadPackets()
{
	for (m_packet = m_peerInterface->Receive(); m_packet; m_peerInterface->DeallocatePacket(m_packet), m_packet = m_peerInterface->Receive())
	{
		switch(m_packet->data[0])
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
		case ID_PLAYER_INPUT_ROTATION_DELTA:
			if(PlayerListContainsAddress(m_packet->systemAddress, m_thisPlayerInfo))
			{
				float rotationDelta = 0.0f;
				RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Serialize(false, rotationDelta);
				m_playerInfo[m_thisPlayerInfo->first].rotation -= rotationDelta;
			}
			break;
		case ID_PLAYER_INPUT_W_DOWN:
		case ID_PLAYER_INPUT_W_UP:
		case ID_PLAYER_INPUT_S_DOWN:
		case ID_PLAYER_INPUT_S_UP:
		case ID_PLAYER_INPUT_A_DOWN:
		case ID_PLAYER_INPUT_A_UP:
		case ID_PLAYER_INPUT_D_DOWN:
		case ID_PLAYER_INPUT_D_UP:
			if(PlayerListContainsAddress(m_packet->systemAddress, m_thisPlayerInfo))
			{
				m_playerInfo[m_thisPlayerInfo->first] = UpdatePlayerInput(m_thisPlayerInfo->second, (RakNet::MessageID)m_packet->data[0]);
			}

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
		std::pair<RakNet::SystemAddress, GameRules::PlayerInfo> newPlayerInfo;
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

GameRules::PlayerInfo ServerManager::UpdatePlayerInput(GameRules::PlayerInfo _playerInfo, RakNet::MessageID _messageID)
{
	switch(_messageID)
	{
	case ID_PLAYER_INPUT_W_DOWN:
		_playerInfo.playerInput |= (1 << GameRules::InputKeys::W);
		break;
	case ID_PLAYER_INPUT_W_UP:
		_playerInfo.playerInput &= ~(1 << GameRules::InputKeys::W);
		break;
	case ID_PLAYER_INPUT_S_DOWN:
		_playerInfo.playerInput |= (1 << GameRules::InputKeys::S);
		break;
	case ID_PLAYER_INPUT_S_UP:
		_playerInfo.playerInput &= ~(1 << GameRules::InputKeys::S);
		break;
	case ID_PLAYER_INPUT_A_DOWN:
		_playerInfo.playerInput |= (1 << GameRules::InputKeys::A);
		break;
	case ID_PLAYER_INPUT_A_UP:
		_playerInfo.playerInput &= ~(1 << GameRules::InputKeys::A);
		break;
	case ID_PLAYER_INPUT_D_DOWN:
		_playerInfo.playerInput |= (1 << GameRules::InputKeys::D);
		break;
	case ID_PLAYER_INPUT_D_UP:
		_playerInfo.playerInput &= ~(1 << GameRules::InputKeys::D);
		break;
	}
	return _playerInfo;
}

GameRules::PlayerInfo ServerManager::UpdatePlayer(GameRules::PlayerInfo _playerInfo, double _dt)
{
	_playerInfo.velocity -= _playerInfo.velocity * (GameRules::Player::k_friction * (float)_dt); //friction

	if(_playerInfo.playerInput & (1 << GameRules::InputKeys::W))
	{
		_playerInfo.velocity.z += GameRules::Player::k_acceleration * _dt;
	}
	if(_playerInfo.playerInput & (1 << GameRules::InputKeys::S))
	{
		_playerInfo.velocity.z -= GameRules::Player::k_acceleration * _dt;
	}
	if(_playerInfo.playerInput & (1 << GameRules::InputKeys::A))
	{
		_playerInfo.velocity.x += GameRules::Player::k_acceleration * _dt;
	}
	if(_playerInfo.playerInput & (1 << GameRules::InputKeys::D))
	{
		_playerInfo.velocity.x -= GameRules::Player::k_acceleration * _dt;
	}

	if(_playerInfo.velocity.length() > GameRules::Player::k_maxSpeed)
	{
		_playerInfo.velocity = glm::normalize(_playerInfo.velocity) * GameRules::Player::k_maxSpeed;
	}

	glm::mat4 trans(1);
	trans = glm::translate(trans, _playerInfo.position);

	trans = glm::rotate(trans, _playerInfo.rotation, glm::vec3(0, 1, 0));

	trans = glm::translate(trans, _playerInfo.velocity * (float)_dt);

	_playerInfo.position = trans[3].xyz;

	if(_playerInfo.position.x > 8.5f)
	{
		_playerInfo.position.x = 8.5f;
		_playerInfo.velocity.x *= -0.25f;
	}

	if(_playerInfo.position.z > 8.5f)
	{
		_playerInfo.position.z = 8.5f;
		_playerInfo.velocity.z *= -0.25f;
	}

	if(_playerInfo.position.x < -8.5f)
	{
		_playerInfo.position.x = -8.5f;
		_playerInfo.velocity.x *= -0.25f;
	}

	if(_playerInfo.position.z < -8.5f)
	{
		_playerInfo.position.z = -8.5f;
		_playerInfo.velocity.z *= -0.25f;
	}

	_playerInfo.position.y = 0.5f;// +(sin(m_timeSinceSpawn * 3.0f) * 0.05f);
	return _playerInfo;
}

void ServerManager::SendStateDump()
{
	RakNet::BitStream bitStream;
	bitStream.Write((RakNet::MessageID)ID_STATE_DUMP);
	
	for each (auto player in m_playerInfo)
	{
		bitStream.Serialize(true, player.second);
	}

	for each (auto player in m_playerInfo)
	{
		uint32_t messageID = m_peerInterface->Send(&bitStream, PacketPriority::HIGH_PRIORITY, PacketReliability::UNRELIABLE_WITH_ACK_RECEIPT, 0, player.first, false);
	}
}


const bool ServerManager::PlayerListContainsAddress(const RakNet::SystemAddress _address, std::map<RakNet::SystemAddress, GameRules::PlayerInfo>::iterator & _outInfo)
{
	for(auto itr = m_playerInfo.begin(); itr != m_playerInfo.end(); itr++)
	{
		if(itr->first == _address)
		{
			_outInfo = itr;
			return true;
		}
	}

	return false;
}
