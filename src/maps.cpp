#include "maps.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream> // debugging

#include "components/allcomponents.hpp"

namespace Maps
{
    long width;
    long height;
    
    void load_background(const char * filename)
    {
        auto background = new Sys::BackgroundDrawable(Ent::New());
        background->init(filename);
    }
    void load_wallmask(const char * filename)
    {
        SDL_Surface * wallmask = IMG_Load(filename);
        if(!wallmask)
        {
            printf("IMG_Load: %s\n", IMG_GetError());
            throw; // can't recover
        }
        int bpp = wallmask->format->BytesPerPixel;
        unsigned char r;
        unsigned char g;
        unsigned char b;
        //std::cout << "Loading wallmask from " << filename << "\n";
        int scale = 4;
        width  = wallmask->w * scale;
        height = wallmask->h * scale;
        
        for (long y = 0; y < wallmask->h; ++y)
        {
            if (y%10 == 0)
                std::cout << "Row " << y << "\n";
            long rect_x = -1;
            
            for (long x = 0; x < wallmask->w; ++x)
            {
                //std::cout << "Column " << x << "\n";
                SDL_GetRGB(*((unsigned char*)(wallmask->pixels)+(x + y*wallmask->w)*bpp), wallmask->format, &r, &g, &b);
                if(r < 127)
                {
                    if(rect_x == -1) // new rect
                    {
                        rect_x = x;
                    }
                }
                else
                {
                    if(rect_x != -1) // end of rect
                    {
                        new Sys::BoxDrawable(Ent::New(), scale*rect_x, scale*y, scale*(x-rect_x), scale);
                        rect_x = -1;
                    }
                }
            }
            if(rect_x != -1) // end of rect
            {
                new Sys::BoxDrawable(Ent::New(), scale*rect_x, scale*y, scale*(wallmask->w-rect_x), scale);
            }
        }
    }
}
