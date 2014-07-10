void renderTexture( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, int w, int h )
{
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy( renderer, tex, NULL, &dst );
}
void renderTexture( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y )
{
	int w, h;
	SDL_QueryTexture( tex, NULL, NULL, &w, &h );
	renderTexture( tex, renderer, x, y, w, h );
}

SDL_Texture *loadTexture( const std::string &file, SDL_Renderer *renderer )
{
	SDL_Texture *texture = IMG_LoadTexture( renderer, file.c_str() );
	if ( texture == nullptr )
		std::cout << "Could not load image: " << SDL_GetError() << std::endl;
	return texture;
}


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
	bfont(SDL_Renderer *renderer, const std::string &file) : renderer(renderer)
	{
		spritesheet = loadTexture(file, renderer);
	}
	SDL_Rect src;
	SDL_Rect dest;
};

int colorLetter(bfont * muhfont, unsigned char r, unsigned char g, unsigned char b)
{
	return SDL_SetTextureColorMod(muhfont->spritesheet, r, g, b);
}

int uncolorLetter(bfont * muhfont)
{
	return SDL_SetTextureColorMod(muhfont->spritesheet, 255, 25, 25);
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
