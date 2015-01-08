#ifndef B_INCLUDE_RENDERING_FONTFUNCTIONS
#define B_INCLUDE_RENDERING_FONTFUNCTIONS
#include "../rendering.hpp"

namespace Sys
{
    namespace Renderers
    {
        struct TemporaryTextureColor
        {
            Uint8 oldr, oldg, oldb;
            SDL_Texture * texture;
            TemporaryTextureColor(bfont * farg, Uint8 r, Uint8 g, Uint8 b)
            {
                texture = farg->spritesheet;
                SDL_GetTextureColorMod(texture, &oldr, &oldg, &oldb);
                SDL_SetTextureColorMod(texture, r, g, b);
            }
            TemporaryTextureColor(SDL_Texture * targ, Uint8 r, Uint8 g, Uint8 b)
            {
                texture = targ;
                SDL_GetTextureColorMod(texture, &oldr, &oldg, &oldb);
                SDL_SetTextureColorMod(texture, r, g, b);
            }
            ~TemporaryTextureColor()
            {
                SDL_SetTextureColorMod(texture, oldr, oldg, oldb);
            }
            void* operator new(size_t)
            {
                puts("DON'T DO THIS. 13461345123");
                return NULL;
            }
        };
        struct TemporaryOffscreenRenderMode
        {
            SDL_Texture * texture;
            SDL_Renderer * renderer;
            TemporaryOffscreenRenderMode(SDL_Renderer * master)
            {
                renderer = master;
                texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Sys::shape.w, Sys::shape.h);
                SDL_SetRenderTarget(renderer, texture);
                SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND );
                
                SDL_BlendMode temp;
                SDL_GetRenderDrawBlendMode( renderer, &temp );
                
                SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_NONE );
                SDL_SetRenderDrawColor( renderer, 127, 0, 255, 0 ); // RGBs are to detect improper blending
                SDL_RenderFillRect( renderer, &shape );
                
                SDL_SetRenderDrawBlendMode( renderer, temp );
            }
            ~TemporaryOffscreenRenderMode()
            {
                if(renderer != NULL)
                    SDL_SetRenderTarget(renderer, NULL);
                SDL_DestroyTexture(texture);
            }
            void release()
            {
                if(renderer != NULL)
                    SDL_SetRenderTarget(renderer, NULL);
                renderer = NULL;
            }
            void* operator new(size_t)
            {
                puts("DON'T DO THIS. 45674rt2134");
                return NULL;
            }
        };
    }
}
#endif // B_INCLUDE_RENDERING_FONTFUNCTIONS
