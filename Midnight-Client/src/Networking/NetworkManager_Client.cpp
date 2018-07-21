#include "Networking/NetworkManager_Client.h"
#include "MessageIdentifiers.h"
#include "Game/GameRules.h"

NetworkManager_Client::NetworkManager_Client()
{
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	m_peerInterface->Startup(1, &RakNet::SocketDescriptor(), 1);
}

NetworkManager_Client::~NetworkManager_Client()
{
	RakNet::RakPeerInterface::DestroyInstance(m_peerInterface);
}

void NetworkManager_Client::CheckForPackets()
{
	for(m_packet = m_peerInterface->Receive(); m_packet; m_peerInterface->DeallocatePacket(m_packet), m_packet = m_peerInterface->Receive())
	{
		switch(m_packet->data[0])
		{
		case ID_CONNECTION_REQUEST_ACCEPTED:
			m_connected = true;
			m_systemAddress = m_packet->systemAddress;
			if(m_onConnectionSuccessful != nullptr)
			{
				m_onConnectionSuccessful();
				m_onConnectionSuccessful = nullptr;
				m_onConnectionFailed = nullptr;
			}
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Another client has disconnected.\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Another client has connected.\n");
			break;
		case ID_NEW_INCOMING_CONNECTION:
			printf("A connection is incoming.\n");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			if(m_onConnectionFailed != nullptr)
			{
				m_onConnectionFailed();
				m_onConnectionSuccessful = nullptr;
				m_onConnectionFailed = nullptr;
			}
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			printf("We have been disconnected.\n");
			break;
		case ID_CONNECTION_LOST:
			printf("Connection lost.\n");
			break;
		case ID_SET_PLAYER_ID:
			m_playerID = m_packet->data[1];
			break;
		case ID_SND_RECEIPT_ACKED:
			AcknowledgePacket();
			break;
		case ID_SND_RECEIPT_LOSS:
			AcknowledgeLostPacket();
			break;
		case ID_STATE_DUMP:
			ReceiveStateDump();
			break;
		}
	}
}

void NetworkManager_Client::AcknowledgePacket()
{
	RakNet::MessageID messageID;
	messageID = 0;
	for each (auto itr in m_unacknowledgedMessages)
	{
		if(itr.second == m_packet->data[1])
		{
			messageID = itr.first;
		}
	}
	if(messageID != 0)
	{
		m_unacknowledgedMessages.erase(m_unacknowledgedMessages.find(messageID));
	}
	printf("receipt acknowledged: %i\n", m_packet->data[1]);
}

void NetworkManager_Client::AcknowledgeLostPacket()
{
	for each (auto itr in m_unacknowledgedMessages)
	{
		if(itr.second == m_packet->data[1])
		{
			SendInputStatus(itr.first);
		}
	}
	printf("receipt lost: %i\n", m_packet->data[1]);
}

void NetworkManager_Client::ReceiveStateDump()
{
	RakNet::BitStream bsIn(m_packet->data, m_packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	bsIn.Serialize(false, m_playerInfo);
}

void NetworkManager_Client::AttemptConnect(const char* _serverAddress, const int _serverPort, std::function<void()> _onConnectionSuccessful, std::function<void()> _onConnectionFailed)
{
	m_onConnectionSuccessful = _onConnectionSuccessful;
	m_onConnectionFailed = _onConnectionFailed;

	m_peerInterface->Connect(_serverAddress, _serverPort, 0, 0);
}

void NetworkManager_Client::SetPlayerName(const char * _playerName)
{
	m_bitStream.Reset();
	m_bitStream.Write((RakNet::MessageID)ID_SET_PLAYER_NAME);
	m_bitStream.Write(_playerName);
	m_peerInterface->Send(&m_bitStream, PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE, 0, m_systemAddress, false);
}

void NetworkManager_Client::SendInputStatus(RakNet::MessageID _ID)
{
	m_bitStream.Reset();
	m_bitStream.Write((RakNet::MessageID)_ID);
	uint32_t messageID = m_peerInterface->Send(&m_bitStream, PacketPriority::HIGH_PRIORITY, PacketReliability::UNRELIABLE_WITH_ACK_RECEIPT, 0, m_systemAddress, false);
	m_unacknowledgedMessages[_ID] = messageID;
}

void NetworkManager_Client::SendInputStatus(RakNet::MessageID _ID, float _value)
{
	m_bitStream.Reset();
	m_bitStream.Write((RakNet::MessageID)_ID);
	m_bitStream.Serialize(true, _value);
	uint32_t messageID = m_peerInterface->Send(&m_bitStream, PacketPriority::HIGH_PRIORITY, PacketReliability::UNRELIABLE_WITH_ACK_RECEIPT, 0, m_systemAddress, false);
}

