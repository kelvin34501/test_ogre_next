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
#include <Threading/OgreBarrier.h>

// declares WinMain / main
#include <MainEntryPointHelper.h>
#include <System/MainEntryPoints.h>

// files within project
#include "graphics_game_state.h"
#include "logic_game_state.h"


namespace Tutorial {
    class TutorialGraphicsSystem : public Demo::GraphicsSystem {
        void setupResources() override {}

    public:
        explicit TutorialGraphicsSystem(Demo::GameState *game_state) : GraphicsSystem(game_state) {}
    };

    extern const double frame_time;
    const double frame_time = 1.0 / 25.0;

    extern bool fake_slowmotion;
    bool fake_slowmotion = false;

    extern bool fake_frame_skip;
    bool fake_frame_skip = false;

    unsigned long render_thread_app(Ogre::ThreadHandle *thread_handle);

    unsigned long render_thread(Ogre::ThreadHandle *thread_handle);

    unsigned long logic_thread(Ogre::ThreadHandle *thread_handle);

    THREAD_DECLARE(render_thread);

    THREAD_DECLARE(logic_thread);

    struct ThreadData {
        Demo::GraphicsSystem *graphics_system;
        Demo::LogicSystem *logic_system;
        Ogre::Barrier *barrier;
    };
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
            "This tutorial shows how to setup two update loops: One for Graphics, another for\n"
            "Logic, each in its own thread. We don't render anything because we will now need\n"
            "to do a robust synchronization for creating, destroying and updating Entities,\n"
            "which is too complex to show in just one tutorial step.\n"
            "\n"
            "The key concept is that Graphic's createScene01 runs in parallel to Logic's\n"
            "createScene01. But we guarantee that createScene02 will be called after all\n"
            "createScene01s have been called. In other words, createScene is divided in\n"
            "two stages and each stage runs in parallel.\n"
            "\n"
            "This means that Logic will be creating the entities in stage 01; and Graphics\n"
            "will see the request to create the Ogre objects (e.g. Item, SceneNode) in\n"
            "stage 02. Meanwhile Graphics may dedicate the time in stage 01 to preload some\n"
            "meshes, overlays, and other resources that will always be needed.\n"
            "Logic in stage 02 will be idle, so it may dedicate that time to load non-\n"
            "graphics related data (like physics representations).\n");
    Demo::GraphicsSystem graphics_system(&graphics_game_state);
//    Tutorial::LogicGameState logic_game_state;
    Demo::GameState logic_game_state; //Dummy
    Demo::LogicSystem logic_system(&logic_game_state);
    Ogre::Barrier barrier(2);

//    logic_game_state._notifyGraphicsGameState(&graphics_game_state);
    graphics_game_state._notifyGraphicsSystem(&graphics_system);

    graphics_system._notifyLogicSystem(&logic_system);
    logic_system._notifyGraphicsSystem(&graphics_system);

    Tutorial::ThreadData thread_data{};
    thread_data.graphics_system = &graphics_system;
    thread_data.logic_system = &logic_system;
    thread_data.barrier = &barrier;

    Ogre::ThreadHandlePtr thread_handles[2];
    thread_handles[0] = Ogre::Threads::CreateThread(
            THREAD_GET(Tutorial::render_thread), 0, &thread_data
    );
    thread_handles[1] = Ogre::Threads::CreateThread(
            THREAD_GET(Tutorial::logic_thread), 1, &thread_data
    );
    Ogre::Threads::WaitForThreads(2, thread_handles);

    return 0;
}

// ====== RENDER THREAD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
unsigned long Tutorial::render_thread(Ogre::ThreadHandle *thread_handle) {
    unsigned long res = -1;
    try {
        res = Tutorial::render_thread_app(thread_handle);
    } catch (Ogre::Exception &e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        Demo::MessageBoxA(nullptr, e.getFullDescription().c_str(), "An exception has occured!",
                       MB_OK | MB_ICONERROR | MB_TASKMODAL );
#else
        std::cerr << "exception occured: " << e.getFullDescription().c_str() << std::endl;
#endif
        abort();
    }
    return res;
}

unsigned long Tutorial::render_thread_app(Ogre::ThreadHandle *thread_handle) {
    auto *thread_data = reinterpret_cast<ThreadData *>(thread_handle->getUserParam());
    Demo::GraphicsSystem *graphics_system = thread_data->graphics_system;
    Ogre::Barrier *barrier = thread_data->barrier;

    graphics_system->initialize("Tutorial 05");
    barrier->sync();

    if (graphics_system->getQuit()) {
        graphics_system->deinitialize();
        return 0; // user canceled config
    }

    graphics_system->createScene01();
    barrier->sync();

    graphics_system->createScene02();
    barrier->sync();

    Ogre::Window *render_window = graphics_system->getRenderWindow();

    Ogre::Timer timer;
    Ogre::uint64 start_time = timer.getMicroseconds();

    double time_since_last = 1.0 / 60.0;
    while (!graphics_system->getQuit()) {
        graphics_system->beginFrameParallel();
        graphics_system->update(static_cast<float>(time_since_last));
        graphics_system->finishFrameParallel();
        if (!render_window->isVisible()) {
            Ogre::Threads::Sleep(500);
        }
        if (Tutorial::fake_frame_skip) {
            Ogre::Threads::Sleep(120);
        }
        Ogre::uint64 end_time = timer.getMicroseconds();
        time_since_last = static_cast<double>(end_time - start_time) / 1000000.0;
        time_since_last = std::min(1.0, time_since_last);
        start_time = end_time;
    }
    barrier->sync();

    graphics_system->destroyScene();
    barrier->sync();

    graphics_system->deinitialize();
    barrier->sync();

    return 0;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


// ====== LOGIC THREAD >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
unsigned long Tutorial::logic_thread(Ogre::ThreadHandle *thread_handle) {
    auto *thread_data = reinterpret_cast<Tutorial::ThreadData *>(thread_handle->getUserParam());
    Demo::GraphicsSystem *graphics_system = thread_data->graphics_system;
    Demo::LogicSystem *logic_system = thread_data->logic_system;
    Ogre::Barrier *barrier = thread_data->barrier;

    logic_system->initialize();
    barrier->sync();

    if (graphics_system->getQuit()) {
        logic_system->deinitialize();
        return 0;
    }

    logic_system->createScene01();
    barrier->sync();

    logic_system->createScene02();
    barrier->sync();

    Ogre::Window *render_window = graphics_system->getRenderWindow();

    Ogre::Timer timer;
    Demo::YieldTimer yield_timer(&timer);

    Ogre::uint64 start_time = timer.getMicroseconds();
    while (!graphics_system->getQuit()) {
        logic_system->beginFrameParallel();
        logic_system->update(static_cast<float>(Tutorial::frame_time));
        logic_system->finishFrameParallel();

        logic_system->finishFrame();

        if (!render_window->isVisible()) {
            Ogre::Threads::Sleep(500);
        }
        if (Tutorial::fake_slowmotion) {
            Ogre::Threads::Sleep(120);
        }

        start_time = yield_timer.yield(Tutorial::frame_time, start_time);
    }
    barrier->sync();

    logic_system->destroyScene();
    barrier->sync();

    logic_system->deinitialize();
    barrier->sync();

    return 0;
}
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
