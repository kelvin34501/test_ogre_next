//
// Created by kelvin34501 on 7/23/20.
//

#include "GraphicsSystem.h"
#include "GameState.h"
#include "custom_game_state.h"

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
        auto *gfxGameState = new Tutorial::CustomGameState("");
        auto *graphicsSystem = new GraphicsSystem(gfxGameState);

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
    Tutorial::CustomGameState custom_game_state(
            "This tutorial demonstrates the most basic rendering loop: Variable framerate.\n"
            "Variable framerate means the application adapts to the current frame rendering\n"
            "performance and boosts or decreases the movement speed of objects to maintain\n"
            "the appearance that objects are moving at a constant velocity.\n"
            "When framerate is low, it looks 'frame skippy'; when framerate is high,\n"
            "it looks very smooth.\n"
            "Note: If you can't exceed 60 FPS, it's probably because of VSync being turned on.\n"
            "\n"
            "Despite what it seems, this is the most basic form of updating, and a horrible way\n"
            "to update your objects if you want to do any kind of serious game development.\n"
            "Keep going through the Tutorials for superior methods of updating the rendering loop.\n"
            "\n"
            "Note: The cube is black because there is no lighting. We are not focusing on that.");
    GraphicsSystem graphics_system(&custom_game_state);
    custom_game_state._notifyGraphicsSystem(&graphics_system);

    graphics_system.initialize("Tutorial 02");
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
