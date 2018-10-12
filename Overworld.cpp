#include "Overworld.h"

#include <cstdio>
#include <cmath>
#include <fstream>

void Overworld::Node::LoadData()
{
    std::string line = "data/Map.txt";
    std::ifstream map_ifile(line.c_str());
    if(map_ifile.is_open())
    {
        bool success = false;
        int i;
        int j;
        while(getline(map_ifile,line))
        {
            //filter empty lines and comments
            if(line.length() == 0 || line[0] == '#')
                continue;
            i = 0;
            while(line[i] != ':')
                ++i;
            //find relevant line
            if(ID == atoi(line.substr(0,i).c_str()))
            {
                success = true;
                //printf("\nID: %i\n",ID);
                //type
                j = ++i;
                while(line[j] != ',')
                    ++j;
                type = atoi(line.substr(i,j-i).c_str());
                //Security level
                i = ++j;
                while(line[j] != ',')
                    ++j;
                Security_level = atoi(line.substr(i,j-i).c_str());
                //Rewards
                i = ++j;
                while(line[j] != ',')
                    ++j;
                Rewards = atoi(line.substr(i,j-i).c_str());
                //x
                i = ++j;
                while(line[j] != ',')
                    ++j;
                x = atoi(line.substr(i,j-i).c_str());
                //y
                i = ++j;
                while(line[j] != ',')
                    ++j;
                y = atoi(line.substr(i,j-i).c_str());
                //name
                ++j;
                while(line[j] != '"')
                    ++j;
                StageName = "";
                while(line[++j] != '"')
                    StageName += line[j];
                //Description
                ++j;
                while(line[j] != '"')
                    ++j;
                StageDescription = "";
                while(line[++j] != '"')
                    StageDescription += line[j];
                //printf("type: %i\nsec: %i\nrewards: %i\nx,y: %i,%i\nName: '%s'\nDescription: '%s'\n",type,Security_level,Rewards,x,y,StageName.c_str(),StageDescription.c_str());
                break;
            }
        }
        map_ifile.close();
    }
}

void Overworld::Node::GetCoordinates(int& mx, int& my)
{
    mx = x;
    my = y;
}

void Overworld::Node::DeactivateFollowing()
{
    state = Inactive;
    //highlighted = false;
    //active = false;
    if(next != NULL)
        next->DeactivateFollowing();
}

bool Overworld::Node::Check(int mx, int my)
{
    if(mx < 100 && my < 100)
    {
        //pressing button to un-highlight level
        state = Inactive;
        //active = false;
        //highlighted = false;
        return false;
    }
    else
    {
        if(ID == 0 && mx<100 && my > SCREEN_HEIGHT-100)
            printf("shop!");
        state = ShowIntro;
        //active = true;
        //highlighted = true;
        return true;
    }
    if(ID != 0 || !available)
        return false;
    active = true;
    printf("shop!\n");
    if(mx < 100 && my < 100)
        return false;
    return true;
}

Overworld::Node* Overworld::Node::Select(int mx,int my)
{
    if(std::abs(mx-x) <= NodeWidth && std::abs(my-y) <= NodeHeight)
    {
        state = ShowIntro;
        //active = true;
        return this;
    }
    state = Inactive;
    //active = false;
    if(next != NULL)
        return next->Select(mx,my);
    return NULL;
}

void Overworld::Node::Draw(int screen_x,int screen_y, Text::TextRenderer* Text)
{
    int delta_x = x - screen_x;
    int delta_y = y - screen_y;
    if(delta_x >=0 && delta_y >= 0)
    {
        if(!available)
            Text->SetColor(true,200,200,200);
        //if(active)
        if(state == ShowIntro)
        {
            Text->Write(delta_x,delta_y,StageName,true);
            Text->Write(delta_x,delta_y+20,StageDescription,true);

            //Text->Draw(delta_x,delta_y,StageName.c_str(),true);
            //Text->Draw(delta_x,delta_y+20,StageDescription.c_str(),true);
        }
        else
            if(state == Highlighted)
                Text->Write(delta_x,delta_y,StageDescription,true);

            else
                Text->Write(delta_x,delta_y,StageName,true);
            //if(highlighted)
            //    Text->Write(delta_x,delta_y,StageDescription,true);
                //Text->Draw(delta_x,delta_y,StageDescription.c_str(),true);
            //else
            //    Text->Write(delta_x,delta_y,StageName,true);
                //Text->Draw(delta_x,delta_y,StageName.c_str(),true);

        if(!available)
            Text->SetColor(true,255,255,255);
    }
    if(next != NULL)
        next->Draw(screen_x,screen_y,Text);
}

Overworld::Node* Overworld::Node::Motion(int mx,int my)
{
    //if(active)
    if(state == ShowIntro || state == ShopWindow)
        return this;
    if(std::abs(mx-x) <= NodeWidth && std::abs(my-y) <= NodeHeight)
    {
        state = Highlighted;
        //highlighted = true;
        return this;
    }
    state = Inactive;
    //highlighted = false;
    if(next != NULL)
        return next->Motion(mx,my);
    return NULL;
}

