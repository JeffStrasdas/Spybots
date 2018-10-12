#include <fstream>

#include "GUI.h"

void GUI::side_screen::Begin()
{
    //Set up
    //*GameMode = 2;
    ActiveAction = NULL;
    ActiveEntity = NULL;
    //undo shifting
    while(AvailablePrograms->prev != NULL)
        AvailablePrograms = AvailablePrograms->prev;
    /*
    //delete now useless list
    Entity::ListElement* tempL = AvailablePrograms;
    while(tempL != NULL)
    {
        tempL = tempL->next;
        delete AvailablePrograms;
        AvailablePrograms = tempL;
    }
    delete AvailablePrograms;
    */
    AvailablePrograms = NULL;

}

void* GUI::side_screen::newClick(int mx,int my,bool Action)
{
    //check program.list
    if(*GameMode == 0)
    {
        if(!Action)
        {
            int i = 0;
            //Check if player shifts selection
            if(mx > 118 && mx < 131)
            {
                if(my > 110 && my < 147)
                {
                    //down
                    Entity::ListElement* tempL = AvailablePrograms;
                    while(++i < 8 && tempL != NULL)
                    {
                        tempL = tempL->next;
                    }
                    if(i == 8 && tempL != NULL)
                    {
                        AvailablePrograms = AvailablePrograms->next;
                    }
                }
                else
                if(my > 40 && my < 77)
                {
                    //up
                    if(AvailablePrograms->prev != NULL)
                        AvailablePrograms = AvailablePrograms->prev;
                }
                ActiveEntity = NULL;
                return NULL;
            }
            else
            {
                //Check if player selects an entity
                i = 0;
                for(i=0;i<7;i++)
                {
                    if( my >= 50 + 12*i && my < 62+12*i)
                    {
                        printf("selected %i. element\n",i);
                        break;
                    }
                }
                if(i < 7)
                {
                    int k = 0;
                    Entity::ListElement* tempL = AvailablePrograms;
                    while(k++ < i && tempL != NULL)
                    {
                        tempL = tempL->next;
                    }
                    if(tempL != NULL)
                    {
                        ActiveEntity = tempL->element;
                        return tempL;
                    }
                    else
                        ActiveEntity = NULL;
                    return NULL;
                }
                ActiveEntity = NULL;
                return NULL;
            }
        }
        else
            return Click(mx,my);
    }
    //check other buttons
    return Click(mx,my);
}

Entity::Action* GUI::side_screen::Click(int mx,int my)
{
    if(ActiveEntity != NULL)
    {
         //Check new action
        ActiveAction = NULL;
        for(int i=ActiveEntity->ActionCount();i>=0;i--)
        {
            if(mx > ACTIONBOX_X && mx < ACTIONBOX_X+120 && my > ACTIONBOX_Y+20*i && my < ACTIONBOX_Y+20*(i+1))
            {
                if(i==ActiveEntity->ActionCount())
                {
                    printf("No action!\n");
                    if(*GameMode == 1)
                    {
                        //Apply
                        Entity::Action* tempA = new Entity::Action;
                        tempA->Type = -1;
                        ActiveEntity->Activate(false);
                        return tempA;
                    }
                    return NULL;
                }
                else
                {
                    ActiveAction = ActiveEntity->ReturnAction(i);
                    printf("%s!\n",ActiveAction->Name.c_str());
                    return ActiveAction;
                }
            }
        }
        //Check UNDO
        if(mx > UNDO_X && mx < UNDO_X+120 && my > UNDO_Y && my < UNDO_Y+20 && *GameMode == 1)
        {
            printf("UNDO!\n");
            //Revert
            ActiveAction = NULL;
            Entity::Action* tempA = new Entity::Action;
            tempA->Type = -2;
            return tempA;
        }
    }
    return NULL;
}

void GUI::side_screen::Motion(int mx,int my)
{
    if(ActiveEntity != NULL)
    {
        //mark UNDO
        if(mx > UNDO_X && mx < UNDO_X+120 && my > UNDO_Y && my < UNDO_Y+20)
        {
            hovering[3] = true;
        }
        else
            hovering[3] = false;
        //Mark action
        for(int i=ActiveEntity->ActionCount();i>=0;i--)
            {
                if(mx > ACTIONBOX_X && mx < ACTIONBOX_X+120 && my > ACTIONBOX_Y+20*i && my < ACTIONBOX_Y+20*(i+1))
                {
                    if(i==ActiveEntity->ActionCount())
                    {
                        hovering[2] = true;
                    }
                    else
                    {
                        hovering[i] = true;
                        break;
                    }
                }
                else
                {
                    if(i==ActiveEntity->ActionCount())
                    {
                        hovering[2] = false;
                    }
                    else
                    {
                        hovering[i] = false;
                    }
                }
            }
    }
}

void GUI::side_screen::SetActive(Entity::EHead* entity)
{
    ActiveEntity = entity;
    ActiveAction = NULL;
}
void GUI::side_screen::SetActive(Entity::Action* action)
{
    ActiveAction = action;
}

void GUI::side_screen::SwapGameMode()
{
    //GameMode == 1 ? GameMode = 2 : GameMode = 1;
    ActiveAction = NULL;
    ActiveEntity = NULL;
}

