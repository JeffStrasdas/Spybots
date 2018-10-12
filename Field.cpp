#include "Field.h"
#include "cmath"
#include <fstream>

void field::Area::Dump_Field(std::string filename)
{
    std::string opath = "data/stages/" + filename + ".stage";
    std::ofstream ofile (opath.c_str());
    if(ofile.is_open())
    {
        ofile << "GROUND\n";
        for(int y=0;y<FIELD_HEIGHT;++y)
        {
            for(int x=0;x<FIELD_WIDTH;++x)
            {
                if(Graphical[x][y] == -1)
                    ofile << ". ";
                else
                    ofile << Graphical[x][y] << " ";
            }
            ofile << "\n";
        }
        ofile << "\nOBJECTS\n";
        int x,y;
        Entity::EHead* tempE;
        for(int i=0;i<3;i++)
        {
            std::string tempS = i == 0 ? "Player:\n" : i == 1 ? "Enemy:\n" : "Other:\n";
            ofile << tempS;
            tempE = AE_List[i];
            while(tempE != NULL)
            {
                int k = tempE->currentsize()-1;
                tempE->GetCoordinates(k,x,y);
                ofile << tempE->ID << ": " << x << "," << y;
                while(--k>=0)
                {
                    tempE->GetCoordinates(k,x,y);
                    ofile << " " << x << "," << y;
                }
                ofile << "\n";
                tempE = tempE->ReturnNext();
            }
        }
        ofile << "\nOTHER\n";
        ofile << "GM: " << *GameMode;
        ofile.close();
    }
}

void field::Area::Apply()
{
    if(ActiveEntity != NULL)
        ActiveEntity->Apply();
}
void field::Area::Revert()
{
    ActiveAction = NULL;
    DetermineDistance();

    if(ActiveEntity != NULL)
    {
        ActiveEntity->Revert(Useable);
        int x,y;
        ActiveEntity->GetCoordinates(0,x,y);
        DetermineDistance(ActiveEntity,x,y);
    }
}

void field::Area::Generate(std::string filename)
{
    std::string line = "data/stages/" + filename + ".stage";
    std::ifstream ifile(line.c_str());

    if(ifile.is_open())
    {
        while(getline(ifile,line))
        {
            //filter empty lines and comments
            if(line.length() == 0 || line[0] == '#')
                continue;
            if(line == "GROUND")
            {
                printf("Setting up ground............");
                int y = -1;
                while(getline(ifile,line) && line != "OBJECTS" && y < FIELD_HEIGHT)
                {
                    //filter empty lines and comments
                    if(line.length() == 0 || line[0] == '#')
                        continue;
                    ++y;
                    for(int x = 0; x < FIELD_WIDTH; ++x)
                        if(line[2*x] == '.')
                        {
                            //interpret '.' as empty
                            Graphical[x][y] = -1;
                            Useable[x][y] = false;
                        }
                        else
                        {
                            Graphical[x][y] = (int)(line[2*x])-48;
                            Useable[x][y] = true;
                        }
                }
                printf("done!\n");
            }
            if(line == "OBJECTS")
            {
                printf("Setting up objects...........");
                bool set_anchor = false;
                int index = -1;
                unsigned int a,b;
                Entity::EHead* tempE = NULL;
                Entity::EHead* last = NULL;

                while(getline(ifile,line) && line != "OTHER")
                {
                    //Filter empty lines and comments
                    if(line.length() == 0 || line[0] == '#')
                        continue;
                    //Check for new anchor element
                    if(line == "Player:" || line == "Enemy:" || line == "Other:")
                    {
                        ++index;
                        set_anchor = true;
                        last  = NULL;
                        continue;
                    }
                    //Extract ID
                    a = 0;
                    b = line.find(':');
                    if(line.length() < 6 || b == -1)
                    {
                        printf("invalid formatting; skipping...");
                        continue;
                    }
                    int ID = atoi(line.substr(a,b).c_str());
                    //Set up objective
                    if(ID == -3)
                        HasObjective = true;
                    //Maybe trash

                    int x,y;
                    line = line.substr(b+2,line.length()-b-2).c_str();
                    //Extract x and y coordinates
                    b = 1;
                    while(b < line.length()-1)
                    {
                        while(line[b] != ',')
                            ++b;
                        x = atoi(line.substr(a,b-a).c_str());
                        a = b+1;
                        while(b < line.length() && line[b] != ' ')
                            ++b;
                        y = atoi(line.substr(a,b-a).c_str());
                        a = b+1;
                        if(tempE != NULL)
                            tempE->AddElement(x,y,Useable,true);
                        else
                            if(last == NULL)
                                tempE = new Entity::EHead(ID,x,y,index == 0 ? true : false,Useable);
                            else
                                tempE = new Entity::EHead(ID,x,y,index == 0 ? true : false,Useable,last);
                    }
                    tempE->Set_Sprites();
                    last  = tempE;
                    tempE = NULL;
                    if(set_anchor)
                    {
                        AE_List[index] = last;
                        set_anchor = false;
                    }
                }
                printf("done!\n");
            }
            if(line == "OTHER")
            {
                printf("Setting up other stuff.......");
                int a,b;
                while(getline(ifile,line))
                {
                    //Filter empty lines and comments
                    if(line.length() == 0 || line[0] == '#')
                        continue;
                    a = line.find(':');
                    b = line.length()-a-2;
                    if( a < 0 || b < 1)
                    {
                        printf("invalid formatting; skipping line...");
                        continue;
                    }
                    std::string FrontEnd = line.substr(0,a);
                    std::string BackEnd  = line.substr(a+2,b);
                    if(BackEnd != "none")
                    {
                        if(FrontEnd == "Background")
                        {
                            Background = BackEnd;
                        }
                        else
                        if(FrontEnd == "Story")
                        {
                            a = BackEnd.find(',');
                            b = BackEnd.length() - a - 1;
                            if(a < 0 || b < 1)
                                printf("Invalid formatting; skipping story");
                            else
                            {
                                Story = BackEnd.substr(0,a);
                                printf("load story '%s'...",Story.c_str());
                                printf("Dump story '%s'...",BackEnd.substr(a+1,b).c_str());

                            }
                            // *GameMode = 3;
                        }
                        else
                        if(FrontEnd == "Unlocks")
                        {
                            a = b = 0;
                            while(b<BackEnd.length())
                            {
                                while(a < BackEnd.length() && BackEnd[a] != ',')
                                    ++a;
                                printf("Unlocks '%s'...",BackEnd.substr(b,a-b).c_str());
                                b = ++a;
                            }
                            Unlocks = BackEnd;
                        }
                        else
                        if(FrontEnd == "Visible")
                        {
                            a = b = 0;
                            while(b<BackEnd.length())
                            {
                                while(a < BackEnd.length() && BackEnd[a] != ',')
                                    ++a;
                                printf("Reveals '%s'...",BackEnd.substr(b,a-b).c_str());
                                b = ++a;
                            }
                            Visible = BackEnd;
                        }
                        else
                        if(FrontEnd == "Rewards")
                        {
                            StageMoney = atoi(BackEnd.c_str());
                        }
                    }
                }
                printf("done!\n");
            }
        }
    }
    ifile.close();

}
void field::Area::Add(int x, int y)
{
    Graphical[x][y] = 0;
    Useable[x][y]   = true;
}
void field::Area::Remove(int x, int y)
{
    Graphical[x][y] = -1;
    Useable[x][y]   = false;
}

