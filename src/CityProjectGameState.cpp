
#include "CityProjectGameState.h"
#include "CameraController.h"
#include "GraphicsSystem.h"

#include "OgreSceneManager.h"
#include "OgreItem.h"

#include "OgreMeshManager.h"
#include "OgreMeshManager2.h"
#include "OgreMesh2.h"

#include "OgreCamera.h"
#include "OgreRenderWindow.h"

#include "OgreHlmsPbsDatablock.h"
#include "OgreHlmsSamplerblock.h"

#include "OgreRoot.h"
#include "OgreHlmsManager.h"
#include "OgreHlmsTextureManager.h"
#include "OgreHlmsPbs.h"

using namespace Demo;

namespace Demo {
    CityProjectGameState::CityProjectGameState(const Ogre::String &helpDescription) :
            TutorialGameState(helpDescription) {
    }

    //-----------------------------------------------------------------------------------
    void CityProjectGameState::createScene01(void) {
        mCameraController = new CameraController(mGraphicsSystem, false);

        TutorialGameState::createScene01();
    }

    //-----------------------------------------------------------------------------------
    void CityProjectGameState::update(float timeSinceLast) {
        TutorialGameState::update(timeSinceLast);
    }

    //-----------------------------------------------------------------------------------
    void CityProjectGameState::generateDebugText(float timeSinceLast, Ogre::String &outText) {
        TutorialGameState::generateDebugText(timeSinceLast, outText);
    }

    //-----------------------------------------------------------------------------------
    void CityProjectGameState::keyReleased(const SDL_KeyboardEvent &arg) {
        if ((arg.keysym.mod & ~(KMOD_NUM | KMOD_CAPS)) != 0) {
            TutorialGameState::keyReleased(arg);
            return;
        }

        TutorialGameState::keyReleased(arg);
    }
}
