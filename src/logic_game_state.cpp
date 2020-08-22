//
// Created by kelvin34501 on 7/25/20.
//

#include "logic_game_state.h"
#include <GameEntityManager.h>
#include <OgreResourceGroupManager.h>

using namespace Tutorial;

LogicGameState::LogicGameState() :
        displacement(0),
        cube_entity(nullptr),
        cube_motion_def(nullptr),
        logic_system(nullptr) {
}

LogicGameState::~LogicGameState() {
    delete cube_motion_def;
    cube_motion_def = nullptr;
}


void LogicGameState::createScene01() {
    const Ogre::Vector3 origin(-5.0f, 0.0f, 0.0f);
    Demo::GameEntityManager *game_entity_manager = logic_system->getGameEntityManager();

    cube_motion_def = new Demo::MovableObjectDefinition();
    cube_motion_def->meshName = "Cube_d.mesh";
    cube_motion_def->resourceGroup = Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME;
    cube_motion_def->moType = Demo::MoTypeItem;

    cube_entity = game_entity_manager->addGameEntity(
            Ogre::SCENE_DYNAMIC,
            cube_motion_def,
            origin,
            Ogre::Quaternion::IDENTITY,
            Ogre::Vector3::UNIT_SCALE
    );
}


void LogicGameState::update(float time_since_last) {
    const Ogre::Vector3 origin(-5.0f, 0.0f, 0.0f);
    displacement += time_since_last * 4.0f;
    displacement = fmodf(displacement, 10.0f);

    const size_t curr_idx = logic_system->getCurrentTransformIdx();
//    const size_t prev_idx = (curr_idx + NUM_GAME_ENTITY_BUFFERS - 1) % NUM_GAME_ENTITY_BUFFERS;
    cube_entity->mTransform[curr_idx]->vPos = origin + Ogre::Vector3::UNIT_X * displacement;

    // following code will read last set position and update it to new buffer
    // graphics will be reading cube_entity->mTransform[prev_idx]
    // as long as we don't write to it, we are ok
//    cube_entity->mTransform[curr_idx]->vPos = cube_entity->mTransform[prev_idx]->vPos
//                                              + Ogre::Vector3::UNIT_X * time_since_last;

    GameState::update(time_since_last);
}

void LogicGameState::_notifyLogicSystem(Demo::LogicSystem *incoming_logic_system) {
    this->logic_system = incoming_logic_system;
}


