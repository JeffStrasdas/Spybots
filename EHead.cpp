#include<fstream>
#include<stdlib.h>
#include<cstdlib>

#include "EHead.h"
#include "Text.h"

namespace Entity
{

    void EHead::Apply()
    {
        for(int i=0;i<max_size;i++)
        {
            body[i].old_x = body[i].x;
            body[i].old_y = body[i].y;
        }
        Set_Sprites();
    }
    void EHead::Revert(bool** Useable)
    {
        //Free new positions
        for(int i=0;i<max_size;++i)
            if(body[i].x != -1)
                Useable[body[i].x][body[i].y] = true;
            else
                break;

        //block old positions, revert
        for(int i=0;i<max_size;++i)
        {
            //printf("clearing (%i,%i), blocking (%i,%i)\n",body[i].x,body[i].y,body[i].old_x,body[i].old_y);

            if(body[i].old_x != -1)
                Useable[body[i].old_x][body[i].old_y] = false;

            body[i].x = body[i].old_x;
            body[i].y = body[i].old_y;
        }
        moved = 0;
        Set_Sprites();
    }

    void EHead::SetNext(EHead* _next)
    {
        next = _next;
    }
    void EHead::SetPrev(EHead* _prev)
    {
        prev = _prev;
    }

    void EHead::Disconnect()
    {
        if(prev != NULL)
            prev->next = next;
        if(next != NULL)
            next->prev = prev;
        next = NULL;
        prev = NULL;
    }

    void EHead::AddElement(int _x, int _y, bool** Useable, bool initialising)
    {
        if(Useable != NULL)
        {
            Useable[_x][_y] = false;
            if(currentsize() == max_size)
                Useable[body[max_size-1].x][body[max_size-1].y] = true;
        }

        if(initialising)
        {
            for(int i=1;i<max_size;i++)
                body[max_size-i] = body[max_size-i-1];

            body[0].old_x = _x;
            body[0].old_y = _y;
        }
        else
        {
            for(int i=1;i<max_size;++i)
            {
                body[max_size-i].x = body[max_size-i-1].x;
                body[max_size-i].y = body[max_size-i-1].y;
            }
        }
        body[0].x = _x;
        body[0].y = _y;
    }

