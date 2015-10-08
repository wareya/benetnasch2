#ifndef INCLUDED_BTIME
#define INCLUDED_BTIME

#include <chrono>
#include <deque>

namespace Time
{
    extern bool dostart;
    
    extern const double Framerate;
    
    extern const double Framesnum;
    extern const double Frametime;
    extern double ticks;
    
    extern std::chrono::time_point<std::chrono::high_resolution_clock> start;
    extern std::chrono::time_point<std::chrono::high_resolution_clock> end;
    extern double scale;
    extern double last_us, simstart_us, delta_us, delta, deviance, error, sim, halt;
    extern std::deque<double> frames;
    
    double get_us();
}

#endif
