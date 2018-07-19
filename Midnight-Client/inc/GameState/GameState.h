#ifndef _I_GAMESTATE_H_
#define _I_GAMESTATE_H_

class IGameState
{
public:

	//Update and Draw functions to be overridden
	virtual void Update(double dt) {};
	virtual void Draw() {};

	//Check to see if it blocks the update/draw functions of other states on the stack
	bool isUpdateBlocking() { return m_updateBlocking; }
	bool isDrawBlocking() { return m_drawBlocking; }

	//Sets blocking status'
	void SetUpdateBlocking(bool _block) { m_updateBlocking = _block; }
	void SetDrawBlocking(bool _block) { m_drawBlocking = _block; }

protected:
private:
	bool m_updateBlocking = true;
	bool m_drawBlocking = true;
};

#endif