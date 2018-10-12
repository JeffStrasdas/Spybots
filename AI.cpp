#include "AI.h"
#include "EHead.h"

#include <SDL.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "EHead.h"
/*
bool AI::Linear(bool aggressive, Entity::EHead** EList, int Index,int* Current, int** Field)
{
    //Can I move?
    if(EList[Index]->ReturnMoved()<EList[Index]->ReturnSpeed())
    {
        //Am I not alone?
        if(*Current > 1)
        {
            //What's the distance to the entities?
            int x,y,tempx,tempy;
            int* distance = new int[*Current];
            EList[Index]->GetCoordinates(0,x,y);
            for(int i = 0;i<*Current;i++)
            {
                if(i != Index && EList[i]->CanControl())
                {
                    int length = EList[i]->currentsize();
                    EList[i]->GetCoordinates(0,tempx,tempy);
                    distance[i] = abs(x-tempx)+abs(y-tempy);
                    if(length >1)
                        for(int i=1;i<length;i++)
                        {
                            EList[i]->GetCoordinates(i,tempx,tempy);
                            int tempd = abs(x-tempx)+abs(y-tempy);
                            if(tempd < distance[i])
                                distance[i] = tempd;
                        }
                }
                else
                    distance[i] = -1;
                if(distance[i] == 1)
                {
                    delete[] distance;
                    return Attack(0,EList,Index,Current);
                }
            }
            //Are there valid targets?
            int smallest = 0;
            while(distance[smallest] == -1)
                if(smallest == *Current-1)
                {
                    smallest = -1;
                    break;
                }
                else
                    smallest++;
            //No valid target. Do something dumb. (should not occur outside of testing)
            if(smallest == -1)
            {
                delete[] distance;
                EList[Index]->Activate(false);
                return false;
            }
            else
            {
                //Choose target with smallest distance
                if(smallest+1 < *Current)
                    for(int i = smallest+1;i<*Current;i++)
                        if(distance[i]!=-1 && distance[i] < distance[smallest])
                            smallest = i;
                delete[] distance;
                //Choose direction to go
                EList[smallest]->GetCoordinates(0,tempx,tempy);
                int dx = x-tempx, dy = y-tempy, c = 0;
                if(abs(dy) <= abs(dx))
                    if(dx < 0)
                        if(aggressive)
                            c = 1;
                        else
                            c = 0;
                    else
                        if(aggressive)
                            c = 0;
                        else
                            c = 1;
                else
                    if(dy < 0)
                        if(aggressive)
                            c = 3;
                        else
                            c = 2;
                    else
                        if(aggressive)
                            c = 2;
                        else
                            c = 3;
                int tries = 0;
                while (tries < 2)
                {
                    //Check if it's possible
                    switch(c)
                    {
                    case 0:
                        if(Check(EList,Index,Current,Field,x-1,y))
                        {
                            EList[Index]->Move(x-1,y);
                            return false;
                        }
                        else
                        {
                            tries++;
                            if(dy<0)
                                if(aggressive)
                                    c=3;
                                else
                                    c=2;
                            else
                                if(aggressive)
                                    c=2;
                                else
                                    c=3;
                        }
                        break;
                    case 1:
                        if(Check(EList,Index,Current,Field,x+1,y))
                        {
                            EList[Index]->Move(x+1,y);
                            return false;
                        }
                        else
                        {
                            tries++;
                            if(dy<0)
                                if(aggressive)
                                    c=2;
                                else
                                    c=3;
                            else
                                if(aggressive)
                                    c=3;
                                else
                                    c=2;
                        }
                        break;
                    case 2:
                        if(Check(EList,Index,Current,Field,x,y-1))
                        {
                            EList[Index]->Move(x,y-1);
                            return false;
                        }
                        else
                        {
                            tries++;
                            if(dx<0)
                                if(aggressive)
                                    c=1;
                                else
                                    c=0;
                            else
                                if(aggressive)
                                    c=1;
                                else
                                    c=0;
                        }
                        break;
                    case 3:
                        if(Check(EList,Index,Current,Field,x,y+1))
                        {
                            EList[Index]->Move(x,y+1);
                            return false;
                        }
                        else
                        {
                            tries++;
                            if(dx<0)
                                if(aggressive)
                                    c=0;
                                else
                                    c=1;
                            else
                                if(aggressive)
                                    c=0;
                                else
                                    c=1;
                        }
                        break;
                    }
                    //No direct path found
                    if(tries > 1)
                    {
                        printf("/panic/\n");
                        return Idiot(EList,Index,Current,Field);
                    }
                }
            }
        }
    }
    else
        return Attack(0,EList,Index,Current);
    return Idiot(EList,Index,Current,Field);
}

bool AI::Idiot(Entity::EHead** EList, int Index,int* Current, int** Field)
{
    //Can I move?
    if(EList[Index]->ReturnMoved()<EList[Index]->ReturnSpeed())
    {
        //Where can I move?
        bool moveable[5] = {false,false,false,false,false};
        int x,y;
        EList[Index]->GetCoordinates(0,x,y);

        moveable[1] = Check(EList,Index,Current,Field,x-1,y);
        moveable[2] = Check(EList,Index,Current,Field,x,y-1);
        moveable[3] = Check(EList,Index,Current,Field,x+1,y);
        moveable[4] = Check(EList,Index,Current,Field,x,y+1);
        for(int i=1;i<5;i++)
            if(moveable[i])
            {
                moveable[0] = true;
                break;
            }
        //No valid direction, do something else
        if(!moveable[0])
        {
            return Attack(0,EList,Index,Current);
        }
        //Choose random VALID direction
        std::srand(SDL_GetTicks());
        int c = std::rand()%4;
        while(!moveable[c+1])
            c = std::rand()%4;
        //move..
        switch(c)
        {
        case 0:
            EList[Index]->Move(x-1,y);
            //printf("Moving %s(%i)\t %i,%i\t %i,%d\n",EList[Index]->ReturnName().c_str(),Index,x,y,Field[x-1][y],moveable[0]);
            break;
        case 1:
            EList[Index]->Move(x,y-1);
            //printf("Moving %s(%i)\t %i,%i\t %i,%d\n",EList[Index]->ReturnName().c_str(),Index,x,y,Field[x][y-1],moveable[0]);
            break;
        case 2:
            EList[Index]->Move(x+1,y);
            //printf("Moving %s(%i)\t %i,%i\t %i,%d\n",EList[Index]->ReturnName().c_str(),Index,x,y,Field[x+1][y],moveable[0]);
            break;
        case 3:
            EList[Index]->Move(x,y+1);
            //printf("Moving %s(%i)\t %i,%i\t %i,%d\n",EList[Index]->ReturnName().c_str(),Index,x,y,Field[x][y+1],moveable[0]);
            break;
        }
    }
    else
    {
       return Attack(0,EList,Index,Current);
    }
    return Attack(0,EList,Index,Current);
}
bool AI::Attack(int mode, Entity::EHead** EList, int Index,int* Current)
{
    if(EList[Index]->ActionCount() == 0)
    {
        EList[Index]->Activate(false);
        return false;
    }
    Entity::EHead* List[*Current];
    int last = 0;
    int range = EList[Index]->ReturnActionRange(0);
    int x,y;
    EList[Index]->GetCoordinates(0,x,y);
    for(int range_x = -range;range_x <= range;range_x++)
    for(int range_y = -range;range_y <= range;range_y++)
        for(int i=0;i<*Current;i++)
            if(i != Index && EList[i]->CanControl() && abs(range_x)+abs(range_y) <= range && EList[i]->Check(x+range_x,y+range_y))
                List[last++] = EList[i];
    if(last!=0)
    {
        int c = 0;
        switch(mode)
        {
        case 0:
            c = std::rand()%last;
            break;
        case 1:
            if(last > 1)
                for(int i = 1;i<last;i++)
                    if(EList[i]->currentsize()<EList[c]->currentsize())
                        c = i;
            break;
        }
        printf("Attack!\n");
        EList[Index]->Activate(false);
        return !List[c]->Damage(EList[Index]->ReturnActionAmmount(0));
    }
    EList[Index]->Activate(false);
    return false;
}

bool AI::Check(Entity::EHead** EList, int Index,int* Current, int** Field,int x, int y)
{
    bool success = false;
    if(-1<x && x<14 && -1<y && y<11 && Field[x][y] != -1)
    {
        success = true;
        for(int i = 0; i<*Current; i++)
            if(i != Index && EList[i]->Check(x,y))
                success = false;
    }
    return success;
}
*/
//-----------------------------------------------------
/*bool AI::new_Linear(bool aggressive, Entity::EHead** AE_List, Entity::EHead* Current, int** Field)
{
    //Can I move?
    if(Current->ReturnMoved() < Current->ReturnSpeed())
    {
        //Am I not alone?
        if(AE_List[0] != NULL)
        {
            //What's the distance to the entities?
            int x,y,tempx,tempy;
            int* distance = new int[*Current];
            EList[Index]->GetCoordinates(0,x,y);
            for(int i = 0;i<*Current;i++)
            {
                if(i != Index && EList[i]->CanControl())
                {
                    int length = EList[i]->currentsize();
                    EList[i]->GetCoordinates(0,tempx,tempy);
                    distance[i] = abs(x-tempx)+abs(y-tempy);
                    if(length >1)
                        for(int i=1;i<length;i++)
                        {
                            EList[i]->GetCoordinates(i,tempx,tempy);
                            int tempd = abs(x-tempx)+abs(y-tempy);
                            if(tempd < distance[i])
                                distance[i] = tempd;
                        }
                }
                else
                    distance[i] = -1;
                if(distance[i] == 1)
                {
                    delete[] distance;
                    return Attack(0,EList,Index,Current);
                }
            }
            //Are there valid targets?
            int smallest = 0;
            while(distance[smallest] == -1)
                if(smallest == *Current-1)
                {
                    smallest = -1;
                    break;
                }
                else
                    smallest++;
            //No valid target. Do something dumb. (should not occur outside of testing)
            if(smallest == -1)
            {
                delete[] distance;
                EList[Index]->Activate(false);
                return false;
            }
            else
            {
                //Choose target with smallest distance
                if(smallest+1 < *Current)
                    for(int i = smallest+1;i<*Current;i++)
                        if(distance[i]!=-1 && distance[i] < distance[smallest])
                            smallest = i;
                delete[] distance;
                //Choose direction to go
                EList[smallest]->GetCoordinates(0,tempx,tempy);
                int dx = x-tempx, dy = y-tempy, c = 0;
                if(abs(dy) <= abs(dx))
                    if(dx < 0)
                        if(aggressive)
                            c = 1;
                        else
                            c = 0;
                    else
                        if(aggressive)
                            c = 0;
                        else
                            c = 1;
                else
                    if(dy < 0)
                        if(aggressive)
                            c = 3;
                        else
                            c = 2;
                    else
                        if(aggressive)
                            c = 2;
                        else
                            c = 3;
                int tries = 0;
                while (tries < 2)
                {
                    //Check if it's possible
                    switch(c)
                    {
                    case 0:
                        if(Check(EList,Index,Current,Field,x-1,y))
                        {
                            EList[Index]->Move(x-1,y);
                            return false;
                        }
                        else
                        {
                            tries++;
                            if(dy<0)
                                if(aggressive)
                                    c=3;
                                else
                                    c=2;
                            else
                                if(aggressive)
                                    c=2;
                                else
                                    c=3;
                        }
                        break;
                    case 1:
                        if(Check(EList,Index,Current,Field,x+1,y))
                        {
                            EList[Index]->Move(x+1,y);
                            return false;
                        }
                        else
                        {
                            tries++;
                            if(dy<0)
                                if(aggressive)
                                    c=2;
                                else
                                    c=3;
                            else
                                if(aggressive)
                                    c=3;
                                else
                                    c=2;
                        }
                        break;
                    case 2:
                        if(Check(EList,Index,Current,Field,x,y-1))
                        {
                            EList[Index]->Move(x,y-1);
                            return false;
                        }
                        else
                        {
                            tries++;
                            if(dx<0)
                                if(aggressive)
                                    c=1;
                                else
                                    c=0;
                            else
                                if(aggressive)
                                    c=1;
                                else
                                    c=0;
                        }
                        break;
                    case 3:
                        if(Check(EList,Index,Current,Field,x,y+1))
                        {
                            EList[Index]->Move(x,y+1);
                            return false;
                        }
                        else
                        {
                            tries++;
                            if(dx<0)
                                if(aggressive)
                                    c=0;
                                else
                                    c=1;
                            else
                                if(aggressive)
                                    c=0;
                                else
                                    c=1;
                        }
                        break;
                    }
                    //No direct path found
                    if(tries > 1)
                    {
                        printf("/panic/\n");
                        return Idiot(EList,Index,Current,Field);
                    }
                }
            }
        }
    }
    else
        return Attack(0,EList,Index,Current);
    return Idiot(EList,Index,Current,Field);
}*/


