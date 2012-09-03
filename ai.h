/*
    usare un
	bool gui
    per decidere se la gui é attiva o no

*/





/*=============================================================================
 *
 */
#define for_AnyPlayer(p)	\
	for(brainC* (p) = game->BrainsHook; (p); (p) = (p)->Next)

#define for_AnyUnit(u)		\
	for(unitC* (u) = game->Units; (u) < game->UnitsEnd; (u)++) if((u)->Exists())

#define for_AnyMapCell(c)	\
	for(cellC* (c) = game->Map.C; (c) < game->Map.Cend; (c)++)

#define for_AnyBattle(b)	\
	for(cellC* _c = game->Map.C; _c < game->Map.Cend; _c++) \
	    if(battleC* (b) = _c->Battle)

#define for_AnyCity(c)	\
	for(cellC* _c = game->Map.C; _c < game->Map.Cend; _c++) \
	    if(cityC* (c) = _c->City)





/*=============================================================================
 *
 */
typedef void (*aiF)();

class aiC { public:
    aiC*	Next;

    aiF		Think;
    aiF		Pick;

    char*	Name;
    char*	Description;


    //
    void	Execute(bool);

    aiC(
	char* name,
	char* descr,
	void (*think)(),
	void (*pick)()
	);
};



/*=============================================================================
 *
 */
aiC* aiGet(char*);
aiC* aiGet(int);





//EOF =========================================================================