void Overworld::Node::Deactivate()
{
    state = Inactive;
    //active = false;
    //highlighted = false;
}

bool Overworld::Node::IsActive()
{
    return (state == ShowIntro || state == ShopWindow);
    //return active;
}
bool Overworld::Node::LoadStage(int mx, int my)
{
    if(mx > SCREEN_WIDTH-100 && my < 100)
        return (state == ShowIntro && available && ID != 0);
        //return (active && available && ID != 0);
    else
        return false;
}

std::string Overworld::Node::ReturnName()
{
    return StageName;
}

void Overworld::Map::Deactivate()
{
    Mapview.x = Mapview.y = 0;
    if(ActiveNode != NULL)
    {
        ActiveNode->GetCoordinates(TargetX,TargetY);
        TargetX -= SCREEN_WIDTH/5;
        TargetY -= SCREEN_HEIGHT/5;
        ActiveNode->Deactivate();
        ActiveNode = NULL;
    }
    else
        TargetX = TargetY = -1;

}

void Overworld::Map::Motion()
{
    if(ActiveNode == NULL)
        ActiveNode = ANode->Motion(Mapview.x + mx, Mapview.y + my);
    else
        if(!ActiveNode->IsActive())
        {
            ActiveNode->Deactivate();
            ActiveNode = ANode->Motion(Mapview.x + mx, Mapview.y + my);
        }
}

bool Overworld::Map::loadMedia(std::string PlayerProfile)
{
    //Background
    if( !TextureMap.loadFromFile("images/overwold_background.png",Renderer) )
	{
		printf( "Failed to load overworld background texture!\n" );
		return false;
	}
    MapX = TextureMap.getWidth()-SCREEN_WIDTH;
    MapY = TextureMap.getHeight()-SCREEN_HEIGHT;

    /*
    //Stages
    std::string line = "data/savestates/" + PlayerProfile + "/explored.map";
    std::ifstream stage_ifile(line.c_str());
    if(stage_ifile.is_open())
    {
        while(getline(stage_ifile,line))
        {
            //filter empty lines and comments
            if(line.length() == 0 || line[0] == '#')
                continue;
            if(line == "Visible:")
            {
                printf("Setting up visible nodes.....");

                int i = 0;
                std::string Name;
                std::string temp;
                int type;
                int x,y;
                while(getline(stage_ifile,line) && line != "Available:")
                {
                    i = 0;
                    temp = Name = "";
                    //filter empty lines and comments
                    if(line.length() == 0 || line[0] == '#')
                        continue;
                    while(line[i] != ':')
                        Name += line[i++];

                    //type
                    while(line[++i] != ',')
                        temp += line[i];
                    type = atoi(temp.c_str());
                    //x,y
                    temp = "";
                    while(line[++i] != ',')
                        temp += line[i];
                    x = atoi(temp.c_str());
                    temp = "";
                    while(line[++i] != ',')
                        temp += line[i];
                    y = atoi(temp.c_str());
                    temp = "";
                    //Description
                    while(line[++i] != '"')
                        continue;
                    while(line[++i] != '"')
                        temp += line[i];
                    //Add to list
                    if(ANode == NULL)
                        if(type == 0)
                            ANode = new Shop(x,y,type,Name,temp,false);
                        else
                            ANode = new Stage(x,y,type,Name,temp,false);
                        //ANode = new Node(x, y, type, Name, temp, false);
                    else
                        if(type == 0)
                            ANode->testAdd(new Shop(x,y,type,Name,temp,false));
                        else
                            ANode->testAdd(new Stage(x,y,type,Name,temp,false));
                        //ANode->Add(x, y, type, Name, temp,false);
                }
                printf("done!\n");
            }
            if(line == "Available:")
            {
                printf("Setting up available nodes...");

                int i = 0;
                std::string Name;
                std::string temp;
                int type;
                int x,y;
                while(getline(stage_ifile,line) && line != "Visible:")
                {
                    i = 0;
                    temp = Name = "";
                    //filter empty lines and comments
                    if(line.length() == 0 || line[0] == '#')
                        continue;
                    while(line[i] != ':')
                        Name += line[i++];

                    //type
                    while(line[++i] != ',')
                        temp += line[i];
                    type = atoi(temp.c_str());
                    //x,y
                    temp = "";
                    while(line[++i] != ',')
                        temp += line[i];
                    x = atoi(temp.c_str());
                    temp = "";
                    while(line[++i] != ',')
                        temp += line[i];
                    y = atoi(temp.c_str());
                    temp = "";
                    //Description
                    while(line[++i] != '"')
                        continue;
                    while(line[++i] != '"')
                        temp += line[i];
                    //Add to list
                    if(ANode == NULL)
                    {
                        if(type == 0)
                            ANode = new Shop(x, y, type, Name, temp, true);
                        else
                            ANode = new Stage(x, y, type, Name, temp, true);

                        //ANode = new Node(x, y, type, Name, temp, true);
                    }
                    else
                        if(type == 0)
                            ANode->testAdd(new Shop(x, y, type, Name, temp, true));
                        else
                            ANode->testAdd(new Stage(x, y, type, Name, temp, true));
                        //ANode->Add(x, y, type, Name, temp,true);
                }
                printf("done!\n");
            }
        }
    }
    stage_ifile.close();
    */
}

