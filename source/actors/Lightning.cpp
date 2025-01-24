#include "Lightning.hpp"

ncp_over(0x020C619C, 0) const ObjectInfo objectInfo = Lightning::objectInfo; //Stage Actor ID 192
ncp_over(0x02039AEC) static constexpr const ActorProfile* profile = &Lightning::profile; //objectID 92

bool Lightning::onPrepareResources(){
	lightningNsbtx.setup(FS::Cache::getFile(2089 - 131), Vec2(16, 16), Vec2(0, 0), 0, 0);
    return 1;
}

bool Lightning::loadResources() {
    return true;
}

// Code that runs the first time the Actor loads in
s32 Lightning::onCreate() {
    onPrepareResources();

    activeSize = Vec2(1000.0, 1000.0);

    this->velocity.y = 1.15fx;

    return 1;
}

// Code that runs every frame
bool Lightning::updateMain() {

	applyVelocity();

    this->velocity.y *= 0.965;

    if (this->velocity.y <= 0.12fx) {
        this->destroy(true);
    }

    return 1;
}

s32 Lightning::onRender() {
    Vec3 scale(1fx);
    lightningNsbtx.render(position, scale);
    return 1;
}

// Code runs when this Actor is being destroyed
s32 Lightning::onDestroy() {
    return 1;
}