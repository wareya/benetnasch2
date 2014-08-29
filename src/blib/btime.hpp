#ifndef INCLUDED_BTIME
#define INCLUDED_BTIME

#include <chrono>
#include <vector>

namespace Time
{
    extern bool dostart;
    
    #ifdef B_FRAMERATE
    const double Framerate = B_FRAMERATE;
    #else
    const double Framerate = 250;
    #endif
    
    const double Framesnum = Framerate;
    const double Frametime = 1000/Framerate;
    extern double ticks;
    
    extern std::chrono::time_point<std::chrono::high_resolution_clock> start;
    extern std::chrono::time_point<std::chrono::high_resolution_clock> end;
    extern double scale;
    extern double last_us, simstart_us, delta_us, delta, deviance, error, sim, halt;
    extern std::vector<double> frames;
    
    double get_us();
}

#endif
