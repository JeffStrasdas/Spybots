#include "IMG.h"
#include <cstdio>
LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::createTexture(SDL_Color color,int w, int h,SDL_Renderer* gRenderer,SDL_Window* gWindow)
{
    if(mTexture != NULL)
        free();
    bool success = true;
    SDL_Texture* newTexture = SDL_CreateTexture(gRenderer,SDL_GetWindowPixelFormat( gWindow ),SDL_TEXTUREACCESS_STREAMING,w,h); //Pixelformat = SDL_PIXELFORMAT_RGB888
    mWidth  = w * scaling_factor;
    mHeight = h * scaling_factor;
    //Lock texture for manipulation
    SDL_LockTexture( newTexture, NULL, &mPixels, &mPitch );
    //Color texture
    Uint32* tempP = (Uint32*)mPixels;
    int pixelCount = ( mPitch / 4 ) * h;
    Uint32 tempC = SDL_MapRGBA(SDL_GetWindowSurface( gWindow )->format,color.r,color.g,color.b,color.a);
    //color pixels
    for(int i=0; i<pixelCount;++i)
    {
        tempP[i] = tempC;
    }
    SDL_UnlockTexture( newTexture );
    mTexture = newTexture;
    mPixels = NULL;
    mPitch = 0;
    return success;
}

bool LTexture::loadFromFile( std::string path,SDL_Renderer* gRenderer)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;
    /*
	//new method
	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if(loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n",path.c_str(),IMG_GetError());
    }
    else
    {
        //Convert surface to display format
        SDL_Surface* formattedSurface = SDL_ConvertSurface(loadedSurface, SDL_GetWindowSurface(gWindow)->format,NULL);
        if(formattedSurface == NULL)
            printf("Unable to convert loaded surface to display format! SDL Error: %s\n",SDL_GetError());
        else
        {
            //Create blank streamable texture
            newTexture = SDL_CreateTexture(gRenderer, SDL_GetWindowPixelFormat(gWindow),SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
            if(newTexture == NULL)
                printf("Unable to create blank texture! SDL Error: %s\n",SDL_GetError());
            else
            {
                //Lock texture for manipulation
                SDL_LockTexture(newTexture, NULL, &mPixels, &mPitch);

                //Copy loaded/formatted surface pixels
                memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

                //Unlock texture to update
                SDL_UnlockTexture(newTexture);
                mPixels = NULL;

                //Get image dimensions
                mWidth = formattedSurface->w;
                mHeight = formattedSurface->h;
            }
            //Get rid of old formatted surface
            SDL_FreeSurface(formattedSurface);
        }
        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }
    //return success
    mTexture = newTexture;
    return mTexture != NULL;
*/
	//old variant
	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w * scaling_factor;
			mHeight = loadedSurface->h * scaling_factor;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;

}

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}
void LTexture::render( int x, int y, SDL_Renderer* gRenderer, SDL_Rect* clip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x * scaling_factor, y * scaling_factor, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w * scaling_factor;
		renderQuad.h = clip->h * scaling_factor;
	}

	//Render to screen
	SDL_RenderCopy( gRenderer, mTexture, clip, &renderQuad );
}

void LTexture::renderx(int x, int y, SDL_Renderer* gRenderer, SDL_Rect* clip, double angle, SDL_RendererFlip flip)
{
    //Set rendering space and render to screen
	SDL_Rect renderQuad = { x * scaling_factor, y * scaling_factor, mWidth, mHeight };

	//Set clip rendering dimensions
	if(clip != NULL)
    {
        renderQuad.w = clip->w * scaling_factor;
        renderQuad.h = clip->h * scaling_factor;
    }

    //Render to screen
    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, NULL, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}
void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
    //Modulate texture
    SDL_SetTextureColorMod( mTexture, red, green, blue );
}
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* gFont,SDL_Renderer* gRenderer)
{
    //get rid of preexisting texture
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont,textureText.c_str(),textColor);
    if(textSurface==NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if(mTexture == NULL)
        {
            printf("Unable to create texture from rendered text! SDL Error:%s\n",SDL_GetError());
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }
        //Get rid of old surface
        SDL_FreeSurface(textSurface);
    }
    //Return success
    return mTexture != NULL;
}






