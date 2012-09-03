/*
 UNIT_TRAVEL.CPP	v0.07

 Copyright (c) 2004 Francesco Orsenigo <francesco.orsenigo@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; see the file COPYING.  If not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA.
*/

#include "map.h"
#include "cmd.h"
#include "main.h"
#include "game.h"
#include "unit.h"
#include "utype.h"





/*=============================================================================
 * BASE TRAVEL TIME
 * Evalutates time required for a unit to enter a certain map square.
 * Teams move at the speed of the slowest member!
 *
 * Note that flying ships will slow down WINDWALKING teams if wind is
 * adverse.
 * Also, SAILing units are assumed unable to walk or swim.
 */

#define WIND_NEUTRAL	0	// @@ fakes, will be defined elsewhere
#define WIND_ENEMY	1
#define WIND_FRIENDLY	2

static unsigned utBaseTravelTime(short move, int land, int wind, bool road)
{
 if(land == TERRAIN_UNCROSSABLE) return 10;


 if(move & MOVE_FLY)
    if(wind != WIND_ENEMY)		return 2; // normal flight
    else if(move & MOVE_SAIL)		return 6; // flying ships
    else				return 3; // enemy wind

 if(move & MOVE_WIND)
    if(wind != WIND_ENEMY)		return 2; // normal flight
    else				return 3; // enemy wind

 int b = 0;

 // add NOC
 if(move & MOVE_NOC) move |= MOVE_WALK + MOVE_SWIM;

 switch(land) {
    case TERRAIN_MOUNTAIN:
	if(!(move & MOVE_WALK)) return 10;	// can't swim on mountains...
	if(move & MOVE_PATH || road) b++;
	if(move & MOVE_MOUNTAIN) b++;
	if(move & MOVE_NOC) b++;

	if(b > 1) return 3;	// unit may move quickly even on mountains.
	if(b > 0) return 6;	// unit may move on mountains.
	return 10;		// unit may not move here.

    case TERRAIN_FOREST:
	if(!(move & MOVE_WALK)) return 10;
	if(move & MOVE_PATH || road) b++;
	if(move & MOVE_FOREST) b++;
	if(move & MOVE_NOC) b++;

	if(b > 1) return 3;	// unit may move quickly even in forests.
	if(b > 0) return 6;	// unit may move in forests.
	return 10;		// unit may not move here.

    case TERRAIN_LAND:
	if(!(move & MOVE_WALK))		return 10;
	if(road)			return 3;
	if(move & MOVE_PATH)		return 3;
	if(move & MOVE_NOC)		return 3;
	return 6;

    case TERRAIN_SEA:
    case TERRAIN_SHORE:
	if(move & MOVE_SAIL) switch(wind) {
		case WIND_FRIENDLY:	return 2;
		case WIND_NEUTRAL:	return 3;
		case WIND_ENEMY:	return 6;
	    }
	if(move & MOVE_NOC)		return 3;
	if(move & MOVE_SWIM)		return 6;

	return 10;		// no water movement allowed
 }
}





/*=============================================================================
 * SET TEAM SPC
 */
static long utTeamSpc;



static void utSetTeamSpc(unitC* tm)
{
 utTeamSpc = 0;

 for(unitC* u = tm->Cell->Unit; u; u = u->Next) if(u->Team == tm->Team)
 {
    utTeamSpc |= u->Type->Move;
    //@@ add enchantements
 }

 // keep only specials that yeld for the whole team
 utTeamSpc &= TEAM_MOVE;
}





/*=============================================================================
 * TEAM TRAVEL TIME
 *
 * If this function is called during the team movement, the units won't
 * be on the same cell, so we check the whole unit buffer instead than
 * the u->Cell->Unit list.
 *
 * Requires utTeamSpc and utTeamSize to be set.
 */
static signed teamTravelTimeTo(unitC* tm, cellC* dest, unsigned direction)
{
 int land = dest->Land->Type;

 // wind
 int wind = 0;

 // road?
 bool road = direction < 8 && tm->Cell->HasRoad(direction);



 // check all units movements within the team
 int max_time = 0;
 for(unitC* u = tm->Cell->Unit; u; u = u->Next) if(u->Team == tm->Team)
    if(!u->Transported || u->Team != u->Transported->Team)
    {
	int time = utBaseTravelTime(utTeamSpc | u->Type->Move, land, wind, road);

	// flag units that stop team movement
	//@@u->BlocksTeam = (time > 6);

	// team travel time = slowest unit travel time
	if(time > max_time)
	    max_time = time;
    }

 return max_time;
}





