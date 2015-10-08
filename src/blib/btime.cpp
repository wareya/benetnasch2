#define USE_SOURCE_HEADER_BTIME

#include "btime.hpp"

namespace Time
{
    bool dostart = true;
    
    double ticks = 0;
    
    #ifdef B_FRAMERATE
    const double Framerate = B_FRAMERATE;
    #else
    const double Framerate = 200;
    #endif
    
    const double Framesnum = Framerate;
    const double Frametime = 1000/Framerate;
    
    std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
    double scale = double(std::chrono::microseconds::period().den) / double(std::chrono::microseconds::period().num);
    
    double last_us, simstart_us, delta_us, delta, deviance, excessdeviance, error, sim, halt, asked;
    std::deque<double> frames;
    
    std::chrono::high_resolution_clock clock;
    double get_us()
    {
        end = clock.now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
}