bool AI::new_Idiot(Entity::EHead** AE_List,Entity::EHead* Current, int** Field)
{
    //Can I move?
    if(Current->ReturnMoved() < Current->ReturnSpeed())
    {
        //Where can I move?
        bool moveable[5] = {false,false,false,false,false};
        int x,y;
        Current->GetCoordinates(0,x,y);

        moveable[1] = new_Check(AE_List,Current,Field,x-1,y);
        moveable[2] = new_Check(AE_List,Current,Field,x,y-1);
        moveable[3] = new_Check(AE_List,Current,Field,x+1,y);
        moveable[4] = new_Check(AE_List,Current,Field,x,y+1);
        for(int i=1;i<5;i++)
            if(moveable[i])
            {
                moveable[0] = true;
                break;
            }
        //No valid direction, do something else
        if(!moveable[0])
        {
            return new_Attack(0,AE_List,Current);
        }
        //Choose random VALID direction
        std::srand(SDL_GetTicks());
        int c = std::rand()%4;
        while(!moveable[c+1])
            c = std::rand()%4;
        //move..
        switch(c)
        {
        case 0:
            Current->Move(x-1,y);
            //printf("Moving %s(%i)\t %i,%i\t %i,%d\n",EList[Index]->ReturnName().c_str(),Index,x,y,Field[x-1][y],moveable[0]);
            break;
        case 1:
            Current->Move(x,y-1);
            //printf("Moving %s(%i)\t %i,%i\t %i,%d\n",EList[Index]->ReturnName().c_str(),Index,x,y,Field[x][y-1],moveable[0]);
            break;
        case 2:
            Current->Move(x+1,y);
            //printf("Moving %s(%i)\t %i,%i\t %i,%d\n",EList[Index]->ReturnName().c_str(),Index,x,y,Field[x+1][y],moveable[0]);
            break;
        case 3:
            Current->Move(x,y+1);
            //printf("Moving %s(%i)\t %i,%i\t %i,%d\n",EList[Index]->ReturnName().c_str(),Index,x,y,Field[x][y+1],moveable[0]);
            break;
        }
    }
    else
    {
        return new_Attack(0,AE_List,Current);
    }
    return false;
}


