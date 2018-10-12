#ifndef EHEAD_H
#define EHEAD_H
#include <string>
#include <cstdio>
#include <SDL.h>
#include <SDL_ttf.h>

#include "Text.h"

namespace Entity
{

struct Element
{
    int x,y,Sprite;
    int old_x,old_y;
    Element() {x=-1;y=-1;Sprite=1;old_x=-1;old_y=-1;};
};

struct Action
{
    std::string Name;
    std::string Description;
    int Type;
    int Range, Ammount;
    int Requirement, Cost;
    Action() {Name="NO_NAME";Description="NO_DESCR";Type=-1;Range=0;Ammount=0;Requirement=0;Cost=0;};
};
/*
// NEW WAY
class EntityHead
{
public:
private:
};
class PlayerHead : public EntityHead
{
public:
    //Undo button action
    void Apply();
    void Revert(bool** Useable);

private:

};
class ObjectHead : public EntityHead
{
};
class EnemyHead : public EntityHead
{
    public:
    //AI-Relevant functions, returns destroyed entity
    EntityHead* AI_Random(EntityHead* ATarget, bool** Useable);
    EntityHead* AI_Linear(EntityHead* ATarget, int** Distance, bool** Useable);
    EntityHead* AI_Attack(EntityHead* ATarget, bool** Useable);
};*/
// OLD WAY
class EHead
{
public:
    EHead(int ID, int x = -1,int y = -1,bool control = true, bool** Useable = NULL, EHead* previous = NULL):ID(ID),controllable(control),prev(previous)
    {
        Name        = "NO_NAME";
        Description = "NO_DESCR";
        Sprite      = 0;
        FetchData();
        body        = new Element[max_size];
        body[0].x = body[0].old_x = x;
        body[0].y = body[0].old_y = y;
        if(x == -1)
            active  = false;
        else
            active  = true;
        moved       = 0;
        next        = NULL;
        if(previous != NULL)
            previous->next = this;
        if(Useable != NULL)
            Useable[x][y] = false;
        Set_Sprites();
    }
    ~EHead()
    {
        //printf("deleting %s\n",Name.c_str());
        delete[] body;
        if(next != NULL)
            next->prev = prev;
        if(prev != NULL)
            prev->next = next;
    }
    //Concerning UNDO
    void Apply();
    void Revert(bool** Useable);

    //Adds Entity as next
    void SetNext(EHead* _next);
    //Adds Entity as prev
    void SetPrev(EHead* _prev);
    //Removes Entity from current list
    void Disconnect();
    //Moves every Element up one step and adds the new Element at the 0th index
    void AddElement(int _x, int _y, bool** Useable = NULL, bool initialising = false);
    //Handles Movement
    void Move(int _x,int _y, bool** Useable = NULL);
    //Returns the x|y coordinates of given Element
    void GetCoordinates(int number, int& _x, int& _y);
    //Returns the size of the booltity
    int currentsize();
    //Checks for any other Entities
    bool Check(int _x, int _y);
    //Returns relevant entity or NULL if none
    EHead* CheckList(int _x,int _y);
    EHead* CheckList(int index, int x,int y);
    //Determines Color-based Offset, Connection-based Offset
    int ColorKey();
    int ConnectionKey(int number);
    //Updates every Sprite
    void Set_Sprites();
    //Returns the saved Sprites
    int Get_Sprite(int number);
    //Loads the data of the Entity
    void FetchData();
    //Delete the oldest [Ammount] Elements
    bool Damage(int Ammount, bool** Useable = NULL);
    //Changes Speed
    void ChangeSpeed(int Ammount);
    //Heal
    void Heal(int Ammount,bool** Useable);
    //Draw Text
    void DrawText(Action* ActiveAction, Text::TextRenderer* TextRenderer);
    //Draw Text
    //void DrawText(Action* ActiveAction,SDL_Renderer* gRenderer,TTF_Font* Font, TTF_Font* BoldFont);

    //AI-Relevant functions, returns destroyed entity
    EHead* AI_Random(EHead* ATarget, bool** Useable);
    EHead* AI_Linear(EHead* ATarget, int** Distance, bool** Useable);
    EHead* AI_Attack(EHead* ATarget, bool** Useable);
    // ----

    //Returning Data
    EHead* ReturnNext();
    EHead* ReturnPrev();
    int ReturnSpeed();
    int ReturnMoved();
    int ReturnMaxSize();
    int ActionCount();

    int ReturnActionAmmount(int index);
    int ReturnActionRange(int index);
    int ReturnActionType(int index);

    std::string ReturnName();
    std::string ReturnDescription();
    std::string ReturnActionName(int index);
    std::string ReturnActionDescription(int index);

    EHead* ReturnFirst();
    EHead* ReturnLast();

    Action* ReturnAction(int index);

    int ReturnChainLength();

    //useless
    bool CanControl();
    //actually useful
    bool CanMove();

    //returns active-status
    bool Activate();
    //changes active-status
    void Activate(bool act);


    int ID;
private:
    //int x,y;
    std::string Name;
    std::string Description;
    int Sprite;
    int max_size,speed;
    Action ActionElement[2];
    Element* body;
    bool active;
    //probably useless?
    bool controllable;
    //
    int moved;
    EHead* next;
    EHead* prev;
};

struct ListElement
{
    EHead*       element;
    int          amount;
    ListElement* next;
    ListElement* prev;
};

}

#endif