    void EHead::Move(int _x,int _y, bool** Useable)
    {
        if(moved>=speed)
            return;
        if(Useable != NULL)
            Useable[_x][_y] = false;
        int new_field = -1;
        for(int i=0;i<currentsize();i++)
        {
            int tempX,tempY;
            GetCoordinates(i,tempX,tempY);
            if(_x == tempX && _y == tempY)
                new_field = i;
        }
        //occupy new field
        if(new_field == -1)
            AddElement(_x,_y,Useable);
        else
        {
            for(int i=0;i<new_field;i++)
            {
                body[new_field-i].x = body[new_field-i-1].x;
                body[new_field-i].y = body[new_field-i-1].y;
            }
            body[0].x = _x;
            body[0].y = _y;
        }
        moved++;
        Set_Sprites();
    }
    void EHead::GetCoordinates(int number, int& _x, int& _y)
    {
        _x = body[number].x;
        _y = body[number].y;
        return;
    }
    int EHead::currentsize()
    {
        int z = 0;
        while(body[z].x > -1 && z<max_size)
                z++;
        return z;
    }
    bool EHead::Check(int _x, int _y)
    {
        for(int i=0;i<currentsize();i++)
            if(body[i].x==_x && body[i].y==_y)
                return true;
        return false;
    }
    EHead* EHead::CheckList(int _x, int _y)
    {
        if(Check(_x,_y))
            return this;
        if(next != NULL)
            return next->CheckList(_x,_y);
        return NULL;
    }
    EHead* EHead::CheckList(int index, int x, int y)
    {
        if(body[index].x == x && body[index].y == y)
            return this;
        if(next != NULL)
            return next->CheckList(index,x,y);
        return NULL;
    }
    void EHead::Set_Sprites()
    {
        if(ID == -1)
            return;
        int Color_Offset = ColorKey()*16;
        int Connection_Offset = ConnectionKey(0);
        Sprite = Color_Offset+Connection_Offset;
        for(int i=0;i<currentsize();i++)
        {
            Connection_Offset = ConnectionKey(i+1);
            body[i].Sprite = Color_Offset+Connection_Offset;
        }
    }
    int EHead::Get_Sprite(int number)
    {
        if(number==0)
            return Sprite;
        number--;
        return body[number].Sprite;
    }
    void EHead::FetchData()
    {
        switch(ID)
        {
        case -1:
            Name        = "Upload zone";
            Description = "Upload your programs here";
            speed       = 0;
            max_size    = 1;
            return;
        case -2:
            Name        = "Credits";
            Description = "Gain brousouf here";
            speed       = 0;
            max_size    = 1;
            return;
        case -3:
            Name        = "Cash money";
            Description = "Get this to complete the mission";
            speed       = 0;
            max_size    = 1;
            return;
        default:
        if(false)
        {
            std::ifstream ifile("data/ID.info");
            std::string line;

            ifile.close();
        }
            break;
        }
        //std::ifstream ifile("data/ID.info");

        //ifile.close();
        //old method
        //Read speed&size from ID-table
        std::ifstream ifile_old("data/ID.bin");
        std::string line;
        speed    = 1;
        max_size = 1;

        if(ifile_old.is_open())
        {
            int current_data = 0;
            int current_Action = 0;
            while(getline(ifile_old,line))
            {
                if(current_data==0)
                {
                    if(ID>9 || ID<0)
                    {
                        if(line.length()==5 && atoi(line.substr(3,2).c_str())==ID)
                            current_data = 1;
                    }
                    else
                        if(line.length()==4 && atoi(line.substr(3,1).c_str())==ID)
                            current_data = 1;

                }
                else
                {
                    if(line.length()==0 || line[0] == '#')
                        break;
                    switch(current_data)
                    {
                    case 1:
                        Name = line;
                        break;
                    case 2:
                        Description = line;
                        break;
                    case 3:
                        speed = atoi(line.substr(line.length()-2,2).c_str());
                        break;
                    case 4:
                        max_size = atoi(line.substr(line.length()-2,2).c_str());
                        break;
                    case 5:
                        ActionElement[current_Action].Name = line;
                        break;
                    case 6:
                        ActionElement[current_Action].Type = atoi(line.substr(line.length()-2,2).c_str());
                        break;
                    case 7:
                        ActionElement[current_Action].Range = atoi(line.substr(line.length()-2,2).c_str());
                        break;
                    case 8:
                        ActionElement[current_Action].Ammount = atoi(line.substr(line.length()-2,2).c_str());
                        break;
                    case 9:
                        ActionElement[current_Action].Requirement = atoi(line.substr(line.length()-2,2).c_str());
                        break;
                    case 10:
                        ActionElement[current_Action].Cost = atoi(line.substr(line.length()-2,2).c_str());
                        break;
                    case 11:
                        ActionElement[current_Action].Description = line;
                        break;
                    case 12:
                        current_Action++;
                        current_data = 5;
                        ActionElement[current_Action].Name = line;
                        break;
                    }

                    current_data++;
                }

            }
            ifile_old.close();
        }

    }
    int EHead::ColorKey()
    {
        if(ID<0)
            return -1;
        if(ID<3)
            return 0;
        if(ID<6)
            return 1;
        if(ID<9)
            return 2;
        if(ID<15)
            return 3;
        if(ID<18)
            return 4;
        if(ID<21)
            return 5;
        if(ID<24)
            return 6;
        if(ID<30)
            return 7;
        if(ID<33)
            return 8;
        if(ID<36)
            return 9;
        if(ID<39)
            return 10;
        if(ID<44)
            return 11;
        if(ID<45)
            return 12;
        if(ID<48)
            return 13;
        if(ID<51)
            return 14;
        if(ID<53)
            return 15;
        if(ID<54)
            return 16;
        return 17;
    }
    int EHead::ConnectionKey(int number)
    {
        //Find Connections
        int _x,_y;
        bool up=false,down=false,left=false,right=false;
        GetCoordinates(number,_x,_y);
        for(int i=0;i<currentsize()+1;i++)
        {
            if(i!=number)
            {
                int tempX,tempY;
                GetCoordinates(i,tempX,tempY);
                if(tempX+1 == _x && tempY == _y)
                    left = true;
                else
                if(tempX-1 == _x && tempY == _y)
                    right = true;
                else
                if(tempX == _x && tempY+1 == _y)
                    up = true;
                else
                if(tempX == _x && tempY-1 == _y)
                    down = true;
            }
        }
        //Set Offset
        if(up && down && left && right)
            return 0;
        if(!up && down && left && right)
            return 1;
        if(up && down && left && !right)
            return 2;
        if(up && !down && left && right)
            return 3;
        if(up && down && !left && right)
            return 4;
        if(up && down && !left && !right)
            return 5;
        if(!up && !down && left && right)
            return 6;
        if(up && !down && left && !right)
            return 7;
        if(up && !down && !left && right)
            return 8;
        if(!up && down && !left && right)
            return 9;
        if(!up && down && left && !right)
            return 10;
        if(!up && !down && left && !right)
            return 11;
        if(up && !down && !left && !right)
            return 12;
        if(!up && !down && !left && right)
            return 13;
        if(!up && down && !left && !right)
            return 14;
        return 15;
    }
    //true if still alive
    bool EHead::Damage(int Ammount, bool** Useable)
    {
        if(Ammount >= currentsize())
        {
            int beginningsize = currentsize();
            for(int i=0;i<beginningsize;i++)
            {
                if(Useable != NULL)
                    Useable[body[i].x][body[i].y] = true;
                body[i].x = -1;
                body[i].y = -1;
                body[i].old_x = -1;
                body[i].old_y = -1;
                body[i].Sprite = 1;
            }
            return false;
        }
        int beginningsize = currentsize();
        for(int i=beginningsize-1;i>beginningsize-1-Ammount;i--)
        {
            if(Useable != NULL)
                Useable[body[i].x][body[i].y] = true;
            body[i].x = -1;
            body[i].y = -1;
            body[i].old_x = -1;
            body[i].old_y = -1;
            body[i].Sprite = 1;
        }
        return true;
    }

