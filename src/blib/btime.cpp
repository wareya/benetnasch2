#define USE_SOURCE_HEADER_BTIME

#include <chrono>
#include <vector>

namespace Time
{
    bool dostart = true;
    
    #ifdef B_FRAMERATE
    const double Framerate = B_FRAMERATE;
    #else
    const double Framerate = 250;
    #endif
    
    const double Framesnum = Framerate;
    const double Frametime = 1000/Framerate;
    
    double ticks = 0;
    
    std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
    double scale = double(std::chrono::microseconds::period().den) / double(std::chrono::microseconds::period().num);
    
    double last_us, simstart_us, delta_us, delta, deviance, error, sim, halt;
    std::vector<double> frames;
    
    std::chrono::high_resolution_clock clock;
    double get_us()
    {
        end = clock.now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
}
