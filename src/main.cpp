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
#include "include/librws.h"

#define VERSION "1.0.0"
#define LOG_TAG "SSYNC"

using namespace std;

void StartWebsocket();
void SendSongInfo(void*);

rws_socket _socket = NULL;
rws_socket _serverSocket = NULL;
char server_ip[32] = "127.0.0.1";
int server_port = 4050;

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

    SendSongInfo(a1);
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


static void on_socket_connected(rws_socket socket) {
    log("[%s] Socket connected", LOG_TAG);
    rws_socket_send_text(socket, "{\"event\":\"hello\"}");
    _serverSocket = socket;
}

static void on_socket_disconnected(rws_socket socket) {
    rws_error error = rws_socket_get_error(socket);

    if (error) { 
        log("[%s] Socket disconnected with code, error: %i, %s", LOG_TAG, rws_error_get_code(error), rws_error_get_description(error)); 
    } else {
        log("[%s] Socket disconnected", LOG_TAG);
    }

    _socket = NULL;
    _serverSocket = NULL;
}

static void on_socket_received_text(rws_socket socket, const char * text, const unsigned int length) {
    log("[%s] Socket Received Text: %s", LOG_TAG, text);
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

    FILE* config = fopen("/sdcard/Android/data/com.beatgames.beatsaber/files/cfgs/lightsync.json", "r");
    fscanf(config, "%s\n%d", &server_ip[0], &server_port);
    fclose(config);
    
    thread websocketThread(StartWebsocket);
    websocketThread.detach();
    
    log("[%s] Loaded, Version: %s", LOG_TAG, VERSION);
}

void StartWebsocket() {
    _socket = rws_socket_create();
    rws_socket_set_scheme(_socket, "ws");
    rws_socket_set_host(_socket, server_ip);
    rws_socket_set_port(_socket, server_port);
    rws_socket_set_path(_socket, "/");    
    rws_socket_set_on_disconnected(_socket, &on_socket_disconnected);
    rws_socket_set_on_connected(_socket, &on_socket_connected);
    rws_socket_set_on_received_text(_socket, &on_socket_received_text);
    rws_socket_connect(_socket);

    log("[%s] Websocket Started", LOG_TAG);
}

void SendSongInfo(void* a1) {
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

    if (_serverSocket) {

    }
}