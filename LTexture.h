#ifndef LTEXTURE_H
#define LTEXTURE_H

#include<string>
#include <SDL_image.h>
#include <SDL_ttf.h>

class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		//Creates image from font string
        bool loadFromRenderedText( std::string textureText, SDL_Color textColor, TTF_Font* gFont );

		//Deallocates texture
		void free();

		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL );

		//Sets color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

#endif