void GUI::side_screen::Draw()
{
    //program.info
    SDL_Rect* tempRect = Choose_Rect(1);
    UITexture.render(8,157, Renderer, tempRect);
    delete tempRect;

    //other buttons

    //Text
    if(ActiveEntity != NULL)
    {
        if(ActiveEntity->ID > -1)
        {
            TextRenderer->Write(15,235,"COMMANDS",false);
            //TextRenderer->Draw(15,235,"COMMANDS",false,{155,155,155});
            //Text::Draw(Renderer,15,235,"COMMANDS",FontBold,{155,155,155});
            if(ActiveEntity->CanControl())
            {
                for(int i=0;i<ActiveEntity->ActionCount();i++)
                {
                    tempRect = Choose_Rect(3,hovering[i]);
                    UITexture.render(8,249+20*i,Renderer,tempRect);
                    delete tempRect;
                }
                //No action
                tempRect = Choose_Rect(5,hovering[2]);
                UITexture.render(8,249+20*ActiveEntity->ActionCount(),Renderer,tempRect);
                delete tempRect;
            }
            else
            {
                for(int i=0;i<ActiveEntity->ActionCount();i++)
                {
                    tempRect = Choose_Rect(6);
                    UITexture.render(8,249+20*i,Renderer,tempRect);
                    delete tempRect;
                }
            }
        }
        ActiveEntity->DrawText(ActiveAction, TextRenderer);
        //ActiveEntity->DrawText(ActiveAction,Renderer,FontSmall,FontBold);
    }

    if(*GameMode == 0)
    {
        //program.list
        tempRect = Choose_Rect(0);
        UITexture.render(8, 21, Renderer, tempRect);
        delete tempRect;
        //inner workings of program.list
        int i = -1;
        Entity::ListElement* tempL = AvailablePrograms;
        //arrows
        if(AvailablePrograms->prev != NULL)
        {
            //Up arrow
            tempRect = Choose_Rect(8);
            UITexture.renderx(110,54,Renderer,tempRect,270);
            delete tempRect;
        }
        while(++i < 7 && tempL != NULL)
        {
            tempL = tempL->next;
        }
        if(i == 7 && tempL != NULL)
        {
            //Down arrow
            tempRect = Choose_Rect(8);
            UITexture.renderx(110,126,Renderer,tempRect,90);
            delete tempRect;
        }

        i = 0;
        tempL = AvailablePrograms;
        //select item
        char tempT[3];
        while(tempL != NULL && i<7)
        {
            TextRenderer->Write(15,50+12*i,tempL->element->ReturnName(),true);
            //TextRenderer->Draw(15,50+12*i,tempL->element->ReturnName().c_str());
            //Text::Draw(Renderer,15,50+12*i,tempL->element->ReturnName().c_str(),FontSmall);
            itoa(tempL->amount,tempT,10);
            TextRenderer->Write(112,50+12*i,tempT,true);
            //TextRenderer->Draw(112,50+12*i,tempT);
            //Text::Draw(Renderer,115,50+12*i,tempT,FontSmall);
            ++i;
            tempL = tempL->next;
        }

        //Begin databattle
        tempRect = Choose_Rect(7);
        UITexture.render(150,386, Renderer, tempRect);
        delete tempRect;
    }
    else
    {
        //Undo
        tempRect = Choose_Rect(2,hovering[3]);
        UITexture.render(8,386, Renderer, tempRect);
        delete tempRect;
    }
}

bool GUI::side_screen::LoadMedia(std::string player_name)
{
	if( !UITexture.loadFromFile("images/UI.png",Renderer) )
	{
		printf( "Failed to load UI texture!\n" );
		return false;
	}
    if(AvailablePrograms == NULL)
    {
        //Read available IDs and amount
        std::string line = "data/savestates/" + player_name + "/available.programs";
        std::ifstream ifile(line.c_str());
        if(ifile.is_open())
        {

            Entity::ListElement* tempL = new Entity::ListElement;
            AvailablePrograms = tempL;

            while(getline(ifile,line))
            {
                if(line.length() <= 2 || line[0] == '#')
                    continue;

                int i = 1;
                while(line[i] != ' ')
                    ++i;
                tempL->element    = new Entity::EHead(atoi(line.substr(0,i).c_str()));
                tempL->amount     = atoi(line.substr(++i,line.length()-1).c_str());
                tempL->next       = new Entity::ListElement;
                tempL->next->prev = tempL;

                tempL             = tempL->next;
            }
            //cleaning up
            tempL->prev->next       = NULL;
            AvailablePrograms->prev = NULL;
            delete tempL;
        }
        ifile.close();
    }
	return true;
}

SDL_Rect* GUI::side_screen::Choose_Rect(int number,bool hovering)
{
    SDL_Rect* returnRect = new SDL_Rect;
    returnRect->x = 0;
    returnRect->w = 124;
    returnRect->h = 20;

    switch(number)
    {
    case 0:
        returnRect->y = 0;
        returnRect->h = 133;
        break;
    case 1:
        returnRect->y = 135;
        returnRect->h = 227;
        break;
    case 2:
        returnRect->y = 364;
        break;
    case 3:
        returnRect->y = 388;
        break;
    case 4:
        //Same as 3; unused
        break;
    case 5:
        returnRect->y = 410;
        break;
    case 6:
        returnRect->y = 500;
        break;
    case 7:
        returnRect->y = 522;
        break;
    case 8:
        returnRect->y = 545;
        returnRect->w =  28;
        returnRect->h =   7;
        break;
    default:
        printf("error 555\n");
        returnRect->y = 388;
        break;
    }
    if(hovering)
    {
        if(number > 1 && number < 6)
            returnRect->y += 68;
        else
            if(number == 8)
                returnRect->x += 30;
    }
    return returnRect;
}