    void EHead::ChangeSpeed(int Ammount)
    {
        speed += Ammount;
        if(speed < 0)
            speed = 0;
        else
            if(speed > 10)
                speed = 10;
    }

    void EHead::Heal(int Ammount, bool** Useable)
    {}

    //Draw Text
    void EHead::DrawText(Action* ActiveAction, Text::TextRenderer* TextRenderer)
    {
        if(ID > -1)
        {
            //Speed
            char _intValue[2];
            itoa(speed,_intValue,10);
            std::string actualText = "Move:";
            actualText += _intValue;
            TextRenderer->Write(51,184,actualText,true);
            //TextRenderer->Draw(51,179,actualText);
            //Text::Draw(gRenderer,51,179,actualText,SmallFont);
            //Max Size
            itoa(max_size,_intValue,10);
            actualText = "Max Size:";
            actualText += _intValue;
            TextRenderer->Write(51,195,actualText,true);
            //TextRenderer->Draw(51,190,actualText);
            //Text::Draw(gRenderer,51,190,actualText,SmallFont);
            //Current Size
            itoa(currentsize(),_intValue,10);
            actualText = "Current Size:";
            actualText += _intValue;
            TextRenderer->Write(51,206,actualText,true);
            //TextRenderer->Draw(51,201,actualText);
            //Text::Draw(gRenderer,51,201,actualText,SmallFont);

            //action fields
            for(int i=0;i<ActionCount();i++)
                TextRenderer->Write(75-4*ActionElement[i].Name.length(),258+i*20,ActionElement[i].Name,true);
                //TextRenderer->Draw(75-4*ActionElement[i].Name.length(),253+i*20,ActionElement[i].Name);
                //Text::Draw(gRenderer,75-4*ActionElement[i].Name.length(),253+i*20,ActionElement[i].Name,SmallFont);
        }
        //Name
        TextRenderer->Write(16,217,Name,false);
        //TextRenderer->Draw(16,217,Name,false);
        //Text::Draw(gRenderer,16,217,Name,BoldFont);
        //Description
        if(ActiveAction == NULL)
            TextRenderer->Write(12,319,Description,true,100);
            //TextRenderer->Draw(12,314,Description);
            //Text::Draw(gRenderer,12,314,Description,SmallFont);
        else
        {
            TextRenderer->Write(12,319,ActiveAction->Name,true);
            TextRenderer->Write(12,329,ActiveAction->Description,true,100);
            //TextRenderer->Draw(12,314,ActiveAction->Name);
            //TextRenderer->Draw(12,324,ActiveAction->Description);
            //Text::Draw(gRenderer,12,314,ActiveAction->Name,SmallFont);
            //Text::Draw(gRenderer,12,324,ActiveAction->Description,SmallFont);
        }
    }

