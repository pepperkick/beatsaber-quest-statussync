#include <android/log.h>
#include <stdlib.h>
#include <time.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string>
#include <thread>
#include <vector>

#include "include/utils/utils.h"
#include "include/inline-hook/inlineHook.h"

#define VERSION "1.0.0"
#define LOG_TAG "SSYNC"

using namespace std;

void StartWebsocket();
void ProcessSongInfo(void*);

typedef struct __attribute__((__packed__)) {
    int m_Handle;
} Scene;

MAKE_HOOK(Hook_GetSongID, 0x129C154, cs_string*, void* self) {
    return Hook_GetSongID(self);
}

MAKE_HOOK(Hook_GetSongName, 0x12B132C, cs_string*, void* self) {
    return Hook_GetSongName(self);
}

MAKE_HOOK(Hook_GetSongSubName, 0x12B1334, cs_string*, void* self) {
    return Hook_GetSongSubName(self);
}

MAKE_HOOK(Hook_GetSongArtist, 0x12B133C, cs_string*, void* self) {
    return Hook_GetSongArtist(self);
}

MAKE_HOOK(Hook_GetSongDuration, 0x12B139C, float, void* self) {
    return Hook_GetSongDuration(self);
}

MAKE_HOOK(Hook_GetLevelAuthor, 0x12B1344, cs_string*, void* self) {
    return Hook_GetLevelAuthor(self);
}

MAKE_HOOK(Hook_GetDifficulty, 0x12B2338, int, void* self) {
    return Hook_GetDifficulty(self);
}

MAKE_HOOK(Hook_GetLevel, 0x12B2390, void*, void* self) {
    return Hook_GetLevel(self);
}

MAKE_HOOK(Hook_StartStandardLevel, 0x12D08D0, void, void* self, void* a1, void* a2, void* a3, void* a4, void* a5, void* a6, void* a7, void* a8, void* a9) {
	Hook_StartStandardLevel(self, a1, a2, a3, a4, a5, a6, a7, a8, a9);

    log("[%s] Song Started", LOG_TAG);

    ProcessSongInfo(a1);
}

MAKE_HOOK(Hook_Scene_GetNameInternal, 0xBE31C4, cs_string*, int handle) {
	return Hook_Scene_GetNameInternal(handle);
}

MAKE_HOOK(Hook_SceneManager_SetActiveScene, 0xBE38CC, int, Scene scene) {	
    int r = Hook_SceneManager_SetActiveScene(scene);

    cs_string* string = Hook_Scene_GetNameInternal(scene.m_Handle);
    char eventText[128];

    csstrtostr(string, &eventText[0]);
    
    log("[%s] Scene Loaded: %s", LOG_TAG, eventText);

    return r;
}

__attribute__((constructor)) void lib_main() {
    INSTALL_HOOK(Hook_Scene_GetNameInternal);
    INSTALL_HOOK(Hook_SceneManager_SetActiveScene);
    INSTALL_HOOK(Hook_StartStandardLevel);
    INSTALL_HOOK(Hook_GetSongID);
    INSTALL_HOOK(Hook_GetSongName);
    INSTALL_HOOK(Hook_GetSongSubName);
    INSTALL_HOOK(Hook_GetSongArtist);
    INSTALL_HOOK(Hook_GetLevelAuthor);
    INSTALL_HOOK(Hook_GetSongDuration);
    INSTALL_HOOK(Hook_GetLevel);
    INSTALL_HOOK(Hook_GetDifficulty);
    
    log("[%s] Loaded, Version: %s", LOG_TAG, VERSION);
}

void ProcessSongInfo(void* a1) {
    int difficulty = Hook_GetDifficulty(a1);
    void* level = Hook_GetLevel(a1);
    cs_string* id = Hook_GetSongID(level);
    cs_string* name = Hook_GetSongName(level);
    cs_string* subname = Hook_GetSongSubName(level);
    cs_string* artist = Hook_GetSongArtist(level);
    cs_string* author = Hook_GetLevelAuthor(level);
    float duration = Hook_GetSongDuration(level);

    char songId[10], songName[128], songSubname[128], songArtist[128], levelAuthor[128];
    csstrtostr(id, &songId[0]);
    csstrtostr(name, &songName[0]);
    csstrtostr(subname, &songSubname[0]);
    csstrtostr(artist, &songArtist[0]);
    csstrtostr(author, &levelAuthor[0]);
}