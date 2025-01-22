#pragma once

#include "nsmb.hpp"
#include "nsmb_nitro.hpp"
#include "NSBTX.hpp"

class Lightning: public StageEntity {
public:
    static bool loadResources();
    virtual bool onPrepareResources() override;
    virtual s32 onCreate() override;

    virtual bool updateMain() override;
    virtual s32 onRender() override;

    virtual s32 onDestroy() override;

    static constexpr u16 objectID = 92; //objectID

    static constexpr ObjectInfo objectInfo = {
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        CollisionSwitch::None,
    };

    static constexpr u16 updatePriority = objectID;
    static constexpr u16 renderPriority = objectID;
    static constexpr ActorProfile profile = {&constructObject<Lightning>, updatePriority, renderPriority, loadResources};
    
    NSBTX lightningNsbtx;
};