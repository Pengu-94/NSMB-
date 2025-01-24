#include "ExclamationBox.hpp"


#include "nsmb/game/sound.hpp"
#include "nsmb/game/stage/player.hpp"
#include "nsmb/core/filesystem/cache.hpp"
#include "nsmb/core/graphics/particle.hpp"
#include "nsmb/core/system/function.hpp"

constexpr u32 ModelFileID = 1584 - 131;
constexpr bool ResetRotationBug = false; // Enables a bug that existed in the beta

static ColliderInfo ExclamationBox_colliderInfo =
{
	-0x10000, //left
	0x10000, //top
	0x10000, //right
	-0x10000, //bottom
	{
		ExclamationBox::hitFromTop, //callbackTop
		ExclamationBox::hitFromBottom, //callbackBottom
		ExclamationBox::hitFromSide, //callbackSide
	}
};

ActorProfile ExclamationBox::profile = {
	&constructObject<ExclamationBox>,
	0xFD, 0x6C,
	ExclamationBox::loadFiles
};

bool ExclamationBox::loadFiles()
{
	FS::Cache::loadFile(ModelFileID, false);
    FS::Cache::loadFile(2089 - 131, false);
	return true;
}

s32 ExclamationBox::onCreate()
{
	void* modelFile = FS::Cache::getFile(ModelFileID);
    this->model.create(modelFile, 0, 0);

	Settings settings;
	settings.raw() = this->settings;

	this->type = Type(settings.type);
	this->color = Color(settings.color);

    if (ResetRotationBug)
    {
        this->rotTimer = 0;
        this->rotation.y = 0;
    }

	this->direction = settings.direction ? 1 : -1;
	this->size = settings.size;
	this->item = settings.item;
	this->singleUse = settings.singleUse;
	this->puffs = 1;
	this->halfSize = (this->size * 0x1000) / 2;

	int tileSize = 0x20 * ((this->size + 9) / 16);
	this->renderSize.x = tileSize;
	this->renderSize.y = tileSize;

	this->initPos = this->position;

	this->scale = (this->size * 0x1000) / 32;

	this->position.y -= 0x1000 * settings.downShift;
	this->position.x += 0x1000 * settings.rightShift;

	this->startPos = this->position;

	alpha = 31;
	this->setColor(this->color, true);

	this->collider.init(this, ExclamationBox_colliderInfo, 0, 0, &this->scale);
	this->collider.link();

	this->beingHit = false;
	this->rotTimer = 0;
	this->switchState(&ExclamationBox::rotateState);

	return 1;
}

s32 ExclamationBox::onUpdate()
{
	updateFunc(this);
	destroyInactive(0);
	return 1;
}

s32 ExclamationBox::onDestroy()
{
	this->collider.unlink();
	return 1;
}

void ExclamationBox::switchState(void (ExclamationBox::*updateFunc)())
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

void ExclamationBox::setColor(Color color, bool setup)
{
	if (this->color != color || setup)
	{
		this->color = color;
	}
}

Player playerActivator;

void ExclamationBox::rotateState()
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

	const int sleepTime = 10;
	const int rotateTime = 28;
	if (this->rotTimer > sleepTime)
	{
		this->rotation.y -= (0x4000 / rotateTime) * this->direction;
		if (this->rotTimer == rotateTime + sleepTime)
		{
			this->rotation.y = 0;
			this->rotTimer = -1;
		}
	}
	this->rotTimer++;
}

void ExclamationBox::hitBehavior(bool animEnd)
{
	switch (this->type)
    {
	case Type::ItemSpawner:
	{
		if (animEnd == (this->item != 3))
			break;

		Vec3 spawnPos = this->startPos;
		u32 settings = this->item;
		if (this->item == 5)
		{
			settings |= 0x1000000;
		}
		if (this->topPounded)
		{
			if (!this->puffs)
			{
				settings |= 0x60;
			}
			spawnPos.y -= this->halfSize;
		}
		else
		{
			spawnPos.y += this->halfSize - 0x10000;
		}
		if (this->puffs)
		{
			settings |= 0x1000000;
			//spawnPos.y += 192 << 12;
		}

		u32 direction = (this->hitActor->position.x > this->startPos.x) ? 0x80000000 : 0;
		settings |= direction;

		Actor::spawnActor(31, settings, &spawnPos, 0, 0, 0);
		break;
	}

	default:
		break;
	}

	if (animEnd)
	{
        if (this->type == Type::LightningBoltSpawner)
        {
			Vec3 pos;
			pos.set(this->position);

			pos.x -= 8fx;

			spawnActor(92, 0, &pos);
			Particle::Handler::createParticle(202, Game::getPlayer((playerActivator.playerID + 1) % 2)->position);
			Game::getPlayer((playerActivator.playerID + 1) % 2)->switchPowerupDirect(PowerupState::Mini);     
        }

        Particle::Handler::createParticle(202, this->position);
        this->destroy(true);
	}
}

void ExclamationBox::hitState()
{
	if (this->updateStep == Func::Init)
	{
		this->updateStep = 1;
		this->beingHit = true;
		this->hitTimer = 0;
		this->position = this->startPos;
		return;
	}
	if (this->updateStep == Func::Exit)
	{
		this->beingHit = false;
		this->topPounded = false;
		return;
	}

	int direction = this->topPounded ? -1 : 1;
	if (this->hitTimer < 10)
	{
        this->scale.x += 0x200;
        this->scale.y -= 0x100;
        this->scale.z += 0x200;
		this->position.y += 0x1000 * direction;
		if (this->hitTimer == 9)
		{
			this->hitBehavior(false);
		}
	}
	else
	{
        this->scale.x -= 0x200;
        this->scale.y += 0x100;
        this->scale.z -= 0x200;
		this->position.y -= 0x1000 * direction;
		if (this->hitTimer == 19)
		{
            this->scale = (this->size * 0x1000) / 32;
			this->hitBehavior(true);
		}
	}
	this->collider.updatePosition();

	this->hitTimer++;
}

void ExclamationBox::hitFromTop(StageActor& _self, StageActor& other)
{
	ExclamationBox& self = scast<ExclamationBox&>(_self);

	//If not hit by player
	if (other.actorType != ActorType::Player || self.beingHit)
		return;

	Player& player = scast<Player&>(other);

	if (player.actionFlag.groundpounding && (player.animID != 0x15))
	{
		self.topPounded = true;
		self.hitActor = &player;
        playerActivator = player;
		self.switchState(&ExclamationBox::hitState);
	}
}

void ExclamationBox::hitFromBottom(StageActor& _self, StageActor& other)
{
	ExclamationBox& self = scast<ExclamationBox&>(_self);

	//If not hit by player or if being hit already
	if (other.actorType != ActorType::Player || self.beingHit)
		return;

	Player& player = scast<Player&>(other);

	self.hitActor = &player;
    playerActivator = player;
	self.switchState(&ExclamationBox::hitState);
}

void ExclamationBox::hitFromSide(StageActor& _self, StageActor& other)
{
	ExclamationBox& self = scast<ExclamationBox&>(_self);

	if (self.beingHit)
		return;

	if (bool(other.collisionMgr.sideSensor->flags & CollisionMgr::SensorFlags::ActivateQuestionBlocks))
	{
		self.hitActor = &other;
		self.switchState(&ExclamationBox::hitState);
	}
}