bool Overworld::Map::Click()
{

    if(ActiveNode != NULL)
    {
        //Check if still active
        //true  -> check if stage should be loaded
        //false -> set activeNode to NULL, don't move map


        if(ActiveNode->Check(mx,my))
        {
            MoveToActive = true;
            ActiveNode->GetCoordinates(TargetX,TargetY);
            TargetX -= (SCREEN_WIDTH/5);
            TargetY -= (SCREEN_HEIGHT/5);
            if(ActiveNode->LoadStage(mx,my))
                return true;
            return false;
        }

        ActiveNode = NULL;
        return false;
    }

    TargetX = Mapview.x + mx;
    TargetY = Mapview.y + my;

    //Look for new active
    ActiveNode = ANode->Select(TargetX,TargetY);

    if(ActiveNode != NULL)
    {
        MoveToActive = true;
        ActiveNode->GetCoordinates(TargetX,TargetY);
        TargetX -= (SCREEN_WIDTH/5);
        TargetY -= (SCREEN_HEIGHT/5);
    }
    else
    {
        TargetX -= (SCREEN_WIDTH/2);
        TargetY -= (SCREEN_HEIGHT/2);
    }
    //Check for out-of-bounds
    if(TargetX < 0)
        TargetX = 0;
    else
        if(TargetX > MapX)
            TargetX = MapX;
    if(TargetY < 0)
        TargetY = 0;
    else
        if(TargetY > MapY)
            TargetY = MapY;
    return false;
}
std::string Overworld::Map::Update(int delay)
{
    //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 )
    {
        //User requests quit
        if( e.type == SDL_QUIT )
        {
            return "/q";
        }
        if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
        {
            SDL_GetMouseState(&mx, &my);
            if(Click())
                return ActiveNode->ReturnName();
        }
        if(e.type == SDL_MOUSEMOTION)
        {
            SDL_GetMouseState(&mx,&my);
            Motion();
        }
        if(e.type == SDL_KEYDOWN)
        {
            switch(e.key.keysym.sym)
            {
            case SDLK_w:
            case SDLK_UP:
                direction[0] = true;
                //TargetY = Mapview.y - speed;
                break;
            case SDLK_a:
            case SDLK_LEFT:
                direction[1] = true;
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                direction[2] = true;
                break;
            case SDLK_s:
            case SDLK_DOWN:
                direction[3] = true;
                break;
            }
            if(ActiveNode != NULL)
            {
                ActiveNode->Deactivate();
                ActiveNode = NULL;
            }
        }
        if(e.type == SDL_KEYUP)
        {
            switch(e.key.keysym.sym)
            {
            case SDLK_w:
            case SDLK_UP:
                direction[0] = false;
                break;
            case SDLK_a:
            case SDLK_LEFT:
                direction[1] = false;
                break;
            case SDLK_d:
            case SDLK_RIGHT:
                direction[2] = false;
                break;
            case SDLK_s:
            case SDLK_DOWN:
                direction[3] = false;
                break;
            //save
            case SDLK_SPACE:
                return "/s";
                break;
            }
        }
    }
    if(TargetX != -1 || TargetY != -1 || direction[0] || direction[1] || direction[2] || direction[3])
        UpdateMap();
    Render();
    return "";
}
void Overworld::Map::UpdateMap()
{
    //Clear active
    if(!MoveToActive && ActiveNode != NULL)
    {
        ActiveNode->Deactivate();
        ActiveNode = NULL;
    }
    //Choose directions
    if(TargetX > -1)
        if(std::abs(TargetX - Mapview.x) < speed)
        {
            TargetX = -1;
            direction[1] = direction[2] = false;
        }
        else
            if(TargetX < Mapview.x)
            {
                direction[1] = true;
                direction[2] = false;
            }
            else
            {
                direction[1] = false;
                direction[2] = true;
            }
    if(TargetY > -1)
        if(std::abs(TargetY - Mapview.y) < speed)
        {
            TargetY = -1;
            direction[0] = direction[3] = false;
            if(TargetX == -1)
                MoveToActive = false;
        }
        else
            if(TargetY < Mapview.y)
            {
                direction[0] = true;
                direction[3] = false;
            }
            else
            {
                direction[0] = false;
                direction[3] = true;
            }
    //Move map
    Mapview.y -= direction[0]*speed;
    Mapview.x -= direction[1]*speed;
    Mapview.x += direction[2]*speed;
    Mapview.y += direction[3]*speed;
    if(Mapview.x < 0)
    {
        Mapview.x = 0;
    }
    if(Mapview.x > MapX)
    {
        Mapview.x = MapX;
    }
    if(Mapview.y < 0)
    {
        Mapview.y = 0;
    }
    if(Mapview.y>MapY)
        Mapview.y = MapY;
}
void Overworld::Map::Render()
{
    TextureMap.render(0,0,Renderer,&Mapview);
    ANode->Draw(Mapview.x,Mapview.y,Text);
}

