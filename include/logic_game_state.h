//
// Created by kelvin34501 on 7/25/20.
//

#ifndef TESTOGRENEXT_LOGIC_GAME_STATE_H
#define TESTOGRENEXT_LOGIC_GAME_STATE_H

#include <OgrePrerequisites.h>
#include <TutorialGameState.h>
#include <GameEntity.h>
#include <LogicSystem.h>

namespace Tutorial {
    class LogicGameState : public Demo::GameState {
        float displacement;
        Demo::GameEntity *cube_entity;
        Demo::MovableObjectDefinition *cube_motion_def;

        Demo::LogicSystem *logic_system;
    public:
        LogicGameState();

        ~LogicGameState() override;

        void _notifyLogicSystem(Demo::LogicSystem *incoming_logic_system);

        void createScene01() override;

        void update(float time_since_last) override;
    };
}

#endif //TESTOGRENEXT_LOGIC_GAME_STATE_H
