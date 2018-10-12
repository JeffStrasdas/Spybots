#include <fstream>

#include "Game.h"

bool Game::Container::Init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		Window = SDL_CreateWindow( "Spybotics", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( Window == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			Renderer = SDL_CreateRenderer( Window, -1, SDL_RENDERER_ACCELERATED );
			if( Renderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer

				SDL_SetRenderDrawColor( Renderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError() );
					success = false;
				}

				//Initialise SDL_ttf
				if(TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n",TTF_GetError());
                    success = false;
                }
			}
		}
	}

	return success;
}

bool Game::Container::Update(int delay)
{
    switch(GM)
    {
    case GM_Intro:
        PlayerProfile = intro->Update(delay);
        if(PlayerProfile.length() > 0)
        {
            if(PlayerProfile == "\q")
                return true;
            LoadProfile();
            SwapGame(GM_Map);
        }
        break;
    case GM_Map:
        stage_name = hub->Update(delay);
        if(stage_name.length() > 0)
        {
            if(stage_name[0] == '/' && stage_name.length() >= 2)
            {
                switch(stage_name[1])
                {
                case 'q':
                    //QUIT
                    SaveProgress();
                    printf("autosave\n");
                    return true;
                    break;
                case 's':
                    //SAVE
                    SaveProgress();
                    printf("save progress\n");
                    break;
                default:
                    printf("error while processing special characters....\n");
                }
            }
            else
                SwapGame(GM_Game);
        }
        //tiny debug info----
        {
        char _intValue[9];
        itoa(*PlayerMoney,_intValue,10);
        std::string actualText = "Player Credits: ";
        actualText += _intValue;
        TextRenderer->Write(0,0,actualText,true);
        itoa(*Security_Clearance,_intValue,10);
        actualText = "Security level: ";
        actualText += _intValue;
        TextRenderer->Write(0,10,actualText,true);
        }
        //-----
        break;
    case GM_Game:
        if(stage->Update(delay))
        {
            SwapGame(GM_Map);
        }
        break;
    }

    SDL_RenderPresent( Renderer );
    return false;
}

void Game::Container::SwapGame(Mode GameMode)
{
    switch(GameMode)
    {
    case GM_Intro:
        delete hub;
        hub = NULL;
        GM = GM_Intro;
        break;
    case GM_Map:
        //Change to map screen
        switch(GM)
        {
        case GM_Intro:
            //Intro menu most likely useless now; instantiate new if needed!
            delete intro;
            intro = NULL;
            //load hub!
            //load player data?
            //---
            break;
        case GM_Game:
            //Came from match; check for rewards!
            //Cash
            int cash = stage->MatchResults()->Cash;
            if(cash > 0)
                *PlayerMoney += cash;
            printf("Player has gained %i\n",cash);
            //New nodes
            stage->MatchResults()->AmmountOfVisibe;
            delete stage;
            stage = NULL;
            break;
        }
        GM = GM_Map;
        //Create new map if no exists
        if(hub == NULL)
        {
            hub = new Overworld::Map(Renderer,TextRenderer,PlayerProfile.c_str(),PlayerMoney,Security_Clearance,PlayerNodes);

        }
        break;
    case GM_Game:
        GM = GM_Game;
        stage = new Scene::Overlay(Renderer,TextRenderer,PlayerPrograms,stage_name,PlayerProfile);
        hub->Deactivate();
        break;
    }
}

