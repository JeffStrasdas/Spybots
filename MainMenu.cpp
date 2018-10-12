#include "MainMenu.h"
#include "fileoperation.h"

bool MainMenu::Menu::Click()
{
    if(mx < 100 && my < 100)
        return true;
    return false;
}

void MainMenu::Menu::Motion()
{
    return;
}
bool MainMenu::Menu::Key(SDL_Keycode key)
{
    switch(key)
    {
        case SDLK_w:
        case SDLK_UP:
            --ActiveProfile;
            break;
        case SDLK_s:
        case SDLK_DOWN:
            ++ActiveProfile;
            break;
        case SDLK_KP_ENTER: //! TODO: FIX THIS
            return true;
    }
    //Handle wrapping
    if(ActiveProfile < 0)
        ActiveProfile = profilecount;
    else
        if(ActiveProfile > profilecount)
            ActiveProfile = 0;
    //printf("%s\n",Profiles[ActiveProfile-1].c_str());
    return false;
}
std::string MainMenu::Menu::Update(int delay)
{

    //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 )
    {
        //User requests quit
        if( e.type == SDL_QUIT )
        {
            return "\q";
        }
        //Start with selected profile
        if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
        {
            SDL_GetMouseState(&mx, &my);
            if(Click())
                if(ActiveProfile != 0)
                    return Profiles[ActiveProfile-1];
                else
                    return "\n";
        }
        //Mouse motion
        if(e.type == SDL_MOUSEMOTION)
        {
            SDL_GetMouseState(&mx,&my);
            Motion();
        }
        //Handle keyboard input
        if(e.type == SDL_KEYDOWN)
        {
            if(Key(e.key.keysym.sym))
                if(ActiveProfile != 0)
                    return Profiles[ActiveProfile-1];
                else
                    return "\n";
        }
    }

    Render();
    return "";
}


bool MainMenu::Menu::loadMedia()
{
    //Background
    if( !TextureIntro.loadFromFile("images/testbackground.png",Renderer) )
	{
		printf( "Failed to load overworld background texture!\n" );
		return false;
	}
    //profiles
	fileop::filehandler* tempFH = new fileop::filehandler("data/savestates/");
	profilecount = tempFH->CountFolders();
	Profiles = new std::string[profilecount];
	for(int i = 0; i < profilecount; ++i)
        Profiles[i] = tempFH->Next();
	delete tempFH;
	//initialize pointer
	if(profilecount > 0)
        PlayerName = Profiles[0];
    ActiveProfile = 0;
	return true;
}

void MainMenu::Menu::Render()
{
    TextureIntro.render(0,0,Renderer);
    TextRenderer->Write(90,100-12,"available profiles:",true);
    TextRenderer->Write(100,100,"create new",true);
    if(ActiveProfile == 0)
        TextRenderer->Write(90,100,"o",true);

    for(int i = 0; i < profilecount; ++i)
    {
        TextRenderer->Write(100,112+12*i,Profiles[i],true);
        //Marker
        if(i+1 == ActiveProfile)
            TextRenderer->Write(90,112+12*i,"o",true);
    }
}
