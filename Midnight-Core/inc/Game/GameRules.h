#ifndef _GAME_RULES_H_
#define	_GAME_RULES_H_

struct GameRules
{
	struct Player
	{
	public:
		static const float k_acceleration; 
		static const float k_maxSpeed;
		static const float k_friction;
	};
};

#endif // 