void field::Area::Draw()
{
    //Background
    for(int x=0;x<FIELD_WIDTH;x++)
        for(int y=0;y<FIELD_HEIGHT;y++)
        {
            SDL_Rect* tempRect = Choose_Rect(Graphical[x][y], 0);
            BackgroundTexture.render(X_OFFSET+32*x, Y_OFFSET+32*y, Renderer, tempRect);
            delete tempRect;
        }
    //Entities
    for(int i=0,x,y,z;i<3;i++)
    {
        Entity::EHead* tempE = AE_List[i];
        while(tempE != NULL)
        {
            tempE->GetCoordinates(0,x,y);
            z = tempE->ID;
            if(i<2)
            {
                for(int _i = 0, _x,_y,_z; _i<tempE->currentsize();_i++)
                {
                    tempE->GetCoordinates(_i,_x,_y);
                    _z = tempE->Get_Sprite(_i);
                    SDL_Rect* tempRect = Choose_Rect(_z,2);
                    EntityBodyTexture.render(X_OFFSET+32*_x, Y_OFFSET+32*_y, Renderer, tempRect);
                    delete tempRect;
                }
                SDL_Rect* tempRect = Choose_Rect(z,1);
                EntityHeadTexture.render(X_OFFSET+32*x, Y_OFFSET+32*y, Renderer, tempRect);
                delete tempRect;
            }
            else
            {
                tempE->GetCoordinates(0,x,y);
                switch(tempE->ID)
                {
                case -1:
                    {
                        //Upload zone
                        SDL_Rect* tempRect = Choose_Rect(20,0);
                        BackgroundTexture.render(X_OFFSET+32*x,Y_OFFSET+32*y,Renderer,tempRect);
                        delete tempRect;
                        break;
                    }
                case -2:
                    {
                        //Credits
                        SDL_Rect* tempRect = Choose_Rect(21,0);
                        BackgroundTexture.render(X_OFFSET+32*x,Y_OFFSET+32*y,Renderer,tempRect);
                        delete tempRect;
                        break;
                    }
                case -3:
                    {
                        //Goal
                        SDL_Rect* tempRect = Choose_Rect(22,0);
                        BackgroundTexture.render(X_OFFSET+32*x,Y_OFFSET+32*y,Renderer,tempRect);
                        delete tempRect;
                        break;
                    }
                default:
                    {
                        //entities to be spawned
                        for(int _i = 0, _x,_y,_z; _i<tempE->currentsize();_i++)
                        {
                            tempE->GetCoordinates(_i,_x,_y);
                            _z = tempE->Get_Sprite(_i);
                            SDL_Rect* tempRect = Choose_Rect(_z,2);
                            EntityBodyTexture.render(X_OFFSET+32*_x, Y_OFFSET+32*_y, Renderer, tempRect);
                            delete tempRect;
                        }
                        SDL_Rect* tempRect = Choose_Rect(z,1);
                        EntityHeadTexture.render(X_OFFSET+32*x, Y_OFFSET+32*y, Renderer, tempRect);
                        delete tempRect;
                    }
                }
            }
            tempE = tempE->ReturnNext();
        }
    }
    //Overlay
    if(ActiveEntity != NULL)
    {
        int SpriteID = ActiveEntity->ColorKey()*16+15;
        //Render background tile
        if(SpriteID>-1)
        {
            SDL_Rect* tempRect = Choose_Rect(SpriteID,2);
            EntityBodyTexture.render(13,182,Renderer,tempRect);
            delete tempRect;
            tempRect = Choose_Rect(ActiveEntity->ID,1);
            EntityHeadTexture.render(13,182,Renderer,tempRect);
        }
        else
        {
            switch(ActiveEntity->ID)
            {
            case -1:
                {
                    //Upload zone
                    SDL_Rect* tempRect = Choose_Rect(20,0);
                    BackgroundTexture.render(13,182,Renderer,tempRect);
                    delete tempRect;
                    break;
                }
            case -2:
                {
                    //Credits
                    SDL_Rect* tempRect = Choose_Rect(21,0);
                    BackgroundTexture.render(13,182,Renderer,tempRect);
                    delete tempRect;
                    break;
                }
            case -3:
                {
                    //Goal
                    SDL_Rect* tempRect = Choose_Rect(22,0);
                    BackgroundTexture.render(13,182,Renderer,tempRect);
                    delete tempRect;
                    break;
                }
            default:
                printf("Error while drawing special entity preview image\n");
            }
        }
        if(ActiveAction == NULL)
        {
            //Draw movement
            int x,y;
            ActiveEntity->GetCoordinates(0,x,y);
            int MovementRange = ActiveEntity->ReturnSpeed()-ActiveEntity->ReturnMoved();
            if(MovementRange != 0 && ActiveEntity->Activate())
            for(int x_range = -MovementRange;x_range <= MovementRange; x_range++)
            for(int y_range = -MovementRange;y_range <= MovementRange; y_range++)
            {
                switch(Distance[x+x_range][y+y_range])
                {
                case 1:
                    {
                        SDL_Rect* tempRect;
                        //Adjacent movement field
                        if(x_range == -1)
                        {
                            tempRect = Choose_Rect(10,0);
                            BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                        }
                        if(x_range ==  1)
                        {
                            tempRect = Choose_Rect(10,0);
                            //BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                            BackgroundTexture.renderx(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect,180);
                        }
                        if(y_range == -1)
                        {
                            tempRect = Choose_Rect(10,0);
                            //BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                            BackgroundTexture.renderx(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect,90);
                        }
                        if(y_range == 1)
                        {
                            tempRect = Choose_Rect(10,0);
                            //BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                            BackgroundTexture.renderx(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect,270);
                        }
                        delete tempRect;
                        break;
                    }
                case  0:
                case -1:
                    break;
                default:
                    {
                        SDL_Rect* tempRect;
                        tempRect = Choose_Rect(11,0);
                        BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                        delete tempRect;
                    }
                }
                //Check for out-of bounds, check entities except for itself and items
                /*if(x+x_range>=0 && x+x_range <FIELD_WIDTH && y+y_range >=0 && y+y_range < FIELD_HEIGHT && (Useable[x+x_range][y+y_range] || ActiveEntity->Check(x+x_range,y+y_range) || (AE_List[2] != NULL ? AE_List[2]->CheckList(x+x_range,y+y_range) : false) ))
                {
                    //Cut to form
                    if(!(x_range==0 && y_range==0) && abs(x_range)+abs(y_range) <= MovementRange)
                    {
                        SDL_Rect* tempRect;
                        switch(abs(x_range)+abs(y_range))
                        {
                        case 1:
                            //Adjacent movement field
                            if(x_range == -1)
                            {
                                tempRect = Choose_Rect(10,0);
                                BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                            }
                            if(x_range ==  1)
                            {
                                tempRect = Choose_Rect(10,0);
                                //BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                                BackgroundTexture.renderx(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect,180);
                            }
                            if(y_range == -1)
                            {
                                tempRect = Choose_Rect(10,0);
                                //BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                                BackgroundTexture.renderx(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect,90);
                            }
                            if(y_range == 1)
                            {
                                tempRect = Choose_Rect(10,0);
                                //BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                                BackgroundTexture.renderx(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect,270);
                            }
                            break;
                        default:
                            //normal movement field
                            tempRect = Choose_Rect(11,0);
                            BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                            break;
                        }
                        delete tempRect;
                    }
                }*/
            }
        }
        else
        {
            //Draw action
            int x,y;
            ActiveEntity->GetCoordinates(0,x,y);
            int ActionRange = ActiveAction->Range;
            for(int x_range = -ActionRange; x_range<=ActionRange;x_range++)
            for(int y_range = -ActionRange; y_range<=ActionRange;y_range++)
            {
                //Check for out-of bounds
                if(x+x_range>=0 && x+x_range <FIELD_WIDTH && y+y_range >=0 && y+y_range < FIELD_HEIGHT)
                {
                    //Check whether field exists
                    if(ActiveAction->Type == 3 || Graphical[x+x_range][y+y_range] != -1)
                    {
                         //Cut to form
                         if(!(x_range==0 && y_range==0) && abs(x_range)+abs(y_range) <= ActionRange && !ActiveEntity->Check(x+x_range,y+y_range))
                         {
                            SDL_Rect* tempRect;
                            switch(ActiveAction->Type)
                            {
                            default:
                            case 0:
                                //Damage
                                tempRect = Choose_Rect(13,0);
                                BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                                break;
                            case 1:
                            case 2:
                                //Support
                                tempRect = Choose_Rect(14,0);
                                BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                                break;
                            case 3:
                                //change field
                                tempRect = Choose_Rect(15,0);
                                BackgroundTexture.render(X_OFFSET+32*(x+x_range), Y_OFFSET+32*(y+y_range), Renderer, tempRect);
                                break;
                            }
                            delete tempRect;
                         }
                    }
                }
            }
        }
    }
    //Render check mark for non-active Units that are currently playing
    if(*GameMode == 1 || *GameMode == 2)
    {
        Entity::EHead* tempE = AE_List[*GameMode-1];
        while(tempE != NULL)
        {
            if(!tempE->Activate())
            {
                int _x,_y;
                tempE->GetCoordinates(0,_x,_y);
                SDL_Rect* tempRect = Choose_Rect(30,0);
                BackgroundTexture.render(X_OFFSET+32*_x+16, Y_OFFSET+32*_y-16, Renderer, tempRect);
                delete tempRect;
            }
            tempE = tempE->ReturnNext();
        }
    }
}

