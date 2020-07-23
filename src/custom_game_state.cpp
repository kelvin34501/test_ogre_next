//
// Created by kelvin34501 on 7/24/20.
//

#include "custom_game_state.h"
#include "GraphicsSystem.h"
#include "OgreSceneManager.h"
#include "OgreItem.h"
#include "OgreTextAreaOverlayElement.h"

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
