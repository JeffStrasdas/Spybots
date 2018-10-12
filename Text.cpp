#include "Text.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include "IMG.h"
#include <string>
#include <cstdio>
#include <fstream>

void Text::TextRenderer::Write(int x, int y,std::string Text, bool small)
{
    int Offset = 0;
    if(small)
    {
        for(unsigned int i = 0; i < Text.length(); ++i)
        {
            //new line
            if(Text[i] == 92)
                if(Text[i+1] == 'n')
                {
                    Write(x,y+10,Text.substr(i+2,Text.length()-i-2),small);
                    return;
                }
            TextureSmallText.render(x+Offset,y,Renderer,&RectSmallText[Text[i]-32]);
            Offset += RectSmallText[Text[i]-32].w +1;
        }
    }
    else
    {
        for(unsigned int i = 0; i < Text.length(); ++i)
        {
            //new line
            if(Text[i] == 92)
                if(Text[i+1] == 'n')
                {
                    Write(x,y+10,Text.substr(i+2,Text.length()-i-2),small);
                    return;
                }

            TextureBoldText.render(x+Offset,y,Renderer,&RectBoldText[Text[i]-32]);
            Offset += RectBoldText[Text[i]-32].w +1;
        }

    }
}
/*
bool Text::Draw(SDL_Renderer* gRenderer,int x,int y,std::string Text,TTF_Font* Font,SDL_Color textColor, int width)
{
    const unsigned int LetterWidth = 6;
    bool success = true;
    LTexture tempTexture;
    if(Text.length() * LetterWidth < width)
    {
        if(!tempTexture.loadFromRenderedText(Text,textColor,Font,gRenderer))
        {
            printf("Failed to render text texture!\n");
            success = false;
        }
        tempTexture.render(x,y,gRenderer);
        return success;
    }
    int index     = Text.length()-1;
    int tempwidth = width/6;
    while(index>0 && (Text[index] != ' ' || index >= tempwidth))
        index--;
    if(index <= 0)
    {
        if(!tempTexture.loadFromRenderedText(Text,textColor,Font,gRenderer))
        {
            printf("Failed to render text texture!\n");
            success = false;
        }
        tempTexture.render(x,y,gRenderer);
        return success;
    }

    if(!tempTexture.loadFromRenderedText(Text.substr(0,index),textColor,Font,gRenderer))
    {
        printf("Failed to render text texture!\n");
        success = false;
    }
    tempTexture.render(x,y,gRenderer);
    if(index < Text.length()-2)
        return Draw(gRenderer,x,y+10,Text.substr(++index),Font,textColor, width);
    return success;
}
*/
void Text::TextRenderer::SetColor(bool small, Uint8 red, Uint8 green, Uint8 blue )
{
    if(small)
        TextureSmallText.setColor(red,green,blue);
    else
        TextureBoldText.setColor(red,green,blue);
}
void Text::TextRenderer::Write(int x,int y,std::string Text,bool small, int width)
{
    int currentwidth = 0;
    int i = 0;
    int index = 0;
    int length = Text.length();
    //printf("try to render %s..",Text.c_str());
    while(i < length)
    {
        if(Text[i] == ' ')
            index = i;
        currentwidth += RectSmallText[Text[i]-32].w;
        if(currentwidth >= width)
            break;
        i++;
    }
    //printf("%i fit inside\n",index);
    if(i != length && index != 0)
    {
        Write(x,y,Text.substr(0,index++),small);
        Write(x,y+10,Text.substr(index,length-index),small,width);
    }
    else
        Write(x,y,Text,small);
}
/*bool Text::TextRenderer::Draw(int x,int y,std::string Text,bool small, SDL_Color textColor, int width)
{
    const unsigned int LetterWidth = 6;
    bool success = true;
    LTexture tempTexture;
    if(Text.length() * LetterWidth < width)
    {
        if(!tempTexture.loadFromRenderedText(Text,textColor,small ? FontSmall : FontBold,Renderer))
        {
            printf("Failed to render text texture!\n");
            success = false;
        }
        tempTexture.render(x,y,Renderer);
        return success;
    }
    int index     = Text.length()-1;
    int tempwidth = width/6;
    while(index>0 && (Text[index] != ' ' || index >= tempwidth))
        index--;
    if(index <= 0)
    {
        if(!tempTexture.loadFromRenderedText(Text,textColor,small ? FontSmall : FontBold,Renderer))
        {
            printf("Failed to render text texture!\n");
            success = false;
        }
        tempTexture.render(x,y,Renderer);
        return success;
    }

    if(!tempTexture.loadFromRenderedText(Text.substr(0,index),textColor,small ? FontSmall : FontBold,Renderer))
    {
        printf("Failed to render text texture!\n");
        success = false;
    }
    tempTexture.render(x,y,Renderer);
    if(index < Text.length()-2)
        return Draw(x,y+10,Text.substr(++index),small,textColor, width);
    return success;
}*/