SDL_Rect* field::Area::Choose_Rect(int ID,int Mode)
{
    SDL_Rect* ReturnRect = new SDL_Rect;
    ReturnRect->h = 32;
    ReturnRect->w = 32;
    switch(Mode)
    {
    case 0: // background
        ReturnRect->x = 36*(ID%10);
        ReturnRect->y = 36*(floor((double)(ID)/10.0));
        break;
    case 1: // sprite body
        ReturnRect->x = 9 + 37*(ID%15);
        ReturnRect->y = 9 + 37*(floor((double)(ID)/15.0));
        break;
    case 2: // sprite head
        ReturnRect->x = 2 + 34*(ID%16);
        ReturnRect->y = 2 + 34*(floor((double)(ID)/16.0));
        break;
    }

    return ReturnRect;
}
void field::Area::SwapGameMode()
{
    ActiveEntity = NULL;
    ActiveAction = NULL;
    DetermineDistance();
    //Activate inactive entities
    Entity::EHead* tempE = AE_List[*GameMode%2];
    while(tempE != NULL)
    {
        tempE->Activate(true);
        tempE = tempE->ReturnNext();
    }

}

bool field::Area::Begin()
{
    bool success = false;
    Entity::EHead* tempE = AE_List[2];
    while(tempE != NULL)
    {
        if(tempE->ID > -1)
        {
            success = true;
            break;
        }
        tempE = tempE->ReturnNext();
    }
    //No playable entities found
    if(!success && AE_List[0] == NULL)
        return false;

    tempE = AE_List[2];
    while(tempE != NULL)
    {
        if(tempE->ID > -1)
        {
            //Remove from object list
            if(AE_List[2] == tempE)
                AE_List[2] = tempE->ReturnNext();
            tempE->Disconnect();
            //Add to playable entity list
            if(AE_List[0] != NULL)
            {
                tempE->SetPrev(AE_List[0]->ReturnLast());
                AE_List[0]->ReturnLast()->SetNext(tempE);
            }
            else
            {
                AE_List[0] = tempE;
            }
            //revert to first position
            tempE = AE_List[2];
        }
        else
        {
            if(tempE->ID == -1)
            {
                int _x,_y;
                tempE->GetCoordinates(0,_x,_y);
                Useable[_x][_y] = true;
                if(tempE == AE_List[2])
                    AE_List[2] = AE_List[2]->ReturnNext();
                delete tempE;
            }
            tempE = tempE->ReturnNext();
        }
    }
    //Set up
    //*GameMode = 2;
    ActiveEntity = NULL;
    ActiveAction = NULL;
    return true;
}
void field::Area::DetermineDistance()
{
    for(int x = 0,y; x < FIELD_WIDTH; ++x)
        for(y = 0; y < FIELD_HEIGHT;++y)
            Distance[x][y] = -1;
}
void field::Area::AI_DetermineDistance(Entity::EHead* Object, Entity::EHead* Target, int x,int y, int Offset)
{
    if(x<FIELD_WIDTH && x>=0 && y >=0 && y<FIELD_HEIGHT)
    if((Distance[x][y] == -1 || Distance[x][y] > Offset) && (Useable[x][y] || Target->Check(x,y) || Object->Check(x,y)))
    {
        Distance[x][y] = Offset;
        //Recursive in other directions
        AI_DetermineDistance(Object,Target,x-1,y,++Offset);
        AI_DetermineDistance(Object,Target,x+1,y,Offset);
        AI_DetermineDistance(Object,Target,x,y-1,Offset);
        AI_DetermineDistance(Object,Target,x,y+1,Offset);
    }
}
void field::Area::DetermineDistance(Entity::EHead* Object,int x,int y,int Offset)
{
    if(Offset <= Object->ReturnSpeed() - Object->ReturnMoved())
    if(x<FIELD_WIDTH && x>=0 && y >=0 && y<FIELD_HEIGHT)
    if((Distance[x][y] == -1 || Distance[x][y] > Offset) && (Useable[x][y] || Object->Check(x,y) || (AE_List[2] != NULL && AE_List[2]->CheckList(x,y) != NULL)))
    {
        Distance[x][y] = Offset;
        //Recursive in other directions
        DetermineDistance(Object,x-1,y,++Offset);
        DetermineDistance(Object,x+1,y,Offset);
        DetermineDistance(Object,x,y-1,Offset);
        DetermineDistance(Object,x,y+1,Offset);
    }
}