bool unitC::CanEscape(int direction)
{
 cellC* dest = Cell->SMAC(direction);

 long tm = Team;
 Team = 0;
 utSetTeamSpc(this);

 // check if unit can actually -move- to destination cell
 signed r = teamTravelTimeTo(this, dest, direction);

 Team = tm;
 if(r > 6) return false;

 // also, units cannot escape to squares occupied by enemies
 if(dest->CheckBattle(Owner? Owner->Bit : BRAIN_NEUTRAL)) return false;

 return true;
}





/*=============================================================================
 * TRAVEL
 */
bool unitC::TeamCanAffordTime(int time)
{
 for(unitC* u = Cell->Unit; u; u = u->Next) if(u->Team == Team)
    if(!u->Transported || u->Transported->Team != Team)
	if(u->TimeAvailable < time) return false;
 return true;
}





/*=============================================================================
 * PATH
 */
static gridC* pathOverlandGrid;
static unitC* pathTeam;
static cellC* pathEnd;
static char utZ;



static int ABS(int q) { return q >0? q:-q;}
static int MIN(int a, int b) { return a>b ? b:a; }
static int MAX(int a, int b) { return a>b ? a:b; }



/*=============================================================================
 * PATH COST
 */
static unsigned pathOverlandCost(int x, int y, int incoming_dir)
{
 cellC* c = map(x,y,utZ);

 int cost;
 if(!pathTeam->Owner || (c->KnowMask & pathTeam->Owner->Bit)) // known cell?
    cost = 10*teamTravelTimeTo(pathTeam, c, (incoming_dir+4)&7);
 else
    cost = 59;	// slightly favour exploring unknown cells

 // transport?
 if(cost > 60 && c == pathEnd)
    if(pathTeam->TeamCanEnterTransportsAt(pathEnd))
	cost = 60;

 if(incoming_dir&1) cost++;	// slightly favour straighter paths

// pf("%d %d : cost %d\n", x, y, cost);

 return cost > 61 ? 0 : cost;	// max cost is now 61 ;-)
}





/*=============================================================================
 * HEU
 */
static unsigned pathOverlandHeu(int sx, int sy, int ex, int ey)
{
 int dw = ABS(sx - ex);
 int dh = ABS(sy - ey);

 dw = MIN(dw, game->Map.W-dw);	// X direction wraps!

 //@@ this should be optimized to account for team movement abilities
 return 60*MAX(dw, dh);
}





/*=============================================================================
 * TEAM PATH
 */
static void checkOverlandGrid()
{
 // create grid if not yet allocated
 if(pathOverlandGrid)
    if(pathOverlandGrid->W != game->Map.W || pathOverlandGrid->H != game->Map.H)
	delete pathOverlandGrid;

 if(!pathOverlandGrid)
    pathOverlandGrid = new gridC(game->Map.W, game->Map.H);
}





bool utTeamPath(unitC* tm)
{
 checkOverlandGrid();		// allocates/reallocates as necessary

 mapC& map = game->Map;		// fetch map
 cellC* start = tm->Cell;	// start and end positions
 cellC* end = (cellC*)tm->LandOrders.WP->P;

 //@@!!!!!
 if(!end) { pf("pathabort: noordrs\n"); return false; }

 utSetTeamSpc(tm);		// set team abilities
 pathTeam = tm;			// set team
 utZ = start->Z;
 pathEnd = end;

 float heu = pathOverlandGrid->Pathfind(
    start->X, start->Y, end->X, end->Y,
    0,
    pathOverlandCost, pathOverlandHeu);


 if(heu == .0)
 {
    // we got a path, so write it to all units in the team

    char* path = pathOverlandGrid->Path;
    unsigned pathL = pathOverlandGrid->PathLength;

    for(unitC* u = tm->Cell->Unit; u; u = u->Next) if(u->Team == tm->Team)
	if(!u->Transported || u->Team != u->Transported->Team)
	    u->LandOrders.Path.Make(path, pathL);
    return true;
 }
 else
 {
    // remove order to all units in the team
    for(unitC* u = tm->Cell->Unit; u; u = u->Next) if(u->Team == tm->Team)
	u->LandOrders.Clear();

    //@@ new event(FADEAWAY_MESSAGE, tm->Owner->Bit, "Can't get there...")
    if(tm->Owner == self) pf("No idea how to get there! =(\n");
    return false;
 }
}





/*=============================================================================
 * DESTRUCTOR, CONSTRUCTOR
 */
travelC::~travelC()
{
 travelC** h;
 for(h = &game->TravelsHook; *h && *h != this; h = &(*h)->Next);

 // detach from chain
 if(*h) *h = Next;

 // clean all references
 for(unitC* u = Src->Unit; u; u = u->Next) if(u->Travel == this) u->Travel = 0;
 for(unitC* u = Dest->Unit; u; u = u->Next) if(u->Travel == this) u->Travel = 0;
}


