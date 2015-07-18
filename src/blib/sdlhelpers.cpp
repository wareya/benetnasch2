#include "sdlhelpers.hpp"

#include <SDL2/SDL.h>
#undef main
#include <SDL2/SDL_image.h>

#include <iostream> // debugging

void renderTextureInternal( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, int w, int h, bool flipx )
{
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopyEx( renderer, tex, NULL, &dst, 0, NULL, flipx?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE );
}
void renderTextureInternalEX( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, int w, int h, int sx, int sy, int sw, int sh )
{
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_Rect src;
	src.x = sx;
	src.y = sy;
	src.w = sw;
	src.h = sh;
	SDL_RenderCopyEx( renderer, tex, &src, &dst, 0, NULL, SDL_FLIP_NONE );
}
void renderTextureInternalEX2( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, int w, int h, int sx, int sy, int sw, int sh, bool flipx )
{
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_Rect src;
	src.x = sx;
	src.y = sy;
	src.w = sw;
	src.h = sh;
	SDL_RenderCopyEx( renderer, tex, &src, &dst, 0, NULL, flipx?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE );
}

void renderTextureAngledInternal( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, int w, int h, double angle, double xorigin, double yorigin, bool flipx )
{
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_Point origin;
	origin.x = xorigin;
	origin.y = yorigin;
	SDL_RenderCopyEx( renderer, tex, NULL, &dst, angle, &origin, flipx?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE );
}
// scaled and flippable
void renderTexture( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, double scale, bool flipx )
{
	int w, h;
	SDL_QueryTexture( tex, NULL, NULL, &w, &h );
	w = round(w*scale);
	h = round(h*scale);
	renderTextureInternal( tex, renderer, x, y, w, h, flipx );
}
// simple
void renderTexture( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y )
{
	int w, h;
	SDL_QueryTexture( tex, NULL, NULL, &w, &h );
	renderTextureInternal( tex, renderer, x, y, w, h, false );
}
// stretched
void renderTexture( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, int width, int height, bool _unused )
{
    _unused = 0;
    int w, h;
    if(height == 0 or width == 0)
    {
        SDL_QueryTexture( tex, NULL, NULL, &w, &h );
        if(width  == 0) width  = w;
        if(height == 0) height = h;
	}
	renderTextureInternalEX( tex, renderer, x, y, width, height, 0, 0, w, h );
}
// cut
void renderTexture( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, int width, int height, int skipx, bool flipx )
{
    renderTextureInternalEX2( tex, renderer, x, y, width, height, skipx, 0, width, height, flipx );
}
// rotated
void renderTexture( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, double scale, double angle, double xorigin, double yorigin, bool flip )
{
	int w, h;
	SDL_QueryTexture( tex, NULL, NULL, &w, &h );
	w = round(w*scale);
	h = round(h*scale);
	renderTextureAngledInternal( tex, renderer, x, y, w, h, angle, flip?w-xorigin:xorigin, yorigin, flip ); 
}

SDL_Texture *loadTexture( const std::string &file, SDL_Renderer *renderer )
{
	SDL_Texture *texture = IMG_LoadTexture( renderer, file.c_str() );
	if ( texture == nullptr )
		std::cout << "Could not load image: " << SDL_GetError() << std::endl;
	return texture;
}

bfont::bfont(SDL_Renderer *renderer, const std::string &file) : renderer(renderer)
{
    SDL_Surface * surface = IMG_Load(file.c_str());
    SDL_SetColorKey(surface, SDL_TRUE, 000);
	spritesheet = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
}
bfont::~bfont()
{
    if(warnifalivewhenkilled.size() > 0)
    {
        puts("Deleting a font that has external state objects still alive! Something very bad is probably going to happen! FIX THIS");
    }
}

int colorLetter(bfont * muhfont, unsigned char r, unsigned char g, unsigned char b)
{
	return SDL_SetTextureColorMod(muhfont->spritesheet, r, g, b);
}

int uncolorLetter(bfont * muhfont)
{
	return SDL_SetTextureColorMod(muhfont->spritesheet, 255, 255, 255);
}

int renderLetter(int x, int y, unsigned char letter, bfont * muhfont)
{
	int which = letter;
	if(letter > muhfont->last or letter < muhfont->first)
		which = muhfont->defaultchar;
	auto rightward = which%muhfont->sheet_width;
	auto downward = which/muhfont->sheet_width;
	rightward *= muhfont->w;
	downward *= muhfont->h;
	muhfont->src = {rightward, downward, muhfont->w, muhfont->h};
	muhfont->dest = {x, y, muhfont->w, muhfont->h};
	return SDL_RenderCopy(muhfont->renderer, muhfont->spritesheet, &muhfont->src, &muhfont->dest);
}

int renderText(int x, int y, const char * string, bfont * muhfont)
{
	for (int i = 0; string[i] != 0; ++i)
	{
		renderLetter(x+i*muhfont->w, y, string[i], muhfont);
	}
    return 0;
}
