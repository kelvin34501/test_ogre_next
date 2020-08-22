//
// Created by kelvin34501 on 7/24/20.
//

#ifndef TESTOGRENEXT_GRAPHICS_GAME_STATE_H
#define TESTOGRENEXT_GRAPHICS_GAME_STATE_H

#include <OgreVector3.h>
#include <OgrePrerequisites.h>
#include <TutorialGameState.h>

namespace Tutorial {
    class GraphicsGameState : public Demo::TutorialGameState {
        bool enable_interpolation;

        void generateDebugText(float time_since_last, Ogre::String &out_text) override;

    public:
        explicit GraphicsGameState(const Ogre::String &help_desc);


        void update(float time_since_last) override;

        void keyReleased(const SDL_KeyboardEvent &arg) override;
    };
}


#endif //TESTOGRENEXT_GRAPHICS_GAME_STATE_H
