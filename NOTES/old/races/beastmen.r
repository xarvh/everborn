/*
BEASTMEN.R		v2.3
 Race: Beastmen
 by Night Walker
*/

//#include "race.h"	no need of these, since they are at the top of R_STD.C
//#include "city.h"
//#include "unit.h"

//UNITS
a_unit_t beastmen_units[] = {
{//============================================================================
	spearmen,
	0,
	0,0,0,0,0,0,0,0,	//anims
	0,			//requirements
	0,			//specials
	20,			//cost
	1,			//speed
	WALK,			//move class
	0,			//overland gfx
	1,	//food
	1,	//gold
	0,	//mana
	2,	//melee
	2,	//defense
	5,	//resist
	2,	//hits
	0,	//to-hit
	0,	//range
	0,	//range_t
	0,	//ammo
	8,	//figures
	0,	//special_f
	SPEARMEN },
{//============================================================================
	swordsmen,
	0,
	0,0,0,0,0,0,0,0,	//anims
	BARRACKS,		//requirements
	LARGE_SHIELD,		//specials
	40,			//cost
	1,			//speed
	WALK,			//move class
	0,			//overland gfx
	1,	//food
	2,	//gold
	0,	//mana
	4,	//melee
	2,	//defense
	5,	//resist
	2,	//hits
	0,	//to-hit
	0,	//range
	0,	//range_t
	0,	//ammo
	6,	//figures
	0,	//special_f
	SWORDSMEN },
{//============================================================================
	halberdiers,
	0,
	0,0,0,0,0,0,0,0,	//anims
	ARMORY,			//requirements
	NEGATE_FIRST_STRIKE,	//specials
	80,			//cost
	1,			//speed
	WALK,			//move class
	0,			//overland gfx
	1,	//food
	3,	//gold
	0,	//mana
	5,	//melee
	3,	//defense
	5,	//resist
	2,	//hits
	0,	//to-hit
	0,	//range
	0,	//range_t
	0,	//ammo
	6,	//figures
	0,	//special_f
	HALBERDIERS },
{//============================================================================
	bowmen,
	0,
	0,0,0,0,0,0,0,0,	//anims
	SAWMILL,		//requirements
	0,			//specials
	60,			//cost
	1,			//speed
	WALK,			//move class
	0,			//overland gfx
	1,	//food
	3,	//gold
	0,	//mana
	2,	//attack
	1,	//defense
	5,	//resist
	2,	//hits
	0,	//to-hit
	2,	//range
	ARROWS,	//range_t
	8,	//ammo
	6,	//figures
	0,	//special_f
	BOWMEN },
{//============================================================================
	priests,
	0,
	0,0,0,0,0,0,0,0,	//anims
	PARTHENON,		//requirements
	HEALER+PURIFY*HEALING_SPELL,	//specials
	150,			//cost
	1,			//speed
	WALK,			//move class
	0,			//overland gfx
	1,	//food
	4,	//gold
	0,	//mana
	4,	//melee
	4,	//defense
	8,	//resist
	2,	//hits
	0,	//to-hit
	4,	//range
	V_MAGIC,//range_t
	4,	//ammo
	4,	//figures
	0,	//special_f
	CLERICS },
{//============================================================================
	magicians,
	0,
	0,0,0,0,0,0,0,0,	//anims
	WIZARDS_GUILD,		//requirements
	IMMUNE_TO_MISSILES+FIRE_BALL_SPELL,	//specials
	180,			//cost
	1,			//speed
	WALK,			//move class
	0,			//overland gfx
	1,	//food
	5,	//gold
	0,	//mana
	2,	//melee
	3,	//defense
	9,	//resist
	2,	//hits
	0,	//to-hit
	5,	//range
	F_MAGIC,//range_t
	4,	//ammo
	4,	//figures
	0,	//special_f
	MAGICIANS },
{//============================================================================
	engineers,
	0,
	0,0,0,0,0,0,0,0,	//anims
	0,			//requirements
	WALL_CRUSHER+CRAFTMAN+BUILD_ROADS,	//specials
	60,			//cost
	1,			//speed
	WALK,			//move class
	0,			//overland gfx
	1,	//food
	3,	//gold
	0,	//mana
	2,	//melee
	1,	//defense
	5,	//resist
	2,	//hits
	0,	//to-hit
	0,	//range
	0,	//range_t
	0,	//ammo
	6,	//figures
	0,	//special_f
	ENGINEERS },
{//============================================================================
	settlers,
	0,
	0,0,0,0,0,0,0,0,	//anims
	0,			//requirements
	CREATE_OUTPOST,		//specials
	120,			//cost
	1,			//speed
	WALK,			//move class
	0,			//overland gfx
	1,	//food
	4,	//gold
	0,	//mana
	1,	//malee
	1,	//defense
	5,	//resist
	20,	//hits
	0,	//to-hit
	0,	//range
	0,	//range_t
	0,	//ammo
	1,	//figures
	0,	//special_f
	SETTLERS },
{//============================================================================
	"Centaurs",
	0,
	0,0,0,0,0,0,0,0,	//anims
	STABLES,		//requirements
	0,			//specials
	100,			//cost
	2,			//speed
	WALK,			//move class
	0,			//overland gfx
	1,	//food
	3,	//gold
	0,	//mana
	3,	//melee
	3,	//defense
	5,	//resist
	3,	//hits
	0,	//to-hit
	2,	//range
	ARROWS,	//range_t
	8,	//ammo
	4,	//figures
	0,	//special_f
	0 },
{//============================================================================
	"Manticores",
	0,
	0,0,0,0,0,0,0,0,	//anims
	ANIMISTS_GUILD,		//requirements
	POISON,			//specials
	150,			//cost
	1,			//speed
	FLY,			//move class
	0,			//overland gfx
	1,	//food
	5,	//gold
	0,	//mana
	5,	//melee
	3,	//defense
	6,	//resist
	7,	//hits
	0,	//to-hit
	0,	//range
	0,	//range_t
	0,	//ammo
	4,	//figures
	6,	//special_f
	FLYER },
{//============================================================================
	"Minotaurs",
	0,
	0,0,0,0,0,0,0,0,	//anims
	ARMORERS_GUILD,		//requirements
	LARGE_SHIELD,		//specials
	200,			//cost
	1,			//speed
	WALK,			//move class
	0,			//overland gfx
	1,	//food
	5,	//gold
	0,	//mana
	12,	//melee
	4,	//defense
	7,	//resist
	12,	//hits
	0,	//to-hit
	0,	//range
	0,	//range_t
	0,	//ammo
	2,	//figures
	0,	//special_f
	ELITE },
{//============================================================================
	trireme,
	0,
	0,0,0,0,0,0,0,0,			//anims
	SHIP_WRIGHTS_GUILD+SEA_AVAILABLE,	//requirements
	0,			//specials
	60,			//cost
	1,			//speed
	SAIL,			//move class
	0,			//overland gfx
	1,	//food
	2,	//gold
	0,	//mana
	6,	//attack
	4,	//defense
	5,	//resist
	10,	//hits
	0,	//to-hit
	0,	//range
	0,	//range_t
	0,	//ammo
	1,	//figures
	0,	//special_f
	LIGHT_SHIP },
};

building_t *beastmen_building_list[MAX_BIC] = {
	&be,
	&be,
	&be,
	&barracks,
	&armory,
	&fighters_guild,
	&armorers_guild,
	&war_college,
	&granary,
	&marketplace,
	&farmers_market,
	&bank,
	&merchants_guild,
	&library,
	&sages_guild,
	&university,
	&alchemists_guild,
	&wizards_guild,
	&shrine,
	&temple,
	&parthenon,
	&cathedral,
	&oracle,
	&ship_wrights_guild,
	&be,
	&maritime_guild,
	&sawmill,
	&foresters_guild,
	&miners_guild,
	&mechanicians_guild,
	&stables,
	&animists_guild,
	&be,
	&city_walls,
	&be,		//stone_of_moloch (gnoll only)
	&fortress,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be,
	&be
};

//MAIN DATA ===================================================================

race_t beastmen = {
	"Beastmen",
	"",
	"gnoll.gfx",	//"beastmen.gfx",
	0,
	beastmen_units,
	beastmen_building_list,
	STANDARD_GROW_RATE,
	MANA_BONUS,
	BEASTMEN,
	sizeof(beastmen_units) / sizeof(a_unit_t)
};

//EOF =========================================================================