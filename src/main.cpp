#include "main.hpp"
#include "Utils/AsyncAudiocliploader.hpp"

#include "UnityEngine/GameObject.hpp"

#include "GlobalNamespace/SongPreviewPlayer.hpp"
#include "GlobalNamespace/SongPreviewPlayer_AudioSourceVolumeController.hpp"
using namespace GlobalNamespace;

#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
using namespace UnityEngine::SceneManagement;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

AsyncAudioClipLoader::loader startupLoader;
bool hasPlayedStartupSound = false;

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

MAKE_HOOK_MATCH(SceneManager_Internal_ActiveSceneChanged, &SceneManager::Internal_ActiveSceneChanged, void, Scene previousActiveScene, Scene newActiveScene) {
    SceneManager_Internal_ActiveSceneChanged(previousActiveScene, newActiveScene);
    if (newActiveScene.IsValid()) {
        if (newActiveScene.get_name() == "MainMenu" && !hasPlayedStartupSound) {
            getLogger().debug("Doing startup thing...");
            startupLoader.filePath = "/sdcard/ModData/com.beatgames.beatsaber/Mods/AmongusSusMod/startup.ogg";

            static auto goName = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("There is an impostor AMONG US");
            UnityEngine::GameObject* audioClipGO = UnityEngine::GameObject::New_ctor(goName);
            UnityEngine::AudioSource* asource = audioClipGO->AddComponent<UnityEngine::AudioSource*>();

            startupLoader.audioSource = asource;
            startupLoader.load();

            getLogger().debug("Created startup thing");
            hasPlayedStartupSound = true;
        }
    }
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup of that amongus thing!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), SceneManager_Internal_ActiveSceneChanged); // For the 3rd point in the feature list
    getLogger().info("Installed all hooks!");
}