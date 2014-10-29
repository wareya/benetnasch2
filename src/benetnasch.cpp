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
    if (main_init())
        return 1;
    
    while ( !Sys::quit )
    {
        std::vector<bool(*)()> marked_for_removal;
        for(auto function : Sys::tems)
        {
            auto r = function();
            if(r)
                marked_for_removal.push_back(function);
        }
        for(auto function : marked_for_removal)
        {
            unsigned i;
            for (i = 0; Sys::tems[i] != function and i < Sys::tems.size(); ++i) ;
            if(i == Sys::tems.size())
                std::cout << "Bad system removal in mainloop!";
            else
                Sys::tems.erase(Sys::tems.begin()+i);
        }
    }
    return 0;
}