static void utPlaneShiftFailed(unitC* tm)
{
 //@@ event register....
 if(tm->Owner == self) pf("Unable to access mirror location!\n");

 for(unitC* u = tm->Cell->Unit; u; u = u->Next) if(u->Team == tm->Team)
    u->LandOrders.Pop();
}





travelC::travelC(unitC* team, char type, int dir)
{
 bool road = (type == TRAVELT_ROAD);

 Next = 0;
 Team = team;
 Type = type;
 StatusCode = TRAVEL_JUST_STARTED;

 Src = team->Cell;
 Dest = Type != TRAVELT_SHIFT ? Src->SMAC(dir) : Src->Mirror();
 Cost = teamTravelTimeTo(team, Dest, dir);
 Status = 0;
 Direction = dir;


 //@@ should it become a class attribute?
 bool disembark = Team->Transported && (Team->Team != Team->Transported->Team);



 // make sure that cells are elegible for road construction
 if(road) if(!Src->Land->RoadAvailability || !Dest->Land->RoadAvailability)
 {
    //pf("noroad!\n");
    StatusCode = TRAVEL_BLOCKED;
    return;
 }


 if(Cost > 6) // movement can NOT be performed
 {
    // attempt to load team
    if(!road) if(team->TeamCanAffordTime(6))
	if(team->TeamEnterTransportsAt(Dest))
	{
	    // team entered, no more need to move
	    delete this;
	    return;
	}

    // movement cannot be performed, and transport is not an option
    // rebuild the path and abort movement
    StatusCode = TRAVEL_BLOCKED;
    if(Type == TRAVELT_SHIFT) utPlaneShiftFailed(team);
    else utTeamPath(team);

    // either case, movement is not going to be issued
    //@@ event "movement interrupted"?
    return;
 }


 // plane shift  always takes whole turn
 if(Type == TRAVELT_SHIFT)
    Cost = 6;


 // register on chain....
 travelC** h;
 for(h = &game->TravelsHook; *h; h = &(*h)->Next);
 *h = this;

 //... and on units
 for(unitC* u = Src->Unit; u; u = u->Next) if(u->Team == team->Team)
 {
    if(u->Travel) delete u->Travel; // ~travelC() will zero u->Travel
    if(!u->Transported || disembark) u->Travel = this;
 }
}





/*=============================================================================
 * THINK
 */
static bool makeRoad(cellC* c, int d)
{
 unsigned char* r = c->RoadStatus + (d&7);

 if(*r == 0) return true;	// road already available here

 *r -= c->Land->RoadAvailability;
 if(*r > 100) *r = 0;
 return false;
}



bool travelC::Think()
{
 // completed already?
 if(StatusCode == TRAVEL_COMPLETED)
    return true;



 // check road
 if(Type == TRAVELT_ROAD && !Src->HasRoad(Direction))
 {
    // all engineers build road
    for(unitC* u = Team->Cell->Unit; u; u = u->Next) if(u->Travel == this)
	if(u->TimeAvailable > 0)
	{
	    u->TimeAvailable--;
	    if(u->Type->Cmd & CMD_BUILD_ROAD)
		if(makeRoad(Src, Direction)) makeRoad(Dest, Direction+4);
	}

    int road_status = Src->RoadStatus[Direction] + Dest->RoadStatus[(Direction+4)&7];
    Status = (200 - road_status) / (200/5);

    // road not completed, do not interrupt
    return false;
 }



 // when movement hasn't yet started,
 // check if there is enough time available to perform it this turn.
 if(StatusCode == TRAVEL_JUST_STARTED || Type == TRAVELT_ROAD)
    if(!Team->TeamCanAffordTime(Cost))
	return false; // wait next turn!
    else
    {
	// start animation
	StatusCode = TRAVEL_ANIMATING;

	// deploy units if necessary
	if(Team->Transported && Team->Transported->Team != Team->Team)
	    Team->TeamExitTransport();
    }



 // advance movement
 if(Cost)	Status += 6 / Cost;
 else		Status = 6;


 // arrived?
 if(Status < 6)
    return false;	// movement NOT completed


 // arrived!
 StatusCode = TRAVEL_COMPLETED;
 Team->TeamDisplace(Dest);
 for(unitC* u = Dest->Unit; u; u = u->Next) if(u->Travel == this)
 {
    u->TimeAvailable -= Cost;
    u->LandOrders.Path.Pop();
 }



 // movement completed
 return true;
}





//EOF =========================================================================
