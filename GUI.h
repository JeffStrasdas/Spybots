#ifndef GUI_H
#define GUI_H

#include "EHead.h"
#include "IMG.h"
#include "Text.h"

//Coordinates for the Logout-Button
const int LOGOUT_X      = 573;
const int LOGOUT_Y      = 14;
const int LOGOUT_W      = 70;
//Actionbox measures
const int ACTIONBOX_X   = 10;
const int ACTIONBOX_Y   = 250;
//
const int UNDO_X        = 10;
const int UNDO_Y        = 387;


namespace GUI
{


class side_screen
{
public:
    side_screen(int* GameMode, SDL_Renderer* Renderer, Text::TextRenderer* TextRenderer,std::string player_name,Entity::ListElement* Available_Programs) : GameMode(GameMode), Renderer(Renderer),TextRenderer(TextRenderer), AvailablePrograms(Available_Programs)
    {
        //Set up active objects
        ActiveEntity = NULL;
        ActiveAction = NULL;

        LoadMedia(player_name);
    };
    ~side_screen()
    {
        UITexture.free();
        ActiveEntity = NULL;
        ActiveAction = NULL;
        Renderer     = NULL;
    };

    //Begins databattle
    void Begin();

    void* newClick(int mx, int my,bool Action = true);

    Entity::Action* Click(int x,int y);
    void Motion(int mx,int my);

    void SetActive(Entity::EHead* entity);
    void SetActive(Entity::Action* action);

    void SwapGameMode();

    //Draws area on screen
    void Draw();

private:
    SDL_Rect* Choose_Rect(int number, bool hovering = false);
    bool LoadMedia(std::string player_name);

    bool hovering[4] = {false,false,false,false};
    int* GameMode; // 0 -> Initializing    1 -> Player turn    2 -> Enemy turn    3 -> Battle over
    //Active objects
    Entity::EHead*  ActiveEntity;
    Entity::Action* ActiveAction;
    //Available Programs
    Entity::ListElement* AvailablePrograms;

    LTexture UITexture;

    SDL_Renderer* Renderer;

    Text::TextRenderer* TextRenderer;
};
}

#endif
