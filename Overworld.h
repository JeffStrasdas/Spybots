#ifndef OVERWORLD_H
#define OVERWORLD_H

#include "IMG.h"
#include "Text.h"
#include <cstdlib>

namespace Overworld
{
enum NodeState{Inactive, Highlighted, ShowIntro, ShopWindow};
class Node
{
public:
    Node(int ID, bool Available, Node* previous = NULL):ID(ID), available(Available)
    {
        next = NULL;
        if(previous != NULL)
            previous->next = this;
        LoadData();
        //printf("node %i loaded! ",ID);
    }
    void LoadData();
    Node(int x,int y, int ID, std::string Name, std::string Description, bool Available, Node* previous = NULL):x(x),y(y),ID(ID),StageName(Name),StageDescription(Description),available(Available)
    {
        printf("build node %i\t",ID);
        state = Inactive;

        next = NULL;
        //soon to be obsolete
        active = highlighted = false;
        if(previous != NULL)
            previous->next = this;
    }
    ~Node()
    {
        printf("delete Node %i\n", ID);
        delete next;
    }

    void Draw(int screen_x, int screen_y,Text::TextRenderer* Text);
    void testAdd(Node* ToAdd)
    {
        if(next != NULL)
            next->testAdd(ToAdd);
        else
            next = ToAdd;
    }
    void Add(int x,int y,int id,std::string Name,std::string Description,bool Available)
    {
        if(next != NULL)
            next->Add(x,y,id,Name,Description,Available);
        else
            next = new Node(x,y,id,Name,Description, Available);
    }
    Node* Motion(int mx, int my);
    Node* Select(int mx, int my);
    bool Check(int mx, int my);

    void GetCoordinates(int& x, int& y);
    void Deactivate();
    bool IsActive();
    bool LoadStage(int mx, int my);
    std::string ReturnName();
    int ReturnID()
    {
        return ID;
    }
    bool IsAvailable()
    {
        return available;
    }
    Node* ReturnNext()
    {
        return next;
    }
private:
    void DeactivateFollowing();
    NodeState state;
    bool available;
    bool highlighted, active;
    int x,y,ID,type,Security_level,Rewards;
    std::string StageName;
    std::string StageDescription;
    Node* next;
};
//Branching
class Shop : public Node
{
public:
    Shop(int x,int y, int ID, std::string Name, std::string Description, bool Available, Node* previous = NULL) : Node(x,y,ID,Name,Description,Available,previous)
    {

        printf("Shop built!\n");
    }
    ~Shop()
    {
        printf("Shop deleted\n");
    }
    //void Add(int x, int y, int id, std::string Name, std::string Description, bool Available);
    //Mouse input
    //Node* Motion(int mx, int my);
    //Node* Select(int mx, int my);
private:
};
class Stage : public Node
{
public:
    Stage(int x,int y, int ID, std::string Name, std::string Description, bool Available, Node* previous = NULL) : Node(x,y,ID,Name,Description,Available,previous)
    {
        printf("Stage built!\n");
    }
    ~Stage()
    {
        printf("Stage deleted\n");
    }
private:
};
//--------
const int speed      = 10;
const int NodeWidth  = 50;
const int NodeHeight = 75;
class Map
{
public:
    Map(SDL_Renderer* renderer,Text::TextRenderer* text,std::string PlayerProfile, int* PlayerMoney, int* Security, Node* AvailableNodes):Renderer(renderer) , Text(text), Player_Cash(PlayerMoney), Security_Clearance(Security), ANode(AvailableNodes)
    {
        for(int i=0;i<4;++i)
            direction[i] = false;
        MoveToActive = false;
        Mapview.x = Mapview.y = 0;
        Mapview.w = SCREEN_WIDTH;
        Mapview.h = SCREEN_HEIGHT;
        TargetX = -1;
        TargetY = -1;
        ActiveNode = NULL;
        Player_Cash = 0;
        Security_Clearance = 0;
        loadMedia(PlayerProfile);
    }
    ~Map()
    {
        delete ANode;
        TextureMap.free();
        Renderer = NULL;
        Text = NULL;
        ActiveNode = ANode = NULL;
    }
    void Deactivate();
    //Mouse handling
    bool Click();
    void Motion();
    //Map movement
    void UpdateMap();
    std::string Update(int delay);

    //Returns data
    Node* ReturnANode()
    {
        return ANode;
    }

private:
    bool loadMedia(std::string PlayerProfile);
    //bool LoadLine(std::string& Name, int& type, int& x, int& y, std::string& Description);
    void Render();

    SDL_Renderer*   Renderer = NULL;
    Text::TextRenderer* Text = NULL;
    //Event handler
    SDL_Event e;

    LTexture TextureMap;
    SDL_Rect Mapview;
    int MapX,MapY;
    int TargetX,TargetY;
    int mx,my;
    bool direction[4];
    bool MoveToActive;

    int* Player_Cash;
    int* Security_Clearance;

    Node* ANode;
    Node* ActiveNode;
};
}

#endif
