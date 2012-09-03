
/*
ske
    sistema di animazione

skeParm
    set di parametri di animazione, specifici per ogni unità.

skeAnimator
    set completo di animazioni, indipendente dallo skeParm.

skeAnimation:
    set di frames per lùanimazione di una variante di una singola azione.
*/


class skeAnimatorC { public:
    animationC* aniHook;
    unsigned	aniCnt;


};

class skeAnimatorC { public:
    skeAnimationC	;
    skeAnimationC	;
    skeAnimationC	;
    skeAnimationC	;
    skeAnimationC	;    
    skeAnimationC	;
    skeAnimationC	;


};




class bonesTypeC { public:
    char*	name;
    bonesParmC*	Parms;
    bonesParmC* Parms_end;

    Display(...);




};









class bonesC { public:

    bonesTypeC* Type;		//contains parm IDs and descriptions
    double*	ParmValues;	//array of parameters values

    //@@ one day there will be several alternatives for each...
    bonesAniC*	Ani[0];
    bonesAniC*	Idle;
    bonesAniC*	Walk;
    bonesAniC*	Run;
    bonesAniC*	Melee;
    bonesAniC*	Range;
    bonesAniC*	Die;
    bonesAniC*	Cry;
};




bodyDisplay(
    bones,
    flesh,
    skin,
    angles
    )







//EOF
