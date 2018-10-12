#ifndef FIELD_H
#define FIELD_H

#include "EHead.h"
#include "IMG.h"

//Field dimensions
const int FIELD_WIDTH   = 14;
const int FIELD_HEIGHT  = 11;

//Offset for the Field
const int X_OFFSET      = 168;
const int Y_OFFSET      = 34;

namespace field
{
struct GameRewards
{
    //Ammount of credits the player earned during match
    int Cash                = 0;
    //upgrade Security level every 10 points;
    int SecurityBonus       = 0;
    //Which element of the story should be played? (0 for none)
    int StoryNode           = 0;
    //length of ID-Arrays
    int AmmountOfVisibe     = 0;
    int AmmountOfAvailable  = 0;
    //Only contains IDs
    int* AVisible   = NULL;
    int* AAvailable = NULL;
};

class Area
{
public:
    Area(int* GameMode, SDL_Renderer* Renderer, std::string stage_name = "1") : GameMode(GameMode),Renderer(Renderer)
    {
        //Initialize rewards
        Rewards = new GameRewards;
        PlayerMoney = StageMoney = 0;
        Story = Unlocks = Visible = "";
        //Set up Fields
        Graphical  = new  int*[FIELD_WIDTH];
        Useable    = new bool*[FIELD_WIDTH];
        Distance   = new  int*[FIELD_WIDTH];

        for(int x=0;x<FIELD_WIDTH;x++)
        {
            Graphical[x] = new  int[FIELD_HEIGHT];
            Useable[x]   = new bool[FIELD_HEIGHT];
            Distance[x]  = new  int[FIELD_HEIGHT];
            for(int y=0;y<FIELD_HEIGHT;++y)
            {
                Graphical[x][y] = 0;
                Useable[x][y]   = true;
                Distance[x][y]  = -1;
            }
        }
        //Set up Anchor entities
        AE_List = new Entity::EHead*[3];
        for(int i=0;i<3;i++)
            AE_List[i] = NULL;
        //Set up active objects
        ActiveEntity = NULL;
        ActiveAction = NULL;
        HasObjective = false;
        Generate(stage_name);
        LoadMedia();
    };
    ~Area()
    {
        close();
	};
	//Concerning UNDO
	void Apply();
	void Revert();

	//
    //Read Data
    void Generate(std::string filename);
    //Manipulates area
    void Add(int x,int y);
    void Remove(int x,int y);
    //Entity relevant method
    void Set(int x,int y, bool free);
    //Draws area on screen
    void Draw();
    //Changes game mode
    void SwapGameMode();
    //Change active action
    void SetActive(void* setTo, bool isAction = true);
    //
    bool AI_Action();

    void MatchResults(int& Player, int& Stage);

    Entity::EHead* newClick(int mx, int my);
    Entity::EHead* Click(int x,int y);
    //Keyboard input
    Entity::EHead* Key(SDL_Keycode code);
    //Load
    bool LoadMedia();
    //Begin battle
    bool Begin();
    //Fill the Distance-Field
    void DetermineDistance();
    void DetermineDistance(Entity::EHead* Object,int x,int y, int Offset = 0);
    void AI_DetermineDistance(Entity::EHead* Object, Entity::EHead* Target, int x, int y, int Offset = 0);

    //Return data
    bool NoActive();
    Entity::Action* ReturnAction();
    bool ReturnUseable(int x,int y);
    std::string BackgroundPath();

    void Dump_Field(std::string filename);

    void close();

private:
    Entity::EHead* Move(int x,int y);
    //Choose fitting rectangle;     0 -> Background     1 -> Entity head     2 -> Entity body
    SDL_Rect* Choose_Rect(int ID,int Mode);

    int**  Graphical;
    bool** Useable;
    //Used for movement
    int** Distance;

    int* GameMode; // 0 -> Initializing    1 -> Player turn    2 -> Enemy turn    3 -> Battle over    4 -> Story
    bool HasObjective;

    //Data used for later
    int PlayerMoney;
    int StageMoney;
    std::string Background;
    GameRewards* Rewards;
    std::string Story;
    std::string Unlocks;
    std::string Visible;
    //Anchor entities
    Entity::EHead**  AE_List; // 0 -> player    1 -> enemy    2 -> other
    //Active objects
    Entity::EHead*  ActiveEntity;
    Entity::Action* ActiveAction;

    LTexture EntityHeadTexture;
    LTexture EntityBodyTexture;
    LTexture BackgroundTexture;
    SDL_Renderer* Renderer;
};
}
#endif
