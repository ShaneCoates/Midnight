#include "GameState/GameStateManager.h"
#include "GameState/GameState.h"

GameStateManager::GameStateManager() 
{
	m_toPop = 0;
}

GameStateManager::~GameStateManager() 
{
	auto itr = m_gameStates.begin();
	while (itr != m_gameStates.end()) 
	{
		delete itr->second;
		itr = m_gameStates.erase(itr);
	}
}

void GameStateManager::Update(double dt) {
	for (auto* i = m_stack.front(); i <= m_stack.back(); i++) 
	{
		i->Update(dt);
		if (i->isUpdateBlocking()) 
		{
			break;
		}
	}
	while (m_toPop > 0)	
	{
		m_stack.pop_front();	
		m_toPop--;
	}
}

void GameStateManager::Draw()
{
	for (auto* i = m_stack.front(); i <= m_stack.back(); i++) 
	{
		i->Draw();
		if (i->isDrawBlocking()) 
		{
			break;
		}
	}
}

void GameStateManager::RegisterState(std::string _name, IGameState* _state) 
{
	m_gameStates.insert(std::pair<std::string, IGameState*>(_name, _state));
}

bool GameStateManager::Push(std::string _name) 
{
	for (auto i = m_gameStates.begin(); i != m_gameStates.end(); i++) 
	{
		if (i->first == _name) 
		{
			m_stack.push_back(i->second);
			return true;
		}
	}
	return false;
}
void GameStateManager::Pop() {
	m_toPop++;
}