void field::Area::SetActive(void* setTo, bool isAction)
{
    if(isAction)
    {
        Entity::Action* tempA = (Entity::Action*)(setTo);
        if(tempA != NULL)
            switch(tempA->Type)
            {
            case -2:
                Revert();
                delete tempA;
                break;
            case -1:
                Apply();
                delete tempA;
                break;
            default:
                ActiveAction = tempA;
                break;
            }
        else
            ActiveAction = NULL;
    }
    else
    {
        Entity::ListElement* tempL = (Entity::ListElement*)(setTo);
        //Check whether an entity should be spawned
        if(ActiveEntity != NULL && (ActiveEntity->ID == -1 || (ActiveEntity->ID > -1 && ActiveEntity->ReturnFirst() == AE_List[2])) && tempL != NULL && tempL->amount > 0)
        {
            int _x,_y;
            ActiveEntity->GetCoordinates(0,_x,_y);
            tempL->amount--;
            Entity::EHead* tempE = new Entity::EHead(tempL->element->ID,_x,_y,true,Useable,AE_List[2]->ReturnLast());
            //"Return" already spawned entity to the list
            if(ActiveEntity->ID != -1)
            {
                Entity::ListElement* _L = tempL;
                bool done = false;
                while(tempL != NULL && !done)
                    if(tempL->element->ID == ActiveEntity->ID)
                    {
                        tempL->amount++;
                        done = true;
                    }
                    else
                        tempL = tempL->prev;
                tempL = _L;
                while(tempL != NULL && !done)
                    if(tempL->element->ID == ActiveEntity->ID)
                    {
                        tempL->amount++;
                        done = true;
                    }
                    else
                        tempL = tempL->next;

            }
            if(ActiveEntity == AE_List[2])
                AE_List[2] = AE_List[2]->ReturnNext();
            delete ActiveEntity;
            ActiveEntity = tempE;
        }
        else
        {
            if(tempL != NULL)
                ActiveEntity = tempL->element;
            else
                ActiveEntity = NULL;
        }
    }

}

