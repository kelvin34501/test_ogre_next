//
// Created by kelvin34501 on 7/25/20.
//

#include "logic_game_state.h"

using namespace Tutorial;

LogicGameState::LogicGameState() : displacement(0), graphics_game_state(nullptr) {
}

void LogicGameState::_notifyGraphicsGameState(GraphicsGameState *incoming_graphics_game_state) {
    this->graphics_game_state = incoming_graphics_game_state;
}

//void LogicGameState::update(float time_since_last) {
//    const Ogre::Vector3 origin(-5.0f, 0.0f, 0.0f);
//    displacement += time_since_last * 4.0f;
//    displacement = fmodf(displacement, 10.0f);
//
//    Ogre::Vector3 &last_pos = graphics_game_state->get_last_position_ref();
//    last_pos = graphics_game_state->get_curr_position_ref();
//    graphics_game_state->get_curr_position_ref() = origin + Ogre::Vector3::UNIT_X * displacement;
//
//    GameState::update(time_since_last);
//}
//

