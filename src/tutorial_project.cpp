//
// Created by kelvin34501 on 7/23/20.
//

#include "GraphicsSystem.h"
#include "GameState.h"

#include "OgreWindow.h"
#include "OgreTimer.h"

#include "Threading/OgreThreads.h"
#include "Threading/YieldTimer.h"

// declares WinMain / main
#include "MainEntryPointHelper.h"
#include "System/MainEntryPoints.h"

// files within project
#include "custom_game_state.h"

// Demo is the namespace of OgreCommon
// only using it in this file
using namespace Demo;

namespace Tutorial {
    class TutorialGraphicsSystem : public GraphicsSystem {
        void setupResources() override {}

    public:
        explicit TutorialGraphicsSystem(GameState *game_state) : GraphicsSystem(game_state) {}
    };

    extern const double frame_time[2];
    const double frame_time[2] = {1.0 / 25.0, 1.0 / 60.0};

    extern int current_frame_time_idx;
    int current_frame_time_idx = 0;

    extern bool fake_slowmotion;
    bool fake_slowmotion = false;
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
            "This tutorial is very similar to Tutorial02; however it uses a fixed framerate instead\n"
            "of a variable one. This means timeSinceLast will always be 1/25 (or whatever value you\n"
            "set); and the update will always be capped at 25hz / 25 FPS.\n"
            "We chose a value of 25hz to demonstrate the big difference. A value of 60hz will look\n"
            "smooth.\n"
            "\n"
            "There are many reasons to using a fixed framerate instead of a variable one:\n"
            " * It is more stable. High framerates (i.e. 10000 fps) cause floating point precision\n"
            "   issues in 'timeSinceLast' as it becomes very small. The value may even eventually\n"
            "   round to 0!\n"
            " * Physics stability, physics and logic simulations don't like variable framerate.\n"
            " * Determinism (given the same input, every run of the program will always return the\n"
            "  same output). Variable framerate and determinism don't play together.\n"
            "\n"
            "This also means that if your CPU/GPU aren't fast enough to render the frame in\n"
            "40ms, the game simulation will start looking in 'slow motion'; unlike variable\n"
            "framerate where the speed is maintained, but it looks a bit more 'frame skippy'.\n"
            "\n"
            "If the frame was rendered in less time than 40ms; The 'YieldTimer' class will tell\n"
            "the OS to yield this thread to give a chance to other threads and processes to process\n"
            "their own stuff.\n"
            "Ideally we would call sleep; but on many OSes (particularly Windows!) the granularity\n"
            "of sleep is very low: calling Sleep(1) may end up sleeping 15ms!!!\n"
            "\n"
            "Notice that the slow motion problem (when CPU/GPU can't cope with our app) is much more\n"
            "noticeable when rendering at 60hz than at 25hz\n"
            "\n");
    GraphicsSystem graphics_system(&custom_game_state);
    custom_game_state._notifyGraphicsSystem(&graphics_system);

    graphics_system.initialize("Tutorial 03");
    if (graphics_system.getQuit()) {
        graphics_system.deinitialize();
        return 0;
    }

    Ogre::Window *render_window = graphics_system.getRenderWindow();
    graphics_system.createScene01();
    graphics_system.createScene02();

    Ogre::Timer timer;
    YieldTimer yield_timer(&timer);
    Ogre::uint64 start_time = timer.getMicroseconds();

    while (!graphics_system.getQuit()) {
        graphics_system.beginFrameParallel();
        graphics_system.update(static_cast<float>(Tutorial::frame_time[Tutorial::current_frame_time_idx]));
        graphics_system.finishFrameParallel();
        graphics_system.finishFrame();

        if (!render_window->isVisible()) {
            Ogre::Threads::Sleep(500);
        }

        if (Tutorial::fake_slowmotion) {
            Ogre::Threads::Sleep(50);
        }

        start_time = yield_timer.yield(Tutorial::frame_time[Tutorial::current_frame_time_idx], start_time);
    }

    graphics_system.destroyScene();
    graphics_system.deinitialize();

    return 0;
}
