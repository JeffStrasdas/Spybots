#include "Scene.h"
#include <fstream>

field::GameRewards* Scene::Overlay::MatchResults()
{
    //Rewards->Cash = area->MatchResults();
    //Setting up other Rewards.....
    return Rewards;
}

bool Scene::Overlay::Update(int delay)
{
    EarlyUpdate(delay);


    //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 )
    {
        //User requests quit
        if( e.type == SDL_QUIT )
        {
            Rewards->Cash = 0;
            return true;
        }
        if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
        {
            SDL_GetMouseState(&mx, &my);
            if(Click(mx,my))
            {
                int a,b;
                area->MatchResults(a,b);
                printf("Close databattle -- Player earned %i by pickups and %i as stage-reward\n",a,b);
                return true;
            }
        }
        if(e.type == SDL_MOUSEMOTION)
        {
            SDL_GetMouseState(&mx, &my);
            Motion(mx,my);
        }
        if(e.type == SDL_KEYUP)
        {
            area->Key(e.key.keysym.sym);
        }
    }

    if(LateUpdate())
        SwapGameMode(delay);
    //
    Draw();
    return false;
}

void Scene::Overlay::Begin()
{
    if(area->Begin())
    {
        NoActive = true;
        *GameMode = 2;
        left_side->Begin();
    }
    else
    {
        printf("can't begin!\n");
    }
}

bool Scene::Overlay::Click(int x,int y)
{
    //printf("%i,%i\n",x,y);
    if(!ShowLogoutMenu)
    {
        if(*GameMode == 3)
        {
            //Player wins databattle
            int a,b;
            area->MatchResults(a,b);
            Rewards->Cash = a+b;

            return true;
        }
        if(x > X_OFFSET && y > Y_OFFSET)
        {
            if(*GameMode == 0 && x < 325 && y > 385 && x > 200 && y < 406)
            {
                Begin();
            }
            else
            {
                //On area
                left_side->SetActive(area->newClick(x,y));
            }
        }
        else
        {
            if(x<=X_OFFSET)
            {
                //On the left side
                if(*GameMode == 0 && y < 155)
                    area->SetActive(left_side->newClick(x,y,false),false);
                else
                    area->SetActive(left_side->newClick(x,y));
            }
            else
                if(x > LOGOUT_X && y < LOGOUT_Y && x <= LOGOUT_X+LOGOUT_W)
                {
                    //Logout
                    ShowLogoutMenu = true;
                }
                else
                    printf("not handled territory");
        }
    }
    else
    {
        if(logoutHover[0])
        {
            //Hide logout
            ShowLogoutMenu = false;
        }
        if(logoutHover[1])
        {
            //log out
            //Clean Rewards-object
            Rewards->Cash = 0;
            Rewards->SecurityBonus = 0;
            Rewards->StoryNode = 0;
            Rewards->AmmountOfAvailable = 0;
            Rewards->AmmountOfVisibe = 0;
            delete[] Rewards->AAvailable;
            delete[] Rewards->AVisible;

            return true;
        }
    }
    return false;
}

void Scene::Overlay::Motion(int mx,int my)
{
    if(!ShowLogoutMenu)
        left_side->Motion(mx,my);
    else
        if(mx > 255 && mx <= LOGOUT_X && my > 180 && my <= 220)
            if(my <= 200)
            {
                logoutHover[0] = true;
                logoutHover[1] = false;
            }
            else
            {
                logoutHover[0] = false;
                logoutHover[1] = true;
            }
        else
            logoutHover[0] = logoutHover[1] = false;
}

void Scene::Overlay::Draw()
{

    //Clear screen
    SDL_SetRenderDrawColor( Renderer, 0, 0, 0, 0xFF );
    SDL_RenderClear( Renderer );
    BackgroundTexture.render(0,0,Renderer);
    //Background and battle splash
    BorderTexture.render(0,0,Renderer);
    //Draw normal things
    left_side->Draw();
    area->Draw();
    //Draw log out window
    if(ShowLogoutMenu)
    {
        SDL_Rect tempRect = {0,0,252,149};
        LogOutTexture.render(255,75, Renderer, &tempRect);
        if(logoutHover[0])
        {
            tempRect = {0,149,252,20};
            LogOutTexture.render(257,175, Renderer, &tempRect);
        }
        if(logoutHover[1])
        {
            tempRect = {0,169,252,20};
            LogOutTexture.render(257,193, Renderer, &tempRect);
        }
        TextR->Write(270,134,"Do you wish to exit this databattle and return to the netmap?",true,200);
        //TextR->Draw(270,134,"Do you wish to exit this databattle and return to the netmap?");
        //Text::Draw(Renderer,270,134,"Do you wish to exit this databattle and return to the netmap?",FontSmall,{255,255,255},260);
    }
}

void Scene::Overlay::SwapGameMode(int delay)
{
    delta_t += delay;
    if(delta_t <= MESSAGE_DELAY)
    {
        //Show Message
        if(delta_t%100 == 0)
            printf("%i\tshow %s turn\n",delta_t,*GameMode == 1 ? "Enemies'" : "Players'");
    }
    else
    {
        //Begin other turn
        printf("actually change game mode now from %i",*GameMode);
        *GameMode == 1 ? *GameMode = 2 : *GameMode = 1;
        printf(" to %i\n",*GameMode);

        left_side->SwapGameMode();
        area->SwapGameMode();

        NoActive = false;
        delta_t = 0;
    }
}

void Scene::Overlay::EarlyUpdate(int delay)
{
    switch(*GameMode)
    {
    case 1:
        if(ChangeSize != 0)
        {
            delta_t += delay;
            if(delta_t > 200)
            {
                printf("heal\n");
                ChangeSize > 0 ? --ChangeSize : ++ChangeSize;
            }
        }
        break;
    case 2:
        if(!NoActive)
        {
            delta_t += delay;
            if(delta_t > 400)
            {
                area->AI_Action();
                delta_t = 0;
            }
        }
        break;
    default:
        break;
    }
}


bool Scene::Overlay::LateUpdate()
{
    if(*GameMode == 1 || *GameMode == 2)
        if(!NoActive)
            if(area->NoActive())
            {
                NoActive = true;
                return true;
            }
            else
                return false;
        else
            return true;
    else
        return false;
}

bool Scene::Overlay::LoadMedia()
{
    //load images
	if( !BorderTexture.loadFromFile("images/battle_splash.png",Renderer) )
	{
		printf( "Failed to load battle splash texture!\n" );
		return false;
	}
    if( !LogOutTexture.loadFromFile("images/log_out.png",Renderer) )
	{
		printf( "Failed to load background texture!\n" );
		return false;
	}
	if(area->BackgroundPath().length() == 0 || !BackgroundTexture.loadFromFile(area->BackgroundPath(),Renderer) )
    {
        if( !BackgroundTexture.loadFromFile("images/background.png",Renderer) )
        {
            printf( "Failed to load default background texture!\n" );
            return false;
        }
    }
	return true;
}
