

/*
    c'é corrispondenza tra questi e gli stat in unit->Type?
    si, c'é corrispondenza 1 a 1

    accuracy
    speed
    melee
    defense
    resist
    life

    breathe
    thrown
    lifesteal
    poison
*/    

void displayStat(int gfx, int std, signed mod);


class statC {
//    char*	Name;

//    int		GfxBase;

    void	(*Display)(unitC*);
    signed	(*Total)(unitC*);
//    signed	(*Mod)(unitC*);

//    void	Display();
};



signed unitC::Melee()
{
}

void unitC::MeleeDisplay()
{
}


class statC {

}


//EOF
