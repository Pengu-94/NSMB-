#pragma once

#include "nsmb/game/stage/entity3danm.hpp"
#include "nsmb/game/physics/activecollider.hpp"

class Heaveho : public StageEntity3DAnm
{
public:
	enum class Type : u8
	{
		Chaser = 0,
		UpDown = 1,
		DownUp = 2
	};

	enum class Color : u8
	{
		Blue = 0,
		Green = 1,
		Yellow = 2
	};

	ActiveCollider collider;

	void (*updateFunc)(Heaveho*);
	s8 updateStep;

	Actor* hitActor; // Last actor hitting the block

	Vec3 initPos;  // Used when setting spawn values
	Vec3 startPos; // Used when changing states

	bool idle;

	u8 topPounded;
	u8 beingHit;

	u8 rotTimer;
	u8 hitTimer;

	static ActorProfile profile;

	constexpr static ObjectInfo objectInfo = {
		0, 0, // position
		0, 0, // renderSize
		0, 0, // spawnOffset
		0, 0, // viewOffset
		CollisionSwitch::None, // collisionSwitch
		SpawnSettings::None // spawnSettings
	};

	static bool loadFiles();
	s32 onCreate() override;
	s32 onUpdate() override;
	s32 onDestroy() override;
	void defeat(fx32 velX, fx32 velY, fx32 accelY, u8 arg) override;
	void defeatMega(fx32 velX, fx32 velY, fx32 accelY) override;

	void switchState(void (Heaveho::*updateFunc)());
	void setColor(Color color, bool setup = false);
	void rotateState();
	void bobbingState();
	void hitBehavior(bool animEnd);
	void hitState(Player* player);
	static void hitFromTop(ActiveCollider& _self, ActiveCollider& other);
	static void hitFromBottom(StageActor& self, StageActor& other);
	static void hitFromSide(StageActor& self, StageActor& other);
};