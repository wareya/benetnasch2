#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <functional>
#include <cmath>

#include <nall/set.hpp>
#include <nall/map.hpp>

#include "bengine.hpp"
#include "boot.hpp"

#include <cstdio>

int main(int argc, char *argv[])
{
    puts("booting");
    // boot engine and set up mainloop
    if (main_init())
        return 1;
    
    // this is our mainloop -- systems are listed in Sys::tems
    while ( !Sys::quit )
    {
        // which systems are marked for removal from Sys::tems
        // (removing by index is a headache when multiple are removed at once)
        std::vector<bool(*)()> marked_for_removal;
        for(auto function : Sys::tems)
        {
            auto r = function(); // run system
            if(r) // check whether it wanted us to remove it
                marked_for_removal.push_back(function);
        }
        for(auto function : marked_for_removal)
        {
            unsigned i;
            for (i = 0; Sys::tems[i] != function and i < Sys::tems.size(); ++i);
            if(i == Sys::tems.size())
                std::cout << "Bad system removal in mainloop!\n";
            else
            {
                std::cout << "Removed a system from mainloops.\n";
                Sys::tems.erase(Sys::tems.begin()+i);
            }
        }
    }
    return 0;
}
