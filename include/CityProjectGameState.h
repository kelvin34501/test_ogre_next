
#ifndef _Demo_EmptyProjectGameState_H_
#define _Demo_EmptyProjectGameState_H_

#include "OgrePrerequisites.h"
#include "TutorialGameState.h"

namespace Demo {
    class CityProjectGameState : public TutorialGameState {
        void generateDebugText(float timeSinceLast, Ogre::String &outText) override;

    public:
        explicit CityProjectGameState(const Ogre::String &helpDescription);

        void createScene01() override;

        void update(float timeSinceLast) override;

        void keyReleased(const SDL_KeyboardEvent &arg) override;
    };
}

#endif
