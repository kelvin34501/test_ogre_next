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

// platform specific headers
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <errno.h>

#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "shlobj.h"
#endif


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
            "This is an advanced multithreading tutorial. For the simple version see Tutorial 05\n"
            "We introduce the 'GameEntity' structure which encapsulates a game object data:\n"
            "It contains its graphics (i.e. Entity and SceneNode) and its physics/logic data\n"
            "(a transform, the hkpEntity/btRigidBody pointers, etc)\n"
            "GameEntity is created via the GameEntityManager; which is responsible for telling\n"
            "the render thread to create the graphics; and delays deleting the GameEntity until\n"
            "all threads are done using it.\n"
            "Position/Rot./Scale is updated via a ring buffer, which ensures that the Logic \n"
            "thread is never writing to the transforms while being read by the Render thread\n"
            "You could gain some performance and memory by purposedly not caring and leaving\n"
            "a race condition (Render thread reading the transforms while Logic may be \n"
            "updating it) if you don't mind a very occasional flickering.\n"
            "\n"
            "The Logic thread is in charge of simulating the transforms and ultimately, updating\n"
            "the transforms.\n"
            "The Render thread is in charge of interpolating these transforms and leaving the \n"
            "rendering to Ogre (culling, updating the scene graph, skeletal animations, rendering)\n"
            "\n"
            "Render-split multithreaded rendering is very powerful and scales well to two cores\n"
            "but it requires a different way of thinking. You don't directly create Ogre objects.\n"
            "You request them via messages that need first to bake all necesary information (do \n"
            "you want an Entity, an Item, a particle FX?), and they get created asynchronously.\n"
            "A unit may be created in a logic frame, but may still not be rendered yet, and may\n"
            "take one or two render frames to appear on screen.\n"
            "\n"
            "Skeletal animation is not covered by this tutorial, but the same principle applies.\n"
            "First define a few baked structures about the animations you want to use, and then\n"
            "send messages for synchronizing it (i.e. play X animation, jump to time Y, set blend\n"
            "weight Z, etc)");
    Demo::GraphicsSystem graphics_system(&graphics_game_state);
    Tutorial::LogicGameState logic_game_state;
    Demo::LogicSystem logic_system(&logic_game_state);
    Ogre::Barrier barrier(2);

    graphics_game_state._notifyGraphicsSystem(&graphics_system);
    logic_game_state._notifyLogicSystem(&logic_system);

    graphics_system._notifyLogicSystem(&logic_system);
    logic_system._notifyGraphicsSystem(&graphics_system);

    Demo::GameEntityManager game_entity_manager(&graphics_system, &logic_system);

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
        std::cerr << "exception occurred: " << e.getFullDescription().c_str() << std::endl;
#endif
        abort();
    }
    return res;
}

unsigned long Tutorial::render_thread_app(Ogre::ThreadHandle *thread_handle) {
    auto *thread_data = reinterpret_cast<ThreadData *>(thread_handle->getUserParam());
    Demo::GraphicsSystem *graphics_system = thread_data->graphics_system;
    Ogre::Barrier *barrier = thread_data->barrier;

    graphics_system->initialize("Tutorial 06");
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
