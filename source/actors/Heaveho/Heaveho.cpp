#include "Heaveho.hpp"

#include "nsmb/game/sound.hpp"
#include "nsmb/core/filesystem/cache.hpp"
#include "nsmb/core/graphics/particle.hpp"
#include "nsmb/game/stage/player.hpp"
#include "nsmb/core/system/function.hpp"

constexpr u32 ModelFileID = 2090 - 131;
constexpr u32 AnimFileID = 2091 - 131;

static ActiveColliderInfo Heaveho_colliderInfo =
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
	Heaveho::hitFromTop //callbackTop
};

ActorProfile Heaveho::profile = {
	&constructObject<Heaveho>,
	260, 260,
	Heaveho::loadFiles
};

bool Heaveho::loadFiles()
{
	FS::Cache::loadFile(ModelFileID, false);
	FS::Cache::loadFile(AnimFileID, false);
	return true;
}

s32 Heaveho::onCreate()
{
	void* mdl_s = FS::Cache::getFile(ModelFileID);
	void* anm = FS::Cache::getFile(AnimFileID);

	this->model.create(mdl_s, anm, 0, 0, 0);
	this->model.init(2, FrameCtrl::Type::Looping, 0x900, 0);

	scale = Vec3(100fx);

	this->initPos = this->position;

	this->startPos = this->position;

	alpha = 31;

	this->renderSize.x = 100000.0fx;
	this->renderSize.y = 100000.0fx;

	this->activeCollider.init(this, Heaveho_colliderInfo, 0);
	this->activeCollider.link();

	static LineSensorH topSensor = {-10.0fx, 10.0fx, 32.0fx};   // (left, right, y)
	static LineSensorV sideSensor = {32.0fx, 9.5fx, 16.0fx};    // (top, bottom, x)
	static PointSensor bottomSensor = {0.0fx, 0.0fx};

	this->collisionMgr.init(this, &bottomSensor, &topSensor, &sideSensor);

	this->switchState(&Heaveho::rotateState);

	return 1;
}

s32 Heaveho::onUpdate()
{
	updateFunc(this);
	updateAnimation();
	applyVelocity();
	updateCollisionSensors(); // for sensors
	destroyInactive(0);
	return 1;
}

s32 Heaveho::onDestroy()
{
	this->activeCollider.unlink();
	return 1;
}

void Heaveho::defeatMega(fx32 velX, fx32 velY, fx32 accelY)
{
	Particle::Handler::createParticle(202, this->position);
	this->destroy(true);
}

void Heaveho::defeat(fx32 velX, fx32 velY, fx32 accelY, u8 arg)
{
	Particle::Handler::createParticle(202, this->position);
	spawnCoin();
	this->destroy(true);
}


void Heaveho::switchState(void (Heaveho::*updateFunc)())
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

void Heaveho::setColor(Color color, bool setup)
{
	return;
}

void Heaveho::bobbingState()
{
	return;
}

void Heaveho::rotateState()
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
	
	if (this->velocity.y > -1.012fx)
		this->velocity.y = -0.26fx;

	if (this->idle) {
		if (this->rotTimer >= 2) {
			this->rotTimer-=2;
		} else {
			this->idle = false;
		}
		this->model.init(1, FrameCtrl::Type::Looping, 0x900, 0);
		this->velocity.x = Math::lerpFx32(this->velocity.x, 0fx, 0.12fx, 0.01fx, 0.01fx);
	}

	if (this->idle == false && this->rotTimer++ >= 100) {
		this->model.init(2, FrameCtrl::Type::Looping, 0x900, 0);
		this->idle = true;
	}	

	if (plyr->position.x > this->position.x && !this->idle) {
		if (this->rotation.y < 2.3fx)
			this->rotation.y+= 0.4fx;
		else {
			this->model.init(1, FrameCtrl::Type::Looping, 0x900, 0);
			this->velocity.x = 0.60fx;
			this->rotation.y = 2.3fx;
		}
	}

	else if (plyr->position.x < this->position.x && !this->idle) {
		if (this->rotation.y > -2.3fx)
			this->rotation.y-= 0.4fx;
		else {
			this->model.init(1, FrameCtrl::Type::Looping, 0x900, 0);
			this->velocity.x = -0.60fx;
			this->rotation.y = -2.3fx;
		}
	}
}

void Heaveho::hitBehavior(bool animEnd)
{
	if (animEnd)
	{
		Particle::Handler::createParticle(202, this->position);
		this->destroy(true);
	}
}

void Heaveho::hitState(Player* player)
{
    Particle::Handler::createParticle(202, this->position);
	this->destroy(true);
}

void Heaveho::hitFromTop(ActiveCollider& _self, ActiveCollider& other)
{
	Heaveho& self = scast<Heaveho&>(*_self.owner);

	//If not hit by player
	if (other.owner->actorType != ActorType::Player || self.beingHit)
		return;

	PlayerStompType playerStompType = self.updatePlayerStomp(_self, 1.0fx, false, false);
	Player& player = scast<Player&>(*other.owner);

	if (playerStompType >= PlayerStompType::Stomp)
	{
		self.hitActor = &player;
		self.hitState(&player);
	} else {
		player.setVerticalVelocity(9.0fx);
		player.velocity.x = self.direction == 0 ? -5.5fx : 5.5fx;
		self.model.init(0, FrameCtrl::Type::Standard, 0x900, 0);
	}
}

void Heaveho::hitFromBottom(StageActor& _self, StageActor& other)
{
	Heaveho& self = scast<Heaveho&>(_self);

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

void Heaveho::hitFromSide(StageActor& _self, StageActor& other)
{
	Heaveho& self = scast<Heaveho&>(_self);

	if (self.beingHit)
		return;
	else {
		Player& player = scast<Player&>(other);
		if (player.currentPowerup != PowerupState::Mega) {
			player.setVerticalVelocity(9.0fx);
			player.setHorizontalVelocity(self.direction == 0 ? -5.5fx : 5.5fx);
			self.model.init(0, FrameCtrl::Type::Looping, 0x900, 0);
		} else {
			self.hitState(&player);
		}
	}
}