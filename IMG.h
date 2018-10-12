#ifndef IMG_H
#define IMG_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>

const float scaling_factor = 1.0;
//Screen dimension constants
const int SCREEN_WIDTH  = 650 * scaling_factor;
const int SCREEN_HEIGHT = 420 * scaling_factor;

//Constants for text
const int TEXT_OFFSET   = 1;
const int ENTITY_AMMOUNT= 5;
const int INFO_COUNT    = 15;
const int TEXT_AMMOUNT  = TEXT_OFFSET + ENTITY_AMMOUNT*INFO_COUNT;
const int MAX_INFO_LENGTH= 20;



//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path, SDL_Renderer* gRenderer);

		//Creates image from font string
        bool loadFromRenderedText( std::string textureText, SDL_Color textColor, TTF_Font* gFont, SDL_Renderer* gRenderer );

        //Creates texture with secified width and height
        bool createTexture(SDL_Color color, int w,int h,SDL_Renderer* gRenderer,SDL_Window* gWindow);

		//Deallocates texture
		void free();

		//Renders texture at given point
		void render( int x, int y , SDL_Renderer* gRenderer , SDL_Rect* clip = NULL );

        //Renders texture at given point including angle
        void renderx(int x , int y , SDL_Renderer* gRenderer , SDL_Rect* clip = NULL , double angle = 0, SDL_RendererFlip flip = SDL_FLIP_NONE);

		//Sets color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Gets image dimensions
		int getWidth();
		int getHeight();

		 //Pixel manipulators
		 bool lockTexture();
		 bool unlockTexture();
		 void* getPixels();
		 int getPitch();

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;
        void* mPixels;
		int mPitch;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//Starts up SDL and creates window
bool init(SDL_Window* gWindow);

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();


#endif
