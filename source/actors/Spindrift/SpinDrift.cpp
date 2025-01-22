#include "SpinDrift.hpp"

#include "nsmb/game/sound.hpp"
#include "nsmb/core/filesystem/cache.hpp"
#include "nsmb/core/graphics/particle.hpp"
#include "nsmb/game/stage/player.hpp"
#include "nsmb/core/system/function.hpp"

constexpr u32 ModelFileID = 1567 - 131;
constexpr u32 AnimFileID = 1566 - 131;

static ActiveColliderInfo SpinDrift_colliderInfo =
{
	{0.0fx, //left
	16.0fx, //top
	24.0fx, //right
	12.0fx, //bottom 
	},
	CollisionGroup::Entity,
	CollisionFlag::None,
	MAKE_GROUP_MASK(CollisionGroup::Player, CollisionGroup::Entity, CollisionGroup::Fireball),
	MAKE_FLAG_MASK_EX(CollisionFlag::None),
	0,
	SpinDrift::hitFromTop //callbackTop
};

ActorProfile SpinDrift::profile = {
	&constructObject<SpinDrift>,
	256, 171,
	SpinDrift::loadFiles
};

bool SpinDrift::loadFiles()
{
	FS::Cache::loadFile(ModelFileID, false);
	FS::Cache::loadFile(AnimFileID, false);
	return true;
}

s32 SpinDrift::onCreate()
{
	void* mdl_s = FS::Cache::getFile(ModelFileID);
	void* anm = FS::Cache::getFile(AnimFileID);

	this->model.create(mdl_s, anm, 0, 0, 0);
	this->model.init(0, FrameCtrl::Type::Looping, 0x900, 0);

	Settings settings;
	settings.raw() = this->settings;

	this->type = Type(settings.type);

	scale = 0x1750;

	this->initPos = this->position;

	this->startPos = this->position;

	alpha = 31;

	bobbing = 0;

	this->renderSize.x = 100000.0fx;
	this->renderSize.y = 100000.0fx;

	this->activeCollider.init(this, SpinDrift_colliderInfo, 0);
	this->activeCollider.link();

	static LineSensorH topSensor = {-10.0fx, 10.0fx, 32.0fx};   // (left, right, y)
	static LineSensorV sideSensor = {32.0fx, 9.5fx, 16.0fx};    // (top, bottom, x)
	static PointSensor bottomSensor = {0.0fx, 0.0fx};

	this->collisionMgr.init(this, &bottomSensor, &topSensor, &sideSensor);

	if (settings.type == 0)
		this->switchState(&SpinDrift::rotateState);
	else
		this->switchState(&SpinDrift::bobbingState);

	return 1;
}

s32 SpinDrift::onUpdate()
{
	updateFunc(this);
	updateAnimation();
	applyVelocity();
	updateCollisionSensors(); // for sensors
	destroyInactive(0);
	return 1;
}

s32 SpinDrift::onDestroy()
{
	this->activeCollider.unlink();
	return 1;
}

void SpinDrift::defeatMega(fx32 velX, fx32 velY, fx32 accelY)
{
	Particle::Handler::createParticle(202, this->position);
	this->destroy(true);
}

void SpinDrift::defeat(fx32 velX, fx32 velY, fx32 accelY, u8 arg)
{
	Particle::Handler::createParticle(202, this->position);
	spawnCoin();
	this->destroy(true);
}


void SpinDrift::switchState(void (SpinDrift::*updateFunc)())
{
	auto updateFuncRaw = ptmf_cast(updateFunc);

	if (this->updateFunc != updateFuncRaw)
	{
		if (this->updateFunc)
		{
			this->updateStep = Func::Exit;
			this->updateFunc(this);
		}

		this->updateFunc = updateFuncRaw;

		this->updateStep = Func::Init;
		this->updateFunc(this);
	}
}

void SpinDrift::setColor(Color color, bool setup)
{
	return;
}

