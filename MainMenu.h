#ifndef MAINMENU_H
#define MAINMENU_H

#include <string>

#include "Text.h"
#include "MouseInteraction.h"

namespace MainMenu
{
enum MainMenuMode{START_SCREEN,CHOOSE_PROFILE,NEW_PROFILE};

class Menu
{
public:
    Menu(SDL_Renderer* renderer, Text::TextRenderer* text): Renderer(renderer), TextRenderer(text)
    {
        profilecount = 0;
        PlayerName = "";
        Profiles   = NULL;
        loadMedia();
    };
    ~Menu()
    {
        Renderer     = NULL;
        TextRenderer = NULL;
        delete[] Profiles;
    }
    //Mouse handling
    bool Click();
    void Motion();
    //Select Profile
    bool Key(SDL_Keycode key);
    void Confirm();

    std::string Update(int delay);

private:
    bool loadMedia();
    void Render();
    //Info
    std::string PlayerName;

    //List of already created profiles
    std::string* Profiles;
    int profilecount;
    //Index of selected profile
    int ActiveProfile;
    //Event handler
    SDL_Event e;
    //Mouse Positions
    int mx,my;
    SDL_Renderer* Renderer;
    Text::TextRenderer* TextRenderer;

    LTexture TextureIntro;
    LTexture TextureSelection;
};
}

#endif
