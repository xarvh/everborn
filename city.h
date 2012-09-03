/*
 CITY			v0.14
 Cities.

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
#ifndef H_FILE_CITY
#define H_FILE_CITY
//=============================================================================

#define CITY_SETTLERS_POPULATION	750

// city flags
#define CITY_FLAG_OPTIMIZE	(1<<1)	// optimize vs. preserve production
#define CITY_FLAG_BLD_SOLD	(1<<2)

// city_t structure map
#define CITY_INFO_OWNER		(1<<0)	// city owner
#define CITY_INFO_ENEMY		(1<<1)	// non-owner
#define CITY_INFO_LAND		(1<<2)	// overland menu
#define CITY_INFO_CMD		(1<<3)	// commands (production queue)

// buffers
#define CITY_NAME_BF	32
#define CITY_QUEUE_BF	9


#define BLD_INFO_BF	(CITY_NAME_BF * 8)


// bld specials
#define CSPC_ALCHEMIST	(1<<0)
#define CSPC_FOOD	(1<<1)
#define CSPC_FORECAST	(1<<2)
#define CSPC_LIBRARY	(1<<3)
#define CSPC_POPMANA	(1<<4)
#define CSPC_REGULAR	(1<<5)
#define CSPC_VETERAN	(1<<6)
#define CSPC_MINE	(1<<7)
#define CSPC_WALL	(1<<8)

#define REQ_SEA			(1<<30)	// fake
#define REQ_NON_BUILDABLE	(1<<31)	// fake

#define BLD_MAX		30





#define CENCH_STREAM_OF_LIFE	0
#define CENCH_PROSPERITY	1
#define CENCH_ASTRAL_GATE	2
#define CENCH_ALTAR_OF_BATTLE	3
#define CENCH_HEAVENLY_LIGHT	4
#define CENCH_CONSECRATION	5
#define CENCH_INSPIRATIONS	6

#define CENCH_TOTAL		7





/*=============================================================================
 * QUEUE
 */
#define Q_FREE		0
#define Q_UNIT		1	// race unit ID
#define Q_COMMON_UNIT	2	// common unit code
#define Q_BLD		3
#define Q_SPC		4	// TradeGoods, Housing
#define Q_PUSH		5	// used only for communication

#define Q_NORMAL	0
#define Q_ENCHANTED	1
#define Q_MITHRIL	2
#define Q_ADAMANTIUM	3

#define QF_WPN		(0x03)	/* lower 2 bits */
#define QF_WARN		(1<<2)	/* alert player when finished */
#define QF_ASK		(1<<3)	/* ask before buy */
#define QF_BUY		(1<<4)	/* buy as soon as possible */
#define QF_RETURN	(1<<5)	/* copy item at end of queue when finished. */

#define QS_TRADE_GOODS	0
#define QS_HOUSING	1
//#define QA_SPECIAL	((1<<QS_TRADE_GOODS) | (1<<QS_HOUSING))




/*=============================================================================
 * struct BLD
 */
class bldC { public:
    char	Id[CITY_NAME_BF];
    char	Name[CITY_NAME_BF];
    char	Info[BLD_INFO_BF];

    long	Req;
    short	Cost;
    short	Spc;

    short	StatsStart[0];	// used by txtStats()
    short	AniStatus;
    short	AniStart;
    short	AniCount;
    short	AniMode;

    short	Unrest;
    short	ProdB;
    short	GoldB;
    short	PopB;

    short	Gold;
    short	Mana;
    short	Food;
    short	Know;
    short	StatsEnd[0];
};





/*=============================================================================
 * struct CITY
 */
class queueC { public:
    unsigned char	Type;	// SpecificUnit, CommonUnit, Building, Special //
    unsigned char	Flags;	// BUYASAP, REPEAT, ALERT, ABB //
    unsigned char	Times;	// repeat count //
    unsigned char	Id;	// item ID //
};



class brainC;
class raceC;
class cellC;
class utypeC;

class cityC { public:
    cellC*		Cell;
    brainC*		Owner;
    raceC*		Race;

    char		ZeroStart[0];	// used to zero all elements ----------
    char		Ench[CENCH_TOTAL];

    char		Name[CITY_NAME_BF];
    queueC		Queue[CITY_QUEUE_BF], QueueEnd[0];

    long		Blds;
    signed long		ProdStatus;
    signed long		Seed;

    signed short	Pop;		// pop below 1000
    unsigned short	Flags;

    unsigned char	FarmPop;
    unsigned char	WorkPop;
    unsigned char	TotalPop;
    unsigned char	QCounter;	// stores current item queue counter
    char		ZeroEnd[0];	//-------------------------------------

    //
    cityC(char*, cellC*, raceC*);
    ~cityC();

    //
    void Display(double a);
    long VisibleBlds();
    int  BldSellPrice(int k);
    int  TurnsLeft();

    //
    bool IsOutpost();
    bool CanDetect();
    bool BACIView();

    void SpawnUnit(int id);
    void SpawnBld(int id);
    void FinalizeProduction();
    void IsConqueredBy(brainC* player);
    void Raze(brainC* player);

    // misc
    void Think();
    int  Unrest();


    // city_prod: production
    long AvlWeapons(queueC* lq);
    long AvlUnits(queueC* lq);
    long AvlBlds(queueC* lq);
    long AvlSpc(queueC* lq);
    long AvlItems(queueC* lq, int type);

    void ProdControl();
    void ProdOptimize();
    void ProdPreserve();
    void ProdCheck();



    // city_prod: queue
    signed	QProdCost(queueC* q);
    void	QRm(queueC* q);
    void	QPop();
    void	QPush(queueC* q);
    utypeC*	QUnit(queueC* q);
    bldC*	QBld(queueC* q);
    char*	QName(queueC* q);
    long	QBaseCost(queueC* q);
    long	QUpkeep(queueC* q);
    bool	QCheck();


    // city_out:
    bool IsTradingGoods();
    bool IsHousing();
    long Spc();
    cellC* CellAvl(cellC* cell);
    signed Food();
    signed Gold();
    signed Work();
    signed Mana();
    signed Know();
    signed TerrainGoldBonus();
    signed TerrainWorkBonus();
    signed TerrainCostBonus();
    signed TerrainBasePop();
    signed Growth();
    signed MaxFarmers();
};



/*=============================================================================
 * CITY
 */
class unitC;

char* cityExSettlerCheck(unitC* u, char* name);
void cityExSettler(unitC* u, char* name);





/*=============================================================================
 * BLD TXT
 */
int bldDestroyList(bldC** l);

long bldReadReq(char* s, char* e, bldC** l);
char* bldWriteReq(char* bf, long req, bldC** l);

bldC** bldLoadAll(char* s, char* e);
char* bldWriteAll(char* w, bldC** l);

bldC** bldLoadBase(char* bf, char* e);
char* bldWriteBase(char* w);



/*=============================================================================
 * DISPLAY
 */
void city_displayBld(cityC*, int id);
void city_displayProdItem(cityC*, int type, int id);
void city_displayFarmer(cityC* city);
void city_displayWorker(cityC* city);
void city_displayRebel(cityC* city);



#endif
//EOF =========================================================================