//returns true if enemies are all done
bool field::Area::AI_Action()
{
    Entity::EHead* tempE = AE_List[1];
    while(tempE != NULL)
    {
        if(tempE->Activate())
        {
            DetermineDistance();
            Entity::EHead* T = AE_List[0];
            while(T != NULL)
            {
                int x,y,i = -1;
                while(++i < T->currentsize())
                {
                    T->GetCoordinates(i,x,y);
                    AI_DetermineDistance(tempE,T,x,y);
                }
                T = T->ReturnNext();
            }

            T = tempE->AI_Linear(AE_List[0],Distance,Useable);
            if(T != NULL)
            {
                if(AE_List[0] == T)
                    AE_List[0] = T->ReturnNext();
                delete T;

                if(AE_List[0] == NULL)
                    *GameMode = 3;
            }
            return false;
        }
        tempE = tempE->ReturnNext();
    }
    return true;
}
Entity::EHead* field::Area::Move(int x,int y)
{
    if(x < 0 || x >= FIELD_WIDTH || y < 0 || y >= FIELD_HEIGHT)
        return NULL;
    Entity::EHead* tempE;
    //check if entity can move
    if(AE_List[2] != NULL)
        tempE = AE_List[2]->CheckList(x,y);
    else
        tempE = NULL;
    if(Distance[x][y] == 1 && (Useable[x][y] || ActiveEntity->Check(x,y) || tempE != NULL) && ActiveEntity->CanMove())
    {
        ActiveEntity->Move(x,y,Useable);
        if(tempE != NULL)
        {
            if(AE_List[2] == tempE)
                AE_List[2] = AE_List[2]->ReturnNext();
            if(tempE->ID == -3)
                *GameMode = 3; //End game
            else
            {
                //Brouzoufs
                PlayerMoney += (StageMoney/2);
                printf("collect brousoufs; Player holds %i in total\n",PlayerMoney);
            }
            delete tempE;
        }
        //Update Movement field
        DetermineDistance();
        DetermineDistance(ActiveEntity,x,y);
        return ActiveEntity;
    }
    //Out of bounds
    return NULL;
}
Entity::EHead* field::Area::newClick(int mx, int my)
{
    //Determine on which field the mouse clicks
    int x,y;
    for(x=0;x<FIELD_WIDTH;x++)
            if(mx-X_OFFSET < x*32)
                break;
    x--;
    for(y=0;y<FIELD_WIDTH;y++)
        if(my-Y_OFFSET < y*32)
            break;
    y--;
    printf("On Field %i,%i.\n",x,y);
    if(ActiveEntity == NULL)
    {
        //Check for new active entity
        if(!Useable[x][y])
        {
            Entity::EHead* tempE;
            int _x,_y;
            for(int i=0;i<3;i++)
            {
                tempE = AE_List[i];
                while(tempE != NULL)
                {
                    tempE->GetCoordinates(0,_x,_y);
                    if(x == _x && y == _y)
                    {
                        ActiveEntity = tempE;
                        ActiveAction = NULL;
                        printf("%s\n",ActiveEntity->ReturnName().c_str());
                        DetermineDistance();
                        DetermineDistance(ActiveEntity,x,y);
                        return ActiveEntity;
                    }
                    tempE = tempE->ReturnNext();
                }
            }
        }
        ActiveEntity = NULL;
        ActiveAction = NULL;
        DetermineDistance();
        return NULL;
    }
    else
    {

        Entity::EHead* tempE = ActiveEntity->ReturnFirst();
        if(tempE == AE_List[0] && *GameMode == 1)
        {
            if(ActiveAction == NULL)
            {
                tempE = Move(x,y);
                if(tempE != NULL)
                    return tempE;
            }
            else
            {
                //Check if action can be used & hits a valid field
                int _x,_y;
                ActiveEntity->GetCoordinates(0,_x,_y);
                if(ActiveAction->Requirement <= ActiveEntity->currentsize() && abs(x-_x)+abs(y-_y) <= ActiveAction->Range  && !ActiveEntity->Check(x,y))
                {
                    //filter action type
                    switch(ActiveAction->Type)
                    {
                        // damage
                    case 0:
                        {
                            if(AE_List[1] == NULL)
                                break;
                            tempE = AE_List[1]->CheckList(x,y);
                            if(tempE == NULL)
                            {
                                printf("miss!\n");
                            }
                            else
                            {
                                //Damage
                                printf("hit %s!\n",tempE->ReturnName().c_str());
                                //enemy survives
                                if(tempE->Damage(ActiveAction->Ammount,Useable))
                                        tempE->Set_Sprites();
                                else
                                {
                                    //Entity dies
                                    if(AE_List[1] == tempE)
                                        AE_List[1] = AE_List[1]->ReturnNext();
                                    delete tempE;
                                    if(!HasObjective && AE_List[1] == NULL)
                                        *GameMode = 3;
                                }
                            }
                        }
                        break;
                        //Change speed
                    case 1:
                        {
                            tempE = AE_List[0]->CheckList(x,y);
                            if(tempE == NULL)
                                tempE = AE_List[1]->CheckList(x,y);
                            if(tempE == NULL)
                            {
                                printf("miss!\n");
                            }
                            else
                            {
                                printf("hit %s!\n",tempE->ReturnName().c_str());
                                tempE->ChangeSpeed(ActiveAction->Ammount);
                            }
                        }
                        break;
                        //Heal
                    case 2:
                        {
                            tempE = AE_List[0]->CheckList(x,y);
                            if(tempE == NULL)
                                tempE = AE_List[1]->CheckList(x,y);
                            if(tempE == NULL)
                            {
                                printf("miss!\n");
                            }
                            else
                            {
                                printf("hit %s!\n",tempE->ReturnName().c_str());
                                tempE->Heal(ActiveAction->Ammount,Useable);
                            }
                        }
                        break;
                        //Add/Remove field
                    case 3:
                        {
                            if(Graphical[x][y] == -1)
                                Add(x,y);
                            else
                                if(Useable[x][y])
                                    Remove(x,y);
                        }
                        break;
                        //Unknown action
                    default:
                        {
                        }
                        break;
                    }


                    Apply();

                    //Reset Distance Field
                    DetermineDistance();

                    ActiveEntity->Activate(false);
                    ActiveEntity = NULL;
                    ActiveAction = NULL;
                    return NULL;
                }
                else
                {
                    //Clicked somewhere outside
                    //do nothing
                    return ActiveEntity;
                }
            }
        }
        else
        {
            //Clicked non-playable or not during gamemode 1
            ActiveEntity = NULL;
            ActiveAction = NULL;
            DetermineDistance();
        }
        //Check for any new selection
        tempE = NULL;
        if(AE_List[0] != NULL)
            tempE = AE_List[0]->CheckList(0,x,y);
        if(tempE == NULL)
        {
            if(AE_List[1] != NULL)
                tempE = AE_List[1]->CheckList(0,x,y);
            if(tempE == NULL)
                if(AE_List[2] != NULL)
                    tempE = AE_List[2]->CheckList(0,x,y);
        }
        ActiveAction = NULL;
        if(tempE != NULL)
        {
            if(ActiveEntity != NULL && ActiveEntity->CanControl() && ActiveEntity->ReturnMoved() > 0)
            {
                ActiveEntity->Activate(false);
                Apply();
            }

            ActiveEntity = tempE;
            DetermineDistance();
            if(ActiveEntity != NULL)
            {
                ActiveEntity->GetCoordinates(0,x,y);
                DetermineDistance(ActiveEntity,x,y);
            }
        }
        return ActiveEntity;
    }
    //Should never be reached
    printf("woowee some not handled territory\n");
    DetermineDistance();
    return NULL;
}

