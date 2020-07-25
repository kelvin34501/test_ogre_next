//
// Created by kelvin34501 on 7/23/20.
//

#include <GraphicsSystem.h>
#include <GameState.h>
#include <LogicSystem.h>

#include <OgreWindow.h>
#include <OgreTimer.h>

#include <Threading/OgreThreads.h>
#include <Threading/YieldTimer.h>

// declares WinMain / main
#include <MainEntryPointHelper.h>
#include <System/MainEntryPoints.h>

// files within project
#include "graphics_game_state.h"
#include "logic_game_state.h"

// Demo is the namespace of OgreCommon
// only using it in this file
using namespace Demo;

namespace Tutorial {
    class TutorialGraphicsSystem : public GraphicsSystem {
        void setupResources() override {}

    public:
        explicit TutorialGraphicsSystem(GameState *game_state) : GraphicsSystem(game_state) {}
    };

    extern const double frame_time;
    const double frame_time = 1.0 / 25.0;

    extern bool fake_slowmotion;
    bool fake_slowmotion = false;

    extern bool fake_frame_skip;
    bool fake_frame_skip = false;
}

namespace Demo {
    void MainEntryPoints::createSystems(GameState **outGraphicsGameState,
                                        GraphicsSystem **outGraphicsSystem,
                                        GameState **outLogicGameState,
                                        LogicSystem **outLogicSystem) {
    }

    void MainEntryPoints::destroySystems(GameState *graphicsGameState,
                                         GraphicsSystem *graphicsSystem,
                                         GameState *logicGameState,
                                         LogicSystem *logicSystem) {
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
    Tutorial::GraphicsGameState graphics_game_state(
            "This tutorial combines fixed and variable framerate: Logic is executed at 25hz, while\n"
            "graphics are being rendered at a variable rate, interpolating between frames to\n"
            "achieve a smooth result.\n"
            "When OGRE or the GPU is taking too long, you will see a 'frame skip' effect, when\n"
            "the CPU is taking too long to process the Logic code, you will see a 'slow motion'\n"
            "effect.\n"
            "This combines the best of both worlds and is the recommended approach for serious\n"
            "game development.\n"
            "\n"
            "The only two disadvantages from this technique are:\n"
            " * We introduce 1 frame of latency.\n"
            " * Teleporting may be shown as very fast movement; as the graphics state will try to\n"
            "   blend between the last and current position. This can be solved though, by writing\n"
            "   to both the previous and current position in case of teleportation. We purposedly\n"
            "   don't do this to show the effect/'glitch'.\n"
            "\n"
            "This approach needs to copy all the transform data from the logic side to the\n"
            "graphics side (i.e. Ogre SceneNodes). This is very common however, since a physics\n"
            "engine will use its own data structures to store its transforms (i.e. Bullet, Havok,\n"
            "ODE, PhysX)\n"
            "\n"
            "The next Tutorials will show how to run the logic and physics in its own thread, while\n"
            "OGRE renders in its own thread.");
    GraphicsSystem graphics_system(&graphics_game_state);
    Tutorial::LogicGameState logic_game_state;
    LogicSystem logic_system(&logic_game_state);

    logic_game_state._notifyGraphicsGameState(&graphics_game_state);
    graphics_game_state._notifyGraphicsSystem(&graphics_system);

    graphics_system._notifyLogicSystem(&logic_system);
    logic_system._notifyGraphicsSystem(&graphics_system);

    graphics_system.initialize("Tutorial 04");
    logic_system.initialize();
    if (graphics_system.getQuit()) {
        logic_system.deinitialize();
        graphics_system.deinitialize();
        return 0;
    }

    Ogre::Window *render_window = graphics_system.getRenderWindow();
    graphics_system.createScene01();
    logic_system.createScene01();
    graphics_system.createScene02();
    logic_system.createScene02();

    Ogre::Timer timer;
    //YieldTimer yield_timer(&timer);
    Ogre::uint64 start_time = timer.getMicroseconds();
    double accumulator = Tutorial::frame_time;
    double time_since_last = 1.0 / 60.0;

    while (!graphics_system.getQuit()) {
        while (accumulator >= Tutorial::frame_time) {
            graphics_system.beginFrameParallel();

            logic_system.beginFrameParallel();
            logic_system.update(static_cast<float>(Tutorial::frame_time));
            logic_system.finishFrameParallel();

            graphics_system.finishFrameParallel();

            logic_system.finishFrame();
            graphics_system.finishFrame();

            accumulator -= Tutorial::frame_time;

            if (Tutorial::fake_slowmotion) {
                Ogre::Threads::Sleep(40);
            }
        }

        graphics_system.beginFrameParallel();
        graphics_system.update(time_since_last);
        graphics_system.finishFrameParallel();

        if (!render_window->isVisible()) {
            Ogre::Threads::Sleep(500);
        }

        if (Tutorial::fake_frame_skip) {
            Ogre::Threads::Sleep(40);
        }

        //start_time = yield_timer.yield(Tutorial::frame_time[Tutorial::current_frame_time_idx], start_time);
        Ogre::uint64 end_time = timer.getMicroseconds();
        time_since_last = (end_time - start_time) / 1000000.0f;
        time_since_last = std::min(1.0, time_since_last);
        accumulator += time_since_last;
        start_time = end_time;
    }

    graphics_system.destroyScene();
    logic_system.deinitialize();
    logic_system.deinitialize();
    graphics_system.deinitialize();

    return 0;
}
