#include <android/log.h>
#include <stdlib.h>
#include <time.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string>
#include <thread>

#include "include/utils/utils.h"
#include "include/inline-hook/inlineHook.h"

#define VERSION "1.0.0"
#define LOG_TAG "SSYNC"

using namespace std;

__attribute__((constructor)) void lib_main() {
    log("[%s] Loaded, Version: %s", LOG_TAG, VERSION);
}