#ifndef GAME_H
#define GAME_H

#include "Scene.h"
#include "Overworld.h"
#include "MainMenu.h"

namespace Game
{
enum Mode {GM_Intro,GM_Map,GM_Game};

class Container
{
public:
    Container()
    {
        Renderer = NULL;
        Window   = NULL;
        intro = NULL;
        hub   = NULL;
        stage = NULL;
        Init();
        GM = GM_Intro;
        //Initialize player data
        PlayerProfile = "";
        PlayerMoney = new int;
        *PlayerMoney = 0;
        Security_Clearance = new int;
        *Security_Clearance = 0;
        PlayerPrograms = NULL;
        //set up remaining objects
        TextRenderer = new Text::TextRenderer(Renderer,"SmallText","SmallText");
        intro = new MainMenu::Menu(Renderer,TextRenderer);
    };
    ~Container()
    {
        delete PlayerMoney;
        delete Security_Clearance;
        delete PlayerPrograms;
        //deleting renderer and window?
        delete TextRenderer;
        delete intro;
        delete hub;
        delete stage;
        //close();
    };

    // First show player select screen
    // Then Overworld
    // followed by stage
    // then overworld again
    void SwapGame(Mode GameMode);

    bool Init();
    bool Update(int delay);

private:
    void LoadProfile();
    void SaveProgress();

    SDL_Renderer* Renderer;
    SDL_Window*   Window;

    Mode GM;
    //player data
    std::string PlayerProfile;
    std::string stage_name;
    int* PlayerMoney;
    int* Security_Clearance;
    //Available programs
    Entity::ListElement* PlayerPrograms;
    //Available nodes
    Overworld::Node* PlayerNodes;

    //Game mode objects
    MainMenu::Menu* intro;
    Overworld::Map* hub;
    Scene::Overlay* stage;

    Text::TextRenderer* TextRenderer;
};
}
#endif
