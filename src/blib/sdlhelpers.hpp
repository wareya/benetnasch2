#ifndef INCLUDED_SDLHELPERS
#define INCLUDED_SDLHELPERS

#include <SDL2/SDL.h>
#undef main
#include <SDL2/SDL_image.h>
#include <string>

void renderTextureInternal( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, int w, int h, bool flipx );

void renderTextureAngledInternal( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, int w, int h, double angle, double xorigin, double yorigin, bool flipx );
// scaled and flippable
void renderTexture( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, double scale, bool flipx );
// simple
void renderTexture( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y );
// rotated
void renderTexture( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, double scale, double angle, double xorigin, double yorigin, bool flip );

SDL_Texture *loadTexture( const std::string &file, SDL_Renderer *renderer );


struct bfont // struct containing data for a fixed width font
{
	SDL_Texture * spritesheet;
	int w = 7;
	int h = 13;
	int sheet_width = 32;
	char first = 32; // lowest valid character
	char last = 127; // highest valid character
	char defaultchar = ' '; // default character when outside of valid range
	SDL_Renderer * renderer;
	bfont(SDL_Renderer *renderer, const std::string &file);
	SDL_Rect src;
	SDL_Rect dest;
};

int colorLetter(bfont * muhfont, unsigned char r, unsigned char g, unsigned char b);

int uncolorLetter(bfont * muhfont);

int renderLetter(int x, int y, unsigned char letter, bfont * muhfont);

int renderText(int x, int y, const char * string, bfont * muhfont);

#endif
