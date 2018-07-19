/*
Author: Shane Coates
Description: Game State Manager class.
Holds a list of states and updates/draws them
*/

#ifndef _GAMESTATE_MANAGER_H_
#define	_GAMESTATE_MANAGER_H_

#include <map>
#include <list>
#include <string>

//IGameState class - to be inherited by actual states
class IGameState;

class GameStateManager
{
public:
	//Constructor
	GameStateManager();
	//Destructor
	~GameStateManager();

	//Update and Draw functions
	void Update(double _dt);
	void Draw();

	//Creates a new state and adds it to the map of states
	void RegisterState(std::string _name, IGameState* _state);

	//Pushes a state onto the stack after looking for it's name in the map
	bool Push(std::string _name);

	//Remove the top state from the stack
	void Pop();

private:
	std::map<std::string, IGameState*> m_gameStates;
	std::list<IGameState*> m_stack;

	unsigned int m_toPop;
};

#endif