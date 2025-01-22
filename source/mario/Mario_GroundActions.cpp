#include "nsmb.hpp"
#include "MarioAnims.hpp"

// Walking transition delay
ncp_repl(0x02116674, 10, "B 0x02116680")

ncp_call(0x02116698, 10) 
void marioWalk(Player *player, int id, bool doBlend, Player::FrameMode frameMode, fx32 speed, u16 frame) {
	// 0x3C00 is the max walk animation speed
	if (speed > (0x3C00 / 2))
		speed = (0x3C00 / 2);

	if (player->animID == MWalk)
	{
		player->setBodyAnimationSpeed(speed * 1.25f);
	}
	else
	{
		if (player->animID == 1)
		{
			if (MATH_ABS(player->velocity.x) >= 0x1800)
				player->setAnimation(MWalk, doBlend, frameMode, speed, frame);
			else
				player->setBodyAnimationSpeed(speed);
		}
		else
			player->setAnimation(MWalkSlow, doBlend, frameMode, speed, frame);
	}
}
ncp_repl(0x02116A14, 10, "CMP R0, #1")
ncp_repl(0x02116A2C, 10, "MOV R1, #1")