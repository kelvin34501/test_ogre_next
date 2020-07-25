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

GraphicsGameState::GraphicsGameState(const Ogre::String &help_desc) : TutorialGameState(help_desc), scene_node(nullptr),
                                                                      last_position(Ogre::Vector3::ZERO),
                                                                      curr_position(Ogre::Vector3::ZERO),
                                                                      enable_interpolation(true) {

}

void GraphicsGameState::createScene01() {
    Ogre::SceneManager *scene_manager = mGraphicsSystem->getSceneManager();
    Ogre::Item *item = scene_manager->createItem("Cube_d.mesh",
                                                 Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                                                 Ogre::SCENE_DYNAMIC);
    scene_node = scene_manager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
    scene_node->attachObject(item);

    TutorialGameState::createScene01();
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
        float weight = mGraphicsSystem->getAccumTimeSinceLastLogicFrame() / frame_time;
        weight = std::min(1.0f, weight);

        if (!enable_interpolation)
            weight = 0;

        out_text += "\nBlend weight: ";
        out_text += Ogre::StringConverter::toString(weight);
    }
}

void GraphicsGameState::update(float time_since_last) {
    float weight = mGraphicsSystem->getAccumTimeSinceLastLogicFrame() / frame_time;
    weight = std::min(1.0f, weight);

    if (!enable_interpolation) {
        weight = 0;
    }

    Ogre::Vector3 interpolated_position = Ogre::Math::lerp(last_position, curr_position, weight);
    scene_node->setPosition(interpolated_position);

    TutorialGameState::update(time_since_last);
}

void GraphicsGameState::keyReleased(const SDL_KeyboardEvent &arg) {
    if ((arg.keysym.mod & ~(KMOD_NUM | KMOD_CAPS)) != 0) {
        TutorialGameState::keyReleased(arg);
        return;
    }

    if (arg.keysym.sym == SDLK_F2) {
        fake_frame_skip = !fake_frame_skip;
    } else if (arg.keysym.sym == SDLK_F3) {
        fake_slowmotion = !fake_slowmotion;
    } else if (arg.keysym.sym == SDLK_F4) {
        enable_interpolation = !enable_interpolation;
    } else {
        TutorialGameState::keyReleased(arg);
    }
}


Ogre::Vector3 &GraphicsGameState::get_last_position_ref() {
    return last_position;
}

Ogre::Vector3 &GraphicsGameState::get_curr_position_ref() {
    return curr_position;
}
