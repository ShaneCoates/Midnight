#ifndef _GAME_RULES_H_
#define	_GAME_RULES_H_

#include "glm/glm.hpp"

struct GameRules
{
	struct Player
	{
	public:
		static const float k_acceleration; 
		static const float k_maxSpeed;
		static const float k_friction;
	};

	struct PlayerInfo
	{
		unsigned int playerID;
		char* playerName;
		uint8_t playerInput = 0;
		glm::vec3 position = glm::vec3(0);
		glm::vec3 velocity = glm::vec3(0);
		float rotation = 0.0f;
	};

	enum InputKeys
	{
		W = 0,
		S = 1,
		A = 2,
		D = 3
	};
};

#endif // 
