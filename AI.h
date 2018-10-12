#ifndef AI_H
#define AI_H

#include "EHead.h"

namespace AI
{
    /*
//Linear AI, true if someone dies
bool Linear(bool aggressive,Entity::EHead** EList, int Index,int* Current, int** Field);
//Idiot-Tier-AI, true if someone dies
bool Idiot(Entity::EHead** EList, int Index,int* Current, int** Field);
//0 - Attack random, 1 - Attack target with least HP, 2 - Attack strongest
bool Attack(int mode, Entity::EHead** EList, int Index,int* Current);
//Check if field is open
bool Check(Entity::EHead** EList, int Index,int* Current, int** Field,int x, int y);
*/
//new Linear
bool new_Linear(bool aggressive, Entity::EHead** AE_List, Entity::EHead* Current, int** Field );
//new Idiot
bool new_Idiot(Entity::EHead** AE_List,Entity::EHead* Current, int** Field);
//new Attack
bool new_Attack(int mode,Entity::EHead** AE_List, Entity::EHead* Current);
//new Check
bool new_Check(Entity::EHead** EList,Entity::EHead* Current, int** Field,int x, int y);

}
#endif