void SpinDrift::bobbingState()
{
	if (this->updateStep == Func::Init)
	{
		this->updateStep = 1;
		this->position = this->startPos;
		return;
	}
	if (this->updateStep == Func::Exit)
	{
		return;
	}

	s32* distX, *distY;

	Player* plyr = this->getClosestPlayer(distX, distY);

	bobbing += 30;

	Settings settings;
	settings.raw() = this->settings;

	this->type = Type(settings.type);
	
	if (this->type == Type::UpDown)
		this->velocity.y = Math::sin((bobbing * 6.24)) / 2;
	else
		this->velocity.y = Math::sin((-bobbing * 6.24)) / 2;

	if (plyr->position.x > this->position.x) {
		if (this->rotation.y < 2.3fx)
			this->rotation.y+= 0.4fx;
		else {
			this->rotation.y = 2.3fx;
		}
	}

	else if (plyr->position.x < this->position.x) {
		if (this->rotation.y > -2.3fx)
			this->rotation.y-= 0.4fx;
		else {
			this->rotation.y = -2.3fx;
		}
	}
}

void SpinDrift::rotateState()
{
	if (this->updateStep == Func::Init)
	{
		this->updateStep = 1;
		this->position = this->startPos;
		return;
	}
	if (this->updateStep == Func::Exit)
	{
		return;
	}

	s32* distX, *distY;

	Player* plyr = this->getClosestPlayer(distX, distY);

	this->velocity.y = -0.36fx;

	if (plyr->position.x > this->position.x) {
		if (this->rotation.y < 2.3fx)
			this->rotation.y+= 0.4fx;
		else {
			this->velocity.x = 0.29fx;
			this->rotation.y = 2.3fx;
		}
	}

	else if (plyr->position.x < this->position.x) {
		if (this->rotation.y > -2.3fx)
			this->rotation.y-= 0.4fx;
		else {
			this->velocity.x = -0.29fx;
			this->rotation.y = -2.3fx;
		}
	}
}

void SpinDrift::hitBehavior(bool animEnd)
{
	if (animEnd)
	{
		Particle::Handler::createParticle(202, this->position);
		this->destroy(true);
	}
}

Vec3 posRand;

void SpinDrift::hitState(Player* player)
{
	if (player->currentPowerup != PowerupState::Mega) {
		player->doSpinJump(0x4250);
		Sound::playSFX(199);
	}
	Particle::Handler::createParticle(202, this->position);
	for (int i = 0; i < 6; i++)
	{
		fx32 offset;
		offset = Stage::getRandom(-10, 10);
		Settings setting;
		setting.downShift = 0x3;
		posRand.set(position.x + offset, position.y + offset, position.z + offset);
		spawnActor(66, setting, &posRand, nullptr, nullptr, nullptr);
	}
	
	this->destroy(true);
}

void SpinDrift::hitFromTop(ActiveCollider& _self, ActiveCollider& other)
{
	SpinDrift& self = scast<SpinDrift&>(*_self.owner);

	//If not hit by player
	if (other.owner->actorType != ActorType::Player || self.beingHit)
		return;

	PlayerStompType playerStompType = self.updatePlayerStomp(_self, 1.0fx, true, false);
	Player& player = scast<Player&>(*other.owner);

	if (playerStompType >= PlayerStompType::Stomp)
	{
		self.hitActor = &player;
		self.hitState(&player);
	} else {
		self.damagePlayerCallback(_self, other);
	}
}

void SpinDrift::hitFromBottom(StageActor& _self, StageActor& other)
{
	SpinDrift& self = scast<SpinDrift&>(_self);

	//If not hit by player or if being hit already
	if (other.actorType != ActorType::Player || self.beingHit)
		return;	
	else {
		Player& player = scast<Player&>(other);
		if (player.currentPowerup != PowerupState::Mega) {
			damagePlayerCallback(_self.activeCollider, other.activeCollider); 
		} else {
			self.hitState(&player);
		}
	}
}

void SpinDrift::hitFromSide(StageActor& _self, StageActor& other)
{
	SpinDrift& self = scast<SpinDrift&>(_self);

	if (self.beingHit)
		return;
	else {
		Player& player = scast<Player&>(other);
		if (player.currentPowerup != PowerupState::Mega) {
			damagePlayerCallback(_self.activeCollider, other.activeCollider); 
		} else {
			self.hitState(&player);
		}
	}
}