Entity::EHead* field::Area::Click(int mx,int my)
{
    //Determine on which field the mouse clicks
    int x,y;
    for(x=0;x<FIELD_WIDTH;x++)
            if(mx-X_OFFSET < x*32)
                break;
    x--;
    for(y=0;y<FIELD_WIDTH;y++)
        if(my-Y_OFFSET < y*32)
            break;
    y--;
    printf("On Field %i,%i.  %i\n",x,y,Distance[x][y]);
    if(ActiveEntity != NULL)
    {
        Entity::EHead* tempE = ActiveEntity->ReturnFirst();
        if(tempE == AE_List[0] && *GameMode == 1)
        {

            //action
            int _x,_y;
            ActiveEntity->GetCoordinates(0,_x,_y);
            if(ActiveAction != NULL && ActiveEntity->currentsize() > ActiveAction->Requirement)
            {   //valid field
                if(abs(x - _x)+abs(y - _y) <= ActiveAction->Range && (x != _x || y != _y) && !ActiveEntity->Check(x,y))
                {
                    switch(ActiveAction->Type)
                    {
                    case 0:
                        {
                            //Damage
                            //Check enemies
                            tempE = AE_List[1];
                            while(tempE != NULL)
                            {
                                if(tempE->Check(x,y))
                                    break;
                                tempE = tempE->ReturnNext();
                            }
                            if(tempE != NULL)
                            {
                                printf("hit %s\n",tempE->ReturnName().c_str());

                                //Entity survives
                                if(tempE->Damage(ActiveAction->Ammount,Useable))
                                    tempE->Set_Sprites();
                                else
                                {
                                    //Entity dies
                                    int i = 0;
                                    while(i<2 && AE_List[i] != tempE)
                                        i++;
                                    if(i < 2)
                                        AE_List[i] = tempE->ReturnNext();
                                    delete tempE;
                                    if(!HasObjective && AE_List[1] == NULL)
                                        *GameMode = 3;
                                }

                            }
                            else
                            {
                                printf("miss\n");
                            }
                            break;
                        }
                    case 1:
                        {
                            //Change Speed
                            //Check player
                            tempE = AE_List[0];
                            while(tempE != NULL)
                            {
                                if(tempE->Check(x,y))
                                    break;
                                tempE = tempE->ReturnNext();
                            }
                            if(tempE == NULL)
                            {
                                //Check enemy
                                tempE = AE_List[1];
                                while(tempE != NULL)
                                {
                                    if(tempE->Check(x,y))
                                        break;
                                    tempE = tempE->ReturnNext();
                                }
                            }
                            if(tempE != NULL)
                            {
                                printf("hit %s\n",tempE->ReturnName().c_str());

                                //Change Speed
                                tempE->ChangeSpeed(ActiveAction->Ammount);
                            }
                            else
                            {
                                printf("miss\n");
                            }
                            break;
                        }
                        break;
                    case 2:
                        printf("heal entity\n");
                        break;
                    case 3:
                        printf("add/remove field");
                        if(Graphical[x][y] == -1)
                            Add(x,y);
                        else
                            if(Useable[x][y])
                                Remove(x,y);
                        break;
                    default:
                        printf("Unknown action\n");
                        break;
                    }

                    Apply();

                    //Reset Distance Field
                    DetermineDistance();

                    ActiveEntity->Activate(false);
                    ActiveEntity = NULL;
                    ActiveAction = NULL;
                    return NULL;
                }
                else
                {//Clicked outside of action fields
                    //Do nothing
                    return ActiveEntity;
                }
            }
            else
            {   //Move
                //Is field adjacent to active entity?
                if((x == _x && (y == _y+1 || y == _y-1)) || ((y == _y && (x == _x+1 || x == _x-1))))
                {
                    if(Useable[x][y])
                    {
                        ActiveEntity->Move(x,y,Useable);
                        //Update Movement field
                        DetermineDistance();
                        DetermineDistance(ActiveEntity,x,y);
                        return ActiveEntity;
                    }
                    else
                    {
                        //Can entity still move
                        if(Graphical[x][y] != -1)
                        {
                            if(ActiveEntity->Check(x,y))
                            {
                                ActiveEntity->Move(x,y,Useable);
                                //Update Movement field
                                DetermineDistance();
                                DetermineDistance(ActiveEntity,x,y);
                                return ActiveEntity;
                            }
                            tempE = AE_List[2];
                            while(tempE != NULL)
                            {
                                if(tempE->Check(x,y))
                                    break;
                                tempE = tempE->ReturnNext();
                            }
                            if(tempE != NULL)
                            {
                                if(tempE->ID == -3)
                                    *GameMode = 3;
                                if(AE_List[2] == tempE)
                                    AE_List[2] = tempE->ReturnNext();
                                delete tempE;

                                printf("Collect brousouf!\n");
                                ActiveEntity->Move(x,y,Useable);
                                //Update Movement field
                                DetermineDistance();
                                DetermineDistance(ActiveEntity,x,y);
                                return ActiveEntity;
                            }
                        }

                    }
                }
                //Change focus
                ActiveEntity = NULL;
                ActiveAction = NULL;
                //Reset Movement field
                DetermineDistance();
            }
        }
        else
        {
            ActiveEntity = NULL;
            ActiveAction = NULL;
            DetermineDistance();
        }
    }
    //Look for new active Entity
    if(!Useable[x][y])
    {
        for(int i=0;i<3;i++)
        {
            Entity::EHead* tempE = AE_List[i];
            while(tempE != NULL)
            {
                int _x,_y;
                tempE->GetCoordinates(0,_x,_y);
                if(x == _x && y == _y)
                {
                    ActiveEntity = tempE;
                    ActiveAction = NULL;
                    printf("%s\n",ActiveEntity->ReturnName().c_str());
                    DetermineDistance();
                    DetermineDistance(ActiveEntity,x,y);
                    return ActiveEntity;
                }
                tempE = tempE->ReturnNext();
            }
        }
    }
    DetermineDistance();
    return NULL;
}

