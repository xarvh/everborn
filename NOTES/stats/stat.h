




il massimo sarebbe:

statC:
    signed short	valore di base (utype + esperienza)
    unsigned short	esperienza accumulata
    unitC&		puntatore
    statTypeC&		descrittore del tipo, con display, descrizione e tutto

    //
    costruttore		setta unitC& e statTypeC&
    AddExperience
    operator()
----
totale 12 byte




class statTypeC:
    char* name
    char* descr

    int gfxId

    //
    display(unitC* )
    evaluate(unitC* )









----------------------------------------------------

// sto confondendo ancora utypeC con unitC....


class statC { public:




    signed short Offset; // (char*)this - (char*)unitC
    signed short Value;



    //
    statC(unitC* );
    unitC* unit;
};


// EOF