void Game::Container::LoadProfile()
{
    printf("setting up profile %s ... ",PlayerProfile.c_str());
    //other player data
    std::string line = "data/savestates/" + PlayerProfile + "/other.info";
    std::ifstream player_ifile(line.c_str());
    if(player_ifile.is_open())
    {
        while(getline(player_ifile,line))
        {
            //filter empty lines and comments
            if(line.length() == 0 || line[0] == '#')
                continue;
            if(line[0] == 'S' && line.length() > 3)
            {

                *Security_Clearance = atoi(line.substr(line.length()-2,2).c_str());
            }
            if(line[0] == 'C' && line.length() > 3)
            {
                int i = 0;
                while(line[i] != ' ' && i < line.length())
                    ++i;
                *PlayerMoney = atoi(line.substr(i,line.length()-i).c_str());
            }
        }
        printf("done\n");
        player_ifile.close();
    }
    printf("setting up available programs ... ");
    //Player programs
    int counter = 0;
    line = "data/savestates/" + PlayerProfile + "/available.programs";
    std::ifstream program_ifile(line.c_str());
    if(program_ifile.is_open())
    {
        Entity::ListElement* tempL = new Entity::ListElement;
        PlayerPrograms = tempL;

        while(getline(program_ifile,line))
        {
            //filter empty
            if(line.length() <= 2 || line[0] == '#')
                continue;

            int i = 1;
            while(line[i] != ' ')
                ++i;
            tempL->element    = new Entity::EHead(atoi(line.substr(0,i).c_str()));
            tempL->amount     = atoi(line.substr(++i,line.length()-1).c_str());
            tempL->next       = new Entity::ListElement;
            tempL->next->prev = tempL;

            ++counter;
            tempL             = tempL->next;
        }
        //cleaning up
        tempL->prev->next       = NULL;
        PlayerPrograms->prev = NULL;
        delete tempL;
        program_ifile.close();
        printf("%i loaded\n",counter);
    }
    counter = 0;
    printf("setting up available nodes ...... ");
    //available nodes
    line = "data/savestates/" + PlayerProfile + "/available.map";
    std::ifstream map_ifile(line.c_str());
    if(map_ifile.is_open())
    {
        int i = 0;
        int j = 0;
        bool Available = false;
        while(getline(map_ifile,line))
        {
            //filter empty
            if(line.length() <= 2 || line[0] == '#')
                continue;
            //Swap mode
            if(line[0] == 'V')
            {
                Available = false;
                continue;
            }
            if(line[0] == 'A')
            {
                Available = true;
                continue;
            }
            i = j = 0;
            //read IDs
            while(j < line.length())
            {
                i = j;
                while(line[j] != ',' && j < line.length())
                    ++j;
                ++counter;
                if(PlayerNodes == NULL)
                    PlayerNodes = new Overworld::Node(atoi(line.substr(i,j-i).c_str()),Available);
                else
                    PlayerNodes->testAdd(new Overworld::Node(atoi(line.substr(i,j-i).c_str()),Available));
                ++j;
            }
            /*if(PlayerNodes == NULL)
                PlayerNodes = new Overworld::Node(atoi(line.substr(i,j-i).c_str()),Available);
            else
                PlayerNodes->testAdd(new Overworld::Node(atoi(line.substr(i,j-i).c_str()),Available));
            printf("--loaded %i--",atoi(line.substr(i,j-i).c_str()));*/
        }
        printf("%i loaded\n",counter);
        map_ifile.close();
    }
}

void Game::Container::SaveProgress()
{
    //player-info
    std::string line = "data/savestates/" + PlayerProfile + "/other.info";
    std::ofstream player_ofile(line.c_str());
    if(player_ofile.is_open())
    {
        player_ofile << "Security Clearance: " << *Security_Clearance << "\n";
        player_ofile << "Cash: " << *PlayerMoney;
        player_ofile.close();
    }
    //player programs
    line = "data/savestates/" + PlayerProfile + "/available.programs";
    std::ofstream programs_ofile(line.c_str());
    if(programs_ofile.is_open())
    {
        Entity::ListElement* tempL = PlayerPrograms;
        while(tempL != NULL)
        {
            programs_ofile << tempL->element->ID << " " << tempL->amount << "\n";
            tempL = tempL->next;
        }
        programs_ofile.close();
    }
    //map
    line = "data/savestates/" + PlayerProfile + "/available.map";
    std::ofstream map_ofile(line.c_str());
    if(map_ofile.is_open())
    {
        std::string Available = "Available:\n";
        std::string Visible   = "Visible:\n";
        Overworld::Node* tempN = PlayerNodes;
        char buffer[4];
        while(tempN != NULL)
        {
            if(tempN->IsAvailable())
            {
                if(Available.length() > 11)
                    Available += ',';
                itoa(tempN->ReturnID(),buffer,10);
                Available += buffer;
            }
            else
            {
                if(Visible.length() > 9)
                    Visible += ',';
                itoa(tempN->ReturnID(),buffer,10);
                Visible += buffer;
            }
            tempN = tempN->ReturnNext();
        }
        map_ofile << Available << "\n" << Visible;
        map_ofile.close();
    }
}
