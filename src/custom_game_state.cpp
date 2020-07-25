//
// Created by kelvin34501 on 7/24/20.
//

#include "custom_game_state.h"
#include "GraphicsSystem.h"
#include "OgreSceneManager.h"
#include "OgreItem.h"
#include "OgreTextAreaOverlayElement.h"

namespace Tutorial {
    extern int current_frame_time_idx;
    extern bool fake_slowmotion;
}

using namespace Tutorial;

CustomGameState::CustomGameState(const Ogre::String &help_desc) : TutorialGameState(help_desc), scene_node(nullptr),
                                                                  displacement(0) {

}

void CustomGameState::createScene01() {
    Ogre::SceneManager *scene_manager = mGraphicsSystem->getSceneManager();
    Ogre::Item *item = scene_manager->createItem("Cube_d.mesh",
                                                 Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
                                                 Ogre::SCENE_DYNAMIC);
    scene_node = scene_manager->getRootSceneNode(Ogre::SCENE_DYNAMIC)->createChildSceneNode(Ogre::SCENE_DYNAMIC);
    scene_node->attachObject(item);

    TutorialGameState::createScene01();
}

void CustomGameState::update(float time_since_last) {
    const Ogre::Vector3 origin(-5.0f, 0.0f, 0.0f);

    displacement += time_since_last * 4.0f;
    displacement = fmodf(displacement, 10.0f);
    scene_node->setPosition(origin + Ogre::Vector3::UNIT_X * displacement);

    TutorialGameState::update(time_since_last);
}

void CustomGameState::keyReleased(const SDL_KeyboardEvent &arg) {
    if ((arg.keysym.mod & ~(KMOD_NUM | KMOD_CAPS)) != 0) {
        TutorialGameState::keyReleased(arg);
        return;
    }

    if (arg.keysym.sym == SDLK_F2) {
        current_frame_time_idx = !current_frame_time_idx;
    }
    if (arg.keysym.sym == SDLK_F3) {
        fake_slowmotion = !fake_slowmotion;
    } else {
        TutorialGameState::keyReleased(arg);
    }
}

void CustomGameState::generateDebugText(float time_since_last, Ogre::String &out_text) {
    TutorialGameState::generateDebugText(time_since_last, out_text);

    out_text += "\nPress F2 to toggle fixed framerate";
    out_text += "\nPress F3 to fake slow motion ";
    out_text += fake_slowmotion ? "[ON]" : "[OFF]";
}
