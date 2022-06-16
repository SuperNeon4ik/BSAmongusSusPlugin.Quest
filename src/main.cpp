#include "main.hpp"
#include "Utils/AsyncAudiocliploader.hpp"

#include "UnityEngine/GameObject.hpp"

#include "GlobalNamespace/SongPreviewPlayer.hpp"
#include "GlobalNamespace/SongPreviewPlayer_AudioSourceVolumeController.hpp"
using namespace GlobalNamespace;

#include "UnityEngine/SceneManagement/Scene.hpp"
#include "UnityEngine/SceneManagement/SceneManager.hpp"
using namespace UnityEngine::SceneManagement;

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/QuestUI.hpp"
using namespace QuestUI;

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

AsyncAudioClipLoader::loader startupLoader;

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

// MAKE_HOOK_MATCH(SongPreviewPlayer_OnEnable, &SongPreviewPlayer::OnEnable, void, SongPreviewPlayer* self) {
    
// }

MAKE_HOOK_MATCH(SceneManager_Internal_ActiveSceneChanged, &SceneManager::Internal_ActiveSceneChanged, void, Scene previousActiveScene, Scene newActiveScene) {
    SceneManager_Internal_ActiveSceneChanged(previousActiveScene, newActiveScene);
    if (newActiveScene.IsValid()) {
        std::string sceneName = to_utf8(csstrtostr(newActiveScene.get_name()));
        getLogger().info("Scene found: %s", sceneName.data());

        // std::string questInit = "QuestInit";
        // if(sceneName == questInit) QuestSounds::AudioClips::loadAudioClips();

        if (sceneName == "MainMenu") {
            getLogger().debug("Doing startup thing...");
            startupLoader.filePath = "/sdcard/ModData/com.beatgames.beatsaber/Mods/AmongusSusMod/startup.ogg";

            static auto goName = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("There is an impostor AMONG US");
            UnityEngine::GameObject* audioClipGO = UnityEngine::GameObject::New_ctor(goName);
            UnityEngine::AudioSource* asource = audioClipGO->AddComponent<UnityEngine::AudioSource*>();

            startupLoader.audioSource = asource;
            startupLoader.load();
            // startupLoader.set_OriginalClip(start)

            // asource->set_playOnAwake(false);
            // asource->set_clip();
            // asource->Play();
            UnityEngine::Object::DontDestroyOnLoad(audioClipGO);
            getLogger().debug("Created startup thing");
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
    // Install our hooks (none defined yet)
    // INSTALL_HOOK(hkLog, SongPreviewPlayer_OnEnable);
    INSTALL_HOOK(getLogger(), SceneManager_Internal_ActiveSceneChanged);
    getLogger().info("Installed all hooks!");
}

void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    // Create our UI elements only when shown for the first time.
    if(firstActivation) {
        // Create a container that has a scroll bar.
        UnityEngine::GameObject* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());
       
        // Create a text that says "Hello World!" and set the parent to the container.
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "There is absolutely nothing here!");
        QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Go to /sdcard/ModData/com.beatgames.beatsaber/Mods/AmongusSusMod/");
    }
}