bool AI::new_Attack(int mode,Entity::EHead** AE_List, Entity::EHead* Current)
{
    if(Current->ActionCount() == 0)
    {
        Current->Activate(false);
        return false;
    }
    int last = 0;
    Entity::EHead* tempE = AE_List[0];
    while (tempE != NULL)
    {
        last++;
        tempE = tempE->ReturnNext();
    }
    Entity::EHead* List[last];
    last = 0;
    int range = Current->ReturnActionRange(0);
    int x,y;
    Current->GetCoordinates(0,x,y);
    for(int range_x = -range;range_x <= range;range_x++)
    for(int range_y = -range;range_y <= range;range_y++)
        if(abs(range_x)+abs(range_y) <= range)
        {
            tempE = AE_List[0];
            while(tempE != NULL)
            {
                if(tempE->Check(x+range_x,y+range_y))
                    List[last++] = tempE;
                tempE = tempE->ReturnNext();
            }
        }

    if(last!=0)
    {
        int c = 0;
        switch(mode)
        {
        case 0:
            c = std::rand()%last;
            break;
        case 1:
            c = std::rand()%last;
            /*
            if(last > 1)
                for(int i = 1;i<last;i++)
                    if(EList[i]->currentsize()<EList[c]->currentsize())
                        c = i;*/
            break;
        }
        printf("Attack!\n");
        Current->Activate(false);
        return !List[c]->Damage(Current->ReturnActionAmmount(0));
    }
    Current->Activate(false);
    return false;
}

bool AI::new_Check(Entity::EHead** EList,Entity::EHead* Current, int** Field,int x, int y)
{
    bool success = false;
    if(-1<x && x<14 && -1<y && y<11 && Field[x][y] != -1)
    {
        success = true;
        for(int i=0;i<3;i++)
        {
            Entity::EHead* tempE = EList[i];
            while(tempE != NULL)
            {
                if(tempE != Current && tempE->Check(x,y))
                    success = false;
                tempE = tempE->ReturnNext();
            }
        }
    }
    return success;
}