    //Draw Text
    /*void EHead::DrawText(Action* ActiveAction,SDL_Renderer* gRenderer,TTF_Font* SmallFont, TTF_Font* BoldFont)
    {
        if(ID > -1)
        {
            //Speed
            char _intValue[2];
            itoa(speed,_intValue,10);
            std::string actualText = "Move:";
            actualText += _intValue;
            Text::Draw(gRenderer,51,179,actualText,SmallFont);
            //Max Size
            itoa(max_size,_intValue,10);
            actualText = "Max Size:";
            actualText += _intValue;
            Text::Draw(gRenderer,51,190,actualText,SmallFont);
            //Current Size
            itoa(currentsize(),_intValue,10);
            actualText = "Current Size:";
            actualText += _intValue;
            Text::Draw(gRenderer,51,201,actualText,SmallFont);

            //action fields
            for(int i=0;i<ActionCount();i++)
                Text::Draw(gRenderer,75-4*ActionElement[i].Name.length(),253+i*20,ActionElement[i].Name,SmallFont);
        }
        //Name
        Text::Draw(gRenderer,16,217,Name,BoldFont);
        //Description
        if(ActiveAction == NULL)
            Text::Draw(gRenderer,12,314,Description,SmallFont);
        else
        {
            Text::Draw(gRenderer,12,314,ActiveAction->Name,SmallFont);
            Text::Draw(gRenderer,12,324,ActiveAction->Description,SmallFont);
        }

    }*/


    /*---Returning data---*/

    EHead* EHead::ReturnNext()
    {
        return next;
    }
    EHead* EHead::ReturnPrev()
    {
        return prev;
    }
    int EHead::ReturnSpeed()
    {
        return speed;
    }
    int EHead::ReturnMoved()
    {
        return moved;
    }
    int EHead::ReturnMaxSize()
    {
        return max_size;
    }
    int EHead::ActionCount()
    {
        if(ActionElement[0].Type==-1)
            return 0;
        if(ActionElement[1].Type==-1)
            return 1;
        return 2;
    }

    int EHead::ReturnActionAmmount(int index)
    {
        return ActionElement[index].Ammount;
    }

    int EHead::ReturnActionRange(int index)
    {
        return ActionElement[index].Range;
    }
    int EHead::ReturnActionType(int index)
    {
        return ActionElement[index].Type;
    }

    std::string EHead::ReturnName()
    {
        return Name;
    }
    std::string EHead::ReturnDescription()
    {
        return Description;
    }
    std::string EHead::ReturnActionName(int index)
    {
        return ActionElement[index].Name;
    }
    std::string EHead::ReturnActionDescription(int index)
    {
        return ActionElement[index].Description;
    }
    EHead* EHead::ReturnFirst()
    {
        if(prev != NULL)
            return prev->ReturnFirst();
        else
            return this;
    }
    EHead* EHead::ReturnLast()
    {
        if(next != NULL)
            return next->ReturnLast();
        else
            return this;
    }
    Action* EHead::ReturnAction(int index)
    {
        return &ActionElement[index];
    }
    int EHead::ReturnChainLength()
    {
        if(next != NULL)
            return next->ReturnChainLength()+1;
        return 1;
    }

    bool EHead::CanControl()
    {
        return controllable;
    }

    bool EHead::CanMove()
    {
        return moved < speed;
    }

