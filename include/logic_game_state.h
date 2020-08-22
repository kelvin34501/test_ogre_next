//
// Created by kelvin34501 on 7/25/20.
//

#ifndef TESTOGRENEXT_LOGIC_GAME_STATE_H
#define TESTOGRENEXT_LOGIC_GAME_STATE_H

#include <OgrePrerequisites.h>
#include <TutorialGameState.h>
#include "graphics_game_state.h"

namespace Tutorial {
    class LogicGameState : public Demo::GameState {
        float displacement;
        GraphicsGameState *graphics_game_state;
    public:
        LogicGameState();

        void _notifyGraphicsGameState(GraphicsGameState *incoming_graphics_game_state);

//        virtual void update(float time_since_last);
    };
}

#endif //TESTOGRENEXT_LOGIC_GAME_STATE_H