bool Text::TextRenderer::LoadMedia(std::string SmallText, std::string BoldText)
{
    //Small Text
    std::string line = "data/" + SmallText + ".txt";
    std::ifstream ifile(line.c_str());
    int index;
    int a,b;

    //Set up empty characters
    for(a = 0; a < 95; ++a)
    {
        RectSmallText[a].x = RectSmallText[a].y = RectSmallText[a].w = RectSmallText[a].h = 0;
        RectBoldText[a].x  = RectBoldText[a].y  = RectBoldText[a].w  = RectBoldText[a].h  = 0;
    }

    //Small Text
    if(ifile.is_open())
    {
        //count lines
        //int length = 94;
        //while(getline(ifile,line))
        //{
        //    //filter empty lines and comments
        //    if(line.length() == 0 || line[0] == '#')
        //        continue;
        //    ++length;
        //}
        //ifile.clear();
        //ifile.seekg (0, ifile.beg);

        //use data
        //RectSmallText = new SDL_Rect[length];

        while(getline(ifile,line))
        {
            //filter empty lines and comments
            if(line.length() < 4 || line[1] != ':')
                continue;
            index = line[0]-32;
            //x y w h
            a = b = 3;
            while(line[b] != ',')
                ++b;
            RectSmallText[index].x = atoi(line.substr(a,b-a).c_str());
            a = ++b;
            while(line[b] != ',')
                ++b;
            RectSmallText[index].y = atoi(line.substr(a,b-a).c_str());
            a = ++b;
            while(line[b] != ',')
                ++b;
            RectSmallText[index].w = atoi(line.substr(a,b-a).c_str());
            a = ++b;
            b = line.length();
            RectSmallText[index].h = atoi(line.substr(a,b-a).c_str());

            //printf("%i,%i,%i,%i\n",RectSmallText[index].x,RectSmallText[index].y,RectSmallText[index].w,RectSmallText[index].h);

        }
        ifile.close();
    }
    //Bold text

    line = "data/" + BoldText + ".txt";
    ifile.open(line.c_str());
    if(ifile.is_open())
    {
        int index;
        int a,b;
        while(getline(ifile,line))
        {
            //filter empty lines and comments
            if(line.length() < 4 || line[1] == '#')
                continue;
            index = line[0]-32;
            //x y w h
            a = b = 3;
            while(line[b] != ',')
                ++b;
            RectBoldText[index].x = atoi(line.substr(a,b-a).c_str());
            a = ++b;
            while(line[b] != ',')
                ++b;
            RectBoldText[index].y = atoi(line.substr(a,b-a).c_str());
            a = ++b;
            while(line[b] != ',')
                ++b;
            RectBoldText[index].w = atoi(line.substr(a,b-a).c_str());
            a = ++b;
            b = line.length();
            RectBoldText[index].h = atoi(line.substr(a,b-a).c_str());

            //printf("%i,%i,%i,%i\n",RectSmallText[index].x,RectSmallText[index].y,RectSmallText[index].w,RectSmallText[index].h);

        }
        ifile.close();
    }
    return true;
}
