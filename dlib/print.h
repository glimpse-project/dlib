#pragma once

#ifdef ANDROID
#include <android/log.h>
#else
#include <stdio.h>
#endif

#ifdef ANDROID
#define DLIB_PRINT(fmt...) __android_log_print(ANDROID_LOG_INFO, "DLib", fmt)
#else
#define DLIB_PRINT(fmt...) do { fprintf(stderr, fmt); fprintf(stderr, "\n"); } while(0)
#endif
