//
// Created by kelvin34501 on 7/24/20.
//

#ifndef TESTOGRENEXT_CUSTOM_GAME_STATE_H
#define TESTOGRENEXT_CUSTOM_GAME_STATE_H

#include "OgrePrerequisites.h"
#include "TutorialGameState.h"

namespace Tutorial {
    class CustomGameState : public Demo::TutorialGameState {
        Ogre::SceneNode *scene_node;
        float displacement;

        void generateDebugText(float time_since_last, Ogre::String &out_text) override;

    public:
        explicit CustomGameState(const Ogre::String &help_desc);

        void createScene01() override;

        void update(float time_since_last) override;

        void keyReleased(const SDL_KeyboardEvent &arg) override;
    };
}


#endif //TESTOGRENEXT_CUSTOM_GAME_STATE_H
