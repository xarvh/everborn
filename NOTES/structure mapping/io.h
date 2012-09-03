



class tableItemC {
    tableItemC* Next;

    char* name;
    int size;
    ToStringT toString;
    FromStringT FromString;
    ToBinT toBin;
    FromBinT fromBin;
};



class tableC {
    tableItemC* ItemHook;

    ToString();
    FromString();
    ToBin();
    FromBin();
};




static tableC* SelectedTable;
static void* SelectedObject;


class tableInitC { public:
    tableInitC(void* you, tableC* table)
    {
	// has 'table' been already written?
	if(table->ItemHook) SelectedTable = 0;
	else SelectedTable = table;
	SelectedObject = you;
    }
};

class elementInitC { public:
    elementInitC(
	char* name,    		int size,
	ToStringT toString,	FromStringT FromString,
	ToBinT toBin,		FromBinT fromBin)
    {
	if(!SelectedTable) return;

	// based on size, missing (ie NULL) arguments
	// will be assigned a default
	new tableItem(SelectedTable,
	    name, size,
	    toString, fromString,
	    toBin, fromBin
	);
    }

    elementInitC(char* name, int size)
    {
	if(!SelectedTable) return;
	new tableItem(SelectedTable, name, size);
    }
};






class U32 {
    unsigned long V;
    elementInitC Init;
 public:
    U32():Init("U32", sizeof(U32), 0,0, 0,0) { V = 0;}
    unsigned long& operator[] { return V; }
};



//EOF