Entity::EHead* field::Area::Key(SDL_Keycode code)
{
    if(ActiveEntity != NULL && ActiveAction == NULL && *GameMode == 1 && ActiveEntity->ReturnFirst() == AE_List[0])
    {
        int x,y;
        ActiveEntity->GetCoordinates(0,x,y);
        switch(code)
        {
            case SDLK_w:
            case SDLK_UP:
                Move(x,y-1);
                break;
            case SDLK_a:
            case SDLK_LEFT:
                Move(x-1,y);
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                Move(x+1,y);
                break;
            case SDLK_s:
            case SDLK_DOWN:
                Move(x,y+1);
                break;
        }
    }
}

void field::Area::Set(int x,int y, bool free)
{
    Useable[x][y] = free;
}

bool field::Area::LoadMedia()
{
    if( !BackgroundTexture.loadFromFile("images/sprites_4.png",Renderer) )
	{
		printf( "Failed to load background sprite texture!\n" );
		return false;
	}
    if( !EntityHeadTexture.loadFromFile("images/overlay_2.png",Renderer) )
	{
		printf( "Failed to load entity head texture!\n" );
		return false;
	}
    if( !EntityBodyTexture.loadFromFile("images/sprites_full_2.png",Renderer) )
	{
		printf( "Failed to load entity body texture!\n" );
		return false;
	}

	return true;
}

