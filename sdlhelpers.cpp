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
