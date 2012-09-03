
/*=============================================================================
    class bodyC;

    class utypeC {
	bodyC*	BodiesHook;
	....
    };
*/

#define ANIMATION_IDLE	0
#define ANIMATION_WALK	1
#define ANIMATION_MELEE	2
...
#define ANIMATIONS_CNT	3



/*=============================================================================
 * ANI PARMS
 *
 * These are just 'suggestion' trying to characterize the unit movements,
 * each animation will have its own interpretation of each parm.
 */
#define ANIPARM_AMPLITUDE	0	// amplitude of movements
#define ANIPARM_FREQUENCY	1	// nervousness of movements
#define ANIPARM_APERTURE	2	// broadness of movements

#define ANIPARMS_CNT		3




/*=============================================================================
 * SKE
 */
class skeC { public:
    char*	Name;

    void	(*Display)(bodyC*, int ani, double t);

    jointC*	JointsHook;
    partC*	PartsHook;
...

    animationC*	AvailableAnimations;
};





/*=============================================================================
 * BODY
 */
class bodyC { public:
    bodyC*	Next;
    bodyC*	Parent;

    skeC*	Ske;
    float	AniParms[ANIPARMS_CNT];
    animationC*	AniTable[ANIMATIONS_CNT];

    fleshC*	FleshTable;
};








// EOF ========================================================================
