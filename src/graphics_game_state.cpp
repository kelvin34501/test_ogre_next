//
// Created by kelvin34501 on 7/24/20.
//

#include "graphics_game_state.h"
#include "GraphicsSystem.h"
#include "OgreSceneManager.h"
#include "OgreItem.h"
#include "OgreTextAreaOverlayElement.h"

namespace Tutorial {
    extern const double frame_time;
    extern bool fake_slowmotion;
    extern bool fake_frame_skip;
}

using namespace Tutorial;

GraphicsGameState::GraphicsGameState(const Ogre::String &help_desc) :
        Demo::TutorialGameState(help_desc),
        enable_interpolation(true) {
}

void GraphicsGameState::generateDebugText(float time_since_last, Ogre::String &out_text) {
    TutorialGameState::generateDebugText(time_since_last, out_text);

    out_text += "\nPress F2 to fake a GPU bottleneck (frame skip). ";
    out_text += fake_frame_skip ? "[On]" : "[Off]";
    out_text += "\nPress F3 to fake a CPU Logic bottleneck. ";
    out_text += fake_slowmotion ? "[On]" : "[Off]";
    out_text += "\nPress F4 to enable interpolation. ";
    out_text += enable_interpolation ? "[On]" : "[Off]";

    //Show the current weight.
    //The text doesn't get updated every frame while displaying
    //help, so don't show the weight as it is inaccurate.
    if (mDisplayHelpMode != 0) {
        out_text += "\nSEE HELP DESCRIPTION!";

        auto weight = static_cast<float>(mGraphicsSystem->getAccumTimeSinceLastLogicFrame() / frame_time);
        weight = std::min(1.0f, weight);

        if (!enable_interpolation)
            weight = 0;

        out_text += "\nBlend weight: ";
        out_text += Ogre::StringConverter::toString(weight);
    }

    out_text += "\nPress ESC to quit. ";
}

void GraphicsGameState::update(float time_since_last) {
    auto weight = static_cast<float>(mGraphicsSystem->getAccumTimeSinceLastLogicFrame() / frame_time);
    weight = std::min(1.0f, weight);

    if (!enable_interpolation) {
        weight = 0;
    }

    mGraphicsSystem->updateGameEntities(
            mGraphicsSystem->getGameEntities(Ogre::SCENE_DYNAMIC),
            weight
    );
    TutorialGameState::update(time_since_last);
}

void GraphicsGameState::keyReleased(const SDL_KeyboardEvent &arg) {
    if ((arg.keysym.mod & ~(KMOD_NUM | KMOD_CAPS)) != 0) {
        TutorialGameState::keyReleased(arg);
        return;
    }

    if (arg.keysym.sym == SDLK_ESCAPE) {
        mGraphicsSystem->setQuit();
    } else if (arg.keysym.sym == SDLK_F2) {
        fake_frame_skip = !fake_frame_skip;
    } else if (arg.keysym.sym == SDLK_F3) {
        fake_slowmotion = !fake_slowmotion;
    } else if (arg.keysym.sym == SDLK_F4) {
        enable_interpolation = !enable_interpolation;
    } else {
        TutorialGameState::keyReleased(arg);
    }
}
