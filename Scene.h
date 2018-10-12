#ifndef SCENE_H
#define SCENE_H

#include "GUI.h"
#include "Field.h"


//How long should the message between turns show up?
const int MESSAGE_DELAY = 1000;

namespace Scene
{


class Overlay
{
public:
    Overlay(SDL_Renderer* Renderer,Text::TextRenderer* TextRenderer, Entity::ListElement* AvailablePrograms,std::string stage_name = "1", std::string player_name = "1"):Renderer(Renderer),TextR(TextRenderer)
    {
        GameMode   = new int;
        *GameMode  = 0;
        ShowLogoutMenu = false;
        logoutHover[0] = logoutHover[1] = false;
        NoActive   = false;
        ChangeSize = 0;
        delta_t    = 0;

        Rewards   = new field::GameRewards;
        area      = new field::Area(GameMode,Renderer,stage_name);
        left_side = new GUI::side_screen(GameMode,Renderer,TextR,player_name,AvailablePrograms);

        LoadMedia();
        printf("game mode %i - %s\n",*GameMode, *GameMode == 0 ? "initializing" : *GameMode == 1 ? "player's turn" : "other");
        //printf("dumping field.....");
        //area->Dump_Field("1");
    };
    ~Overlay()
    {
        delete GameMode;
        delete Rewards;
        delete area;
        delete left_side;

        LogOutTexture.free();
        BorderTexture.free();
        BackgroundTexture.free();

        GameMode  = NULL;
        Renderer  = NULL;
    };
    void Draw();
    bool Click(int x,int y);
    void Motion(int mx,int my);

    field::GameRewards* MatchResults();

    void Begin();

    bool Update(int delay);

    void SwapGameMode(int delay);
    void EarlyUpdate(int delay);
    bool LateUpdate();

private:
    bool LoadMedia();

    //Container for rewards
    field::GameRewards* Rewards;

    //Shows the logout menu
    bool ShowLogoutMenu;
    //Whether mouse is hovering over an logout option
    bool logoutHover[2];
    //true if the game mode should be changed
    bool NoActive;
    //Heal active entity
    int  ChangeSize;
    // 0 - initializing; 1 - Players' turn; 2 - Enemy's turn; 3 - story/exposition
    int* GameMode;
    //time since last action
    int delta_t;
    //Mouse position
    int mx,my;
    //Event handler
    SDL_Event e;


    GUI::side_screen* left_side;
    field::Area* area;

    SDL_Renderer* Renderer;
    Text::TextRenderer* TextR;

    LTexture BackgroundTexture;
    LTexture LogOutTexture;
    LTexture BorderTexture;

};
}
#endif