bool field::Area::NoActive()
{
    Entity::EHead* tempE = AE_List[*GameMode-1];
    while(tempE != NULL)
    {
        if(tempE->Activate())
            return false;
        tempE = tempE->ReturnNext();
    }
    return true;
}


Entity::Action* field::Area::ReturnAction()
{
    return ActiveAction;
}

std::string field::Area::BackgroundPath()
{
    return Background;
}

void field::Area::MatchResults(int& Player, int& Stage)
{
    if(AE_List[0] == NULL)
    {
        Player = Stage = -1;
    }
    else
    {
        Player = PlayerMoney;
        Stage = StageMoney;
    }
}

bool field::Area::ReturnUseable(int x,int y)
{
    return Useable[x][y];
}
void field::Area::close()
{
    //Free textures
    EntityHeadTexture.free();
    EntityBodyTexture.free();
    BackgroundTexture.free();
    //Free Entity list
    for(int i=0;i<3;i++)
    {
        Entity::EHead* tempE = AE_List[i];
        Entity::EHead* lastE = AE_List[i];
        while(tempE != NULL)
        {
            tempE = lastE->ReturnNext();
            delete lastE;
            lastE = tempE;
        }
        //delete AE_List[i];
    }
    delete[] AE_List;
    //Free field
    for(int x=0;x<FIELD_WIDTH;x++)
    {
        delete[] Graphical[x];
        delete[] Useable[x];
        delete[] Distance[x];
    }
    //Set pointers to NULL
    ActiveEntity = NULL;
    ActiveAction = NULL;
    Renderer     = NULL;
}
