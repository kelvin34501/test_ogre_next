//
// Created by kelvin34501 on 7/23/20.
//

#include "OgreArchiveManager.h"
#include "OgreCamera.h"
#include "OgreConfigFile.h"
#include "OgreRoot.h"
#include "OgreWindow.h"

#include "OgreHlmsManager.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsUnlit.h"

#include "Compositor/OgreCompositorManager2.h"

#include "OgreWindowEventUtilities.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "OSX/macUtils.h"
#endif

static void register_hlms() {
    using namespace Ogre;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    const String resource_path = Ogre::macBundlePath() + "/Contents/Resources/";
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    const String resource_path = Ogre::macBundlePath() + "/";
#else
    String resource_path;
#endif

    ConfigFile cf;
    cf.load(resource_path + "resources2.cfg");

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    String root_hlms_dir = macBundlePath() + '/' + cf.getSetting( "DoNotUseAsResource", "Hlms", "" );
#else
    String root_hlms_dir = resource_path + cf.getSetting("DoNotUseAsResource", "Hlms", "");
#endif

    if (root_hlms_dir.empty()) {
        root_hlms_dir = "./";
    } else if (*(root_hlms_dir.end() - 1) != '/') {
        root_hlms_dir += "/";
    }
    // at this point root_hlms_dir should be a valid path to the Hlms data folder

    HlmsUnlit *hlms_unlit = nullptr;
    HlmsPbs *hlms_pbs = nullptr;

    // for retrieval of the paths to the different folders needed
    String main_dir_path;
    StringVector library_dir_path_vec;

    ArchiveManager &archive_manager = ArchiveManager::getSingleton();

    {
        // create & register HlmsUnlit
        // get the path to all the subdirectories used by HlmsUnlit
        HlmsUnlit::getDefaultPaths(main_dir_path, library_dir_path_vec);
        Archive *archive_unlit = archive_manager.load(root_hlms_dir + main_dir_path, "FileSystem", true);
        ArchiveVec archive_unlit_library_dirs;
        for (const auto &library_dir_path: library_dir_path_vec) {
            Archive *archive_library = archive_manager.load(root_hlms_dir + library_dir_path, "FileSystem", true);
            archive_unlit_library_dirs.push_back(archive_library);
        }

        // create and register the unlit Hlms
        hlms_unlit = OGRE_NEW HlmsUnlit(archive_unlit, &archive_unlit_library_dirs);
        Root::getSingleton().getHlmsManager()->registerHlms(hlms_unlit);
    }

    {
        // create & register HlmsPbs
        // do the same for HlmsPbs
        HlmsPbs::getDefaultPaths(main_dir_path, library_dir_path_vec);
        Archive *archive_pbs = archive_manager.load(root_hlms_dir + main_dir_path, "FileSystem", true);
        ArchiveVec archive_pbs_library_dirs;
        for (const auto &library_dir_path: library_dir_path_vec) {
            Archive *archive_library = archive_manager.load(root_hlms_dir + library_dir_path, "FileSystem", true);
            archive_pbs_library_dirs.push_back(archive_library);
        }

        // create and register
        hlms_pbs = OGRE_NEW HlmsPbs(archive_pbs, &archive_pbs_library_dirs);
        Root::getSingleton().getHlmsManager()->registerHlms(hlms_pbs);
    }

    RenderSystem *render_system = Root::getSingletonPtr()->getRenderSystem();
    if (render_system->getName() == "Direct3D11 Rendering Subsystem") {
        // set lower limits 512kb instead of the default 4MB per Hlms in D3D 11.0
        // and below to avoid saturating AMD's discard limit (8MB) or
        // saturate the PCIE bus in some low end machines.
        bool support_no_overwrite_on_texture_buffers;
        render_system->getCustomAttribute("MapNoOverwriteOnDynamicBufferSRV", &support_no_overwrite_on_texture_buffers);
        if (!support_no_overwrite_on_texture_buffers) {
            hlms_pbs->setTextureBufferDefaultSize(512 * 1024);
            hlms_unlit->setTextureBufferDefaultSize(512 * 1024);
        }
    }
}

class MyWindowEventListener : public Ogre::WindowEventListener {
    bool m_quit;
public:
    MyWindowEventListener() : m_quit(false) {}

    void windowClosed(Ogre::Window *rw) override { m_quit = true; }

    [[nodiscard]] bool get_quit() const { return m_quit; }
};

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR strCmdLine, INT nCmdShow)
#else

int main(int argc, const char *argv[])
#endif
{
    using namespace Ogre;

    const String plugins_folder = "./";
    const String write_access_folder = "./";

#ifndef OGRE_STATIC_LIB
#if OGRE_DEBUG_MODE && !((OGRE_PLATFORM == OGRE_PLATFORM_APPLE) || (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS))
    const char *plugins_file = "plugins_d.cfg";
#else
    const char *plugins_file = "plugins.cfg";
#endif
#endif

    Root *root = OGRE_NEW Root(plugins_folder + plugins_file,
                               write_access_folder + "ogre.cfg",
                               write_access_folder + "Ogre.log");
    if (!root->showConfigDialog()) {
        return -1;
    }

    // initialize root
    root->getRenderSystem()->setConfigOption("Full Screen", "No");
    root->getRenderSystem()->setConfigOption("sRGB Gamma Conversion", "Yes");
    Window *window = root->initialise(true, "Tutorial 00: Basic");
    register_hlms();

    // create scene manager
    const size_t num_threads = 1u;
    SceneManager *scene_manager = root->createSceneManager(ST_GENERIC, num_threads, "ExampleSMInstance");

    // create & setup camera
    Camera *camera = scene_manager->createCamera("Main Camera");

    // position it at 500 in z direction
    // look back along -z
    camera->setPosition(Vector3(0, 5, 15));
    camera->lookAt(Vector3(0, 0, 0));
    camera->setNearClipDistance(0.2f);
    camera->setFarClipDistance(1000.0f);
    camera->setAutoAspectRatio(true);

    // setup basic compositor with a blue clear color
    CompositorManager2 *compositor_manager = root->getCompositorManager2();
    const String workspace_name("Demo Workspace");
    const ColourValue background_color(0.2f, 0.4f, 0.6f);
    compositor_manager->createBasicWorkspaceDef(workspace_name, background_color, IdString());
    compositor_manager->addWorkspace(scene_manager, window->getTexture(), camera, workspace_name, true);

    // event listener
    MyWindowEventListener my_window_event_listener;
    WindowEventUtilities::addWindowEventListener(window, &my_window_event_listener);

    bool quit = false;
    while (!quit) {
        WindowEventUtilities::messagePump();
        quit |= !root->renderOneFrame();
        quit |= my_window_event_listener.get_quit();
    }

    WindowEventUtilities::removeWindowEventListener(window, &my_window_event_listener);

    OGRE_DELETE root;
    root = nullptr;

    return 0;
}