#include "nsmb.hpp"

ncp_call(0x020CC720, 1) // BootScene::onUpdate
static void replaceBootEnd() {

	static constexpr u8 Group = StageGroups::World1;
	static constexpr u8 Stage = Game::getStage(Group, 1);
	static constexpr u8 Area = 0;

	Game::loadLevel(
		SceneID::TitleScreen,	// scene
		0,						// vs mode
		0, 0, 0,		// group (world), stage, (sub) area
		0, 1,					// player ID, player mask
		0, 0,					// character 1, character 2
		1,						// powerup
		0,						// entrance
		1, 						// flag
		1, 						// <unused>
		-1,						// control mode
		1, 						// <unused>
		0, 						// challenge mode
		-1						// rng seed
	);
	
}
