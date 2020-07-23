//
// Created by kelvin34501 on 7/23/20.
//

#include "GraphicsSystem.h"
#include "GameState.h"

#include "OgreWindow.h"
#include "OgreTimer.h"

#include "Threading/OgreThreads.h"

// declares WinMain / main
#include "MainEntryPointHelper.h"
#include "System/MainEntryPoints.h"

// Demo is the namespace of OgreCommon
// only using it in this file
using namespace Demo;

namespace Tutorial {
    class TutorialGraphicsSystem : public GraphicsSystem {
        void setupResources() override {}

    public:
        explicit TutorialGraphicsSystem(GameState *game_state) : GraphicsSystem(game_state) {}
    };
}

namespace Demo {
    void MainEntryPoints::createSystems(GameState **outGraphicsGameState,
                                        GraphicsSystem **outGraphicsSystem,
                                        GameState **outLogicGameState,
                                        LogicSystem **outLogicSystem) {
        GameState *gfxGameState = new GameState();
        GraphicsSystem *graphicsSystem = new Tutorial::TutorialGraphicsSystem(gfxGameState);

        *outGraphicsGameState = gfxGameState;
        *outGraphicsSystem = graphicsSystem;
    }

    void MainEntryPoints::destroySystems(GameState *graphicsGameState,
                                         GraphicsSystem *graphicsSystem,
                                         GameState *logicGameState,
                                         LogicSystem *logicSystem) {
        delete graphicsSystem;
        delete graphicsGameState;
    }

    const char *MainEntryPoints::getWindowTitle() {
        return "";
    }
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMainApp( HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR strCmdLine, INT nCmdShow )
#else

int mainApp(int argc, const char *argv[])
#endif
{
    GameState game_state;
    Tutorial::TutorialGraphicsSystem graphics_system(&game_state);

    graphics_system.initialize("Tutorial 01");
    if (graphics_system.getQuit()) {
        graphics_system.deinitialize();
        return 0;
    }

    Ogre::Window *render_window = graphics_system.getRenderWindow();
    graphics_system.createScene01();
    graphics_system.createScene02();

    Ogre::Timer timer;
    Ogre::uint64 start_time = timer.getMicroseconds();
    double time_since_last = 1.0 / 60.0;

    while (!graphics_system.getQuit()) {
        graphics_system.beginFrameParallel();
        graphics_system.update(static_cast<float>(time_since_last));
        graphics_system.finishFrameParallel();
        graphics_system.finishFrame();

        if (!render_window->isVisible()) {
            Ogre::Threads::Sleep(500);
        }

        Ogre::uint64 end_time = timer.getMicroseconds();
        time_since_last = (end_time - start_time) / 1000000.0;
        time_since_last = std::min(1.0, time_since_last);
        start_time = end_time;
    }

    graphics_system.destroyScene();
    graphics_system.deinitialize();

    return 0;
}
