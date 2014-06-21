namespace Time
{
    bool dostart = true;
    const double Framerate = 60;
    const double Framesnum = Framerate;
    const double Frametime = 1000/Framerate;
    double ticks = 0;
    
    std::chrono::high_resolution_clock clock;
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    double scale = double(std::chrono::microseconds::period().den) / double(std::chrono::microseconds::period().num);
    double last_us, simstart_us, delta_us, delta, deviance, error;
    std::vector<double> frames;
    
    double get_us()
    {
        end = clock.now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
}