    bool EHead::Activate()
    {
        return active;
    }
    void EHead::Activate(bool act)
    {
        active = act;
        moved = 0;
    }
    //AI
    EHead* EHead::AI_Random(EHead* ATarget, bool** Useable)
    {
        if(moved >= speed)
        {
            active = false;
            return NULL;
        }
        std::srand(SDL_GetTicks());
        printf("Do ai stuff\t%i -- %i\n",moved,speed);
        int tries = 0;
        int _x,_y;
        while(true)
        {
            if(tries > 5)
                break;
            int c = std::rand()%4;
            //printf("Try to move %s\n", c == 0 ? "left" : c == 1 ? "right" : c == 2 ? "up" : "down");
            switch(c)
            {
            case 0:
                _x = body[0].x-1;
                _y = body[0].y;
                break;
            case 1:
                _x = body[0].x+1;
                _y = body[0].y;
                break;
            case 2:
                _x = body[0].x;
                _y = body[0].y-1;
                break;
            case 3:
                _x = body[0].x;
                _y = body[0].y+1;
                break;
            }
            //using FIELD_WIDTH AND FIELD_HEIGHT -- change asap!
            if(_x <= -1 || _x >= 14 || _y <= -1 || _y >= 11)
            {
                tries++;
                continue;
            }
            if(Useable[_x][_y])
            {
                Move(_x,_y,Useable);
                return NULL;
            }
            else
                if(Check(_x,_y))
                {
                    Move(_x,_y,Useable);
                    return NULL;
                }
            tries++;
        }
        active = false;
        return NULL;
    }
    EHead* EHead::AI_Linear(EHead* ATarget,int** Distance, bool** Useable)
    {

        for(int i= 0;i<max_size;++i)
        {
            if(body[i].x == -1)
                break;
            Distance[body[i].x][body[i].y]++;
        }
        /*
        //print Distance
        for(int y= 0;y<11;y++)
        {
            for(int x=0;x<14;x++)
                Distance[x][y] == -1 ? printf(" .") : Distance[x][y] > 9 ? printf(" x") : printf(" %i",Distance[x][y]);
            printf("\n");
        }*/

        if(moved >= speed)
        {
            active = false;
            return AI_Attack(ATarget,Useable);
        }
        else
        {
            int dir[4];
            int x = body[0].x , y = body[0].y;
            //up down left right
            y-1 >= 0 ? dir[0] = Distance[x][y-1] : dir[0] = -1;
            y+1 < 11 ? dir[1] = Distance[x][y+1] : dir[1] = -1;
            x-1 >= 0 ? dir[2] = Distance[x-1][y] : dir[2] = -1;
            x+1 < 14 ? dir[3] = Distance[x+1][y] : dir[3] = -1;
            //printf("%i,%i,%i,%i\n",dir[0],dir[1],dir[2],dir[3]);
            //find minimal distance
            int minimal = -1;
            bool contact = false;
            for(int i=0;i<4;++i)
            {
                if(dir[i] == 0)
                {
                    contact = true;
                    continue;
                }
                if(dir[i] > 0 && (minimal == -1 || dir[i] < dir[minimal]))
                    minimal = i;
            }
            //handle edge case where enemy is next to player
            if(dir[minimal] > speed-moved && (dir[0] == 0 || dir[1] == 0 || dir[2] == 0 || dir[3] == 0))
                return AI_Attack(ATarget,Useable);
            switch(minimal)
            {
            case 0:
                Move(x,y-1,Useable);
                return NULL;
            case 1:
                Move(x,y+1,Useable);
                return NULL;
            case 2:
                Move(x-1,y,Useable);
                return NULL;
            case 3:
                Move(x+1,y,Useable);
                return NULL;
            }
            active = false;
            return AI_Attack(ATarget,Useable);
        }
    }
    EHead* EHead::AI_Attack(EHead* ATarget, bool** Useable)
    {
        printf("Attack\n");
        ListElement* TargetList = NULL;
        EHead*       tempE      = NULL;
        for(int x = body[0].x - ActionElement[0].Range,y; x <= body[0].x + ActionElement[0].Range ; x++)
            for(y = body[0].y - ActionElement[0].Range;   y <= body[0].y + ActionElement[0].Range ; y++)
            {
                if(x >= 0 && y >= 0 && x < 14 && y < 11 && abs(x - body[0].x)+abs(y - body[0].y) <= ActionElement[0].Range)
                {
                    tempE = ATarget->CheckList(x,y);
                    if(tempE != NULL)
                    {
                        //Add target to list
                        if(TargetList == NULL)
                        {
                            TargetList = new ListElement;
                            TargetList->amount  = 0;
                            TargetList->element = tempE;
                            TargetList->next    = NULL;
                            TargetList->prev    = TargetList;
                        }
                        else
                        {
                            //find last element on list
                            ListElement* tempL = TargetList;
                            while(tempL->next != NULL)
                                tempL = tempL->next;
                            tempL->next = new ListElement;
                            tempL->next->prev    = tempL;
                            tempL->next->next    = NULL;
                            tempL->next->element = tempE;
                            tempL->next->amount  = tempL->amount + 1;

                            TargetList->prev = tempL->next;
                        }
                    }
                }
            }
        //Attack random target
        if(TargetList != NULL)
        {
            std::srand(SDL_GetTicks());
            int c = std::rand()%(TargetList->prev->amount+1);
            //printf("find %i; %i\n",c,TargetList->prev->amount);
            ListElement* tempL = TargetList;
            while(tempL->amount != c)
            {
                tempL = tempL->next;
            }
            active = false;
            printf("Attack %s with %i\n",tempL->element->ReturnName().c_str(),ActionElement[0].Ammount);
            tempE = tempL->element;
            //Clear up memory
            tempL = TargetList->next;
            while(tempL != NULL)
            {
                delete TargetList;
                TargetList = tempL;
                tempL = tempL->next;
            }
            delete TargetList;
            printf("success\n");
            //apply damage
            if(!tempE->Damage(ActionElement[0].Ammount,Useable))
                return tempE;

            return NULL;
        }
        active = false;
        return NULL;
    }
}
