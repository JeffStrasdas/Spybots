#ifndef TEXT_H
#define TEXT_H

#include <cstdio>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "IMG.h"

namespace Text
{
//Old
//bool Draw(SDL_Renderer* gRenderer,int x,int y,std::string Text,TTF_Font* Font,SDL_Color textColor = {255,255,255}, int width = 114);
class Window
{
public:
    Window(int width, int height, std::string Title): width(width),height(height)
    {
        //Empty window
        GenerateEmpty(Title);
    }
    Window(int width, int height, std::string Title, std::string Description, std::string Button1, std::string Button2):
        width(width),height(height)
    {
        //Normal window
        GenerateNormal(Title, Description, Button1, Button2);
    }
    Window(int ID, int width, int height, int bHeight, std::string Title, std::string Description, std::string Button1, std::string Button2):
        width(width),height(height),bHeight(bHeight)
    {
        //Shop window
        GenerateShop(ID, Title, Description, Button1, Button2);
    }

    ~Window()
    {
        Texture->free();
        delete Texture;
        Texture = NULL;

        delete next;
        next = NULL;
    }

    void GenerateEmpty(std::string Title);
    void GenerateNormal(std::string Title, std::string Description, std::string Button1, std::string Button2);
    void GenerateShop(int ID, std::string Title, std::string Description, std::string Button1, std::string Button2);

    void Show(int UID, int x, int y);
    void ChangeTitle(std::string NewTitle);
    //0 -> no button   1 -> Button1   2 -> Button2
    int Click(int mx, int my);

private:
    //bHeight useless? Measure!
    int width, height, bHeight;
    //std::string Title,Description,Button1Text,Button2Text;
    LTexture* Texture;
    Window* next;
};

class TextRenderer
{
public:
    TextRenderer(SDL_Renderer* renderer,std::string SmallText, std::string BoldText):Renderer(renderer)
    {
        std::string line = "images/"+SmallText+".png";
        if( !TextureSmallText.loadFromFile(line.c_str(),Renderer) )
        {
            printf( "Failed to load small text texture!\n" );
        }
        line = "images/"+BoldText+".png";
        if( !TextureBoldText.loadFromFile(line.c_str(),Renderer) )
        {
            printf( "Failed to load bold text texture!\n" );
        }
        if(!LoadMedia(SmallText,BoldText))
            printf("Failed to load text rects!\n");
    }
    ~TextRenderer()
    {
        Renderer = NULL;
    }
    void SetColor(bool small, Uint8 red, Uint8 green, Uint8 blue );
    //bool Draw(int x, int y, std::string Text,bool small = true, SDL_Color textColor = {255,255,255}, int width = 114);
    void Write(int x, int y, std::string Text, bool small);
    void Write(int x, int y, std::string Text, bool small, int width);
private:
    bool LoadMedia(std::string SmallTextPath, std::string BoldTextPath);

    SDL_Renderer* Renderer = NULL;
    LTexture TextureSmallText;
    LTexture TextureBoldText;
    SDL_Rect RectSmallText[95];
    SDL_Rect RectBoldText[95];
};
}
#endif
