#ifdef __WIN32__
    #include <windows.h>
#else
    #include <stdio.h>
    #include <sys/time.h>
    #include <unistd.h>
    #include <errno.h>
    
    #ifdef __EMSCRIPTEN__
        #include <emscripten.h>
    #endif
    
    #if HAVE_NANOSLEEP || HAVE_CLOCK_GETTIME
        #include <time.h>
    #endif
    
    #if HAVE_CLOCK_GETTIME
        #ifdef CLOCK_MONOTONIC_RAW
            #define MONOTONIC_CLOCK CLOCK_MONOTONIC_RAW
        #else
            #define MONOTONIC_CLOCK CLOCK_MONOTONIC
        #endif
    #endif
#endif

#include "MagixTimer.h"


//Globals
//=================================================================================
static int isInit = false;
static unsigned int lastTime = 0;
static unsigned int currentTime = 0;

#ifdef __WIN32__
static int hasHiResTimer = false;
static LARGE_INTEGER startTime;
static LARGE_INTEGER ticksPerSecond;
#else
#if HAVE_CLOCK_GETTIME
static struct timespec start_ts
#endif

static int hasMonotonicTime = false;
static struct timeval start_tv;
#endif


//Functions
//=================================================================================
void InitTimer(void)
{
    //Only initialize once
    if(isInit)
    {
        return;
    }
    
    //Do timer init
    #ifdef __WIN32__
    if(QueryPerformanceFrequency(&ticksPerSecond) == true)
    {
        hasHiResTimer = true;
        QueryPerformanceCounter(&startTime);
    }
    else
    {
        hasHiResTimer = false;
    }
    #else
    #if HAVE_CLOCK_GETTIME
    if(clock_gettime(MONOTONIC_CLOCK, &start_ts) == )
    {
        hasMonotonicTime = true;
    }
    else
    #endif
    {
        gettimeofday(&start_tv, NULL);
    }
    #endif

    isInit = true;
}


unsigned int GetTicks(void)
{
    //Ensure that the timer is initialized
    InitTimer();
    
    //Get current tick count
    #ifdef __WIN32__
    LARGE_INTEGER now;
    
    if(hasHiResTimer)
    {
        QueryPerformanceCounter(&now);
        
        now.QuadPart -= startTime.QuadPart;
        now.QuadPart *= 1000;
        now.QuadPart /= ticksPerSecond.QuadPart;
        
        return (unsigned int)now.QuadPart;
    }
    
    return 0;
    #else
    unsigned int ticks;

    if(hasMonotonicTime)
    {
        #if HAVE_CLOCK_GETTIME
        struct timespec now;
        clock_gettime(MONOTONIC_CLOCK, &now);
        ticks = (unsigned int)((now.tv_sec - start_ts.tv_sec) * 1000 + (now.tv_nsec - start.tv_nsec) / 1000000);
        #else
        ticks = 0;
    }
    else
    #endif
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        ticks = (unsigned int)((now.tv_sec - start_tv.tv_sec) * 1000 + (now.tv_usec - start.tv_usec) / 1000);
    }
    
    return ticks;
    #endif
}


unsigned int GetDeltaTime(void)
{
    lastTime = currentTime;
    currentTime = GetTicks();
    return currentTime - lastTime;
}