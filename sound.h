


#include <SDL/SDL_mixer.h>


class soundC {
    Mix_Chunk*	Sound;
    bool	FailedAlready;

 public:
    soundC*	Next;		//@@ required? o_O
    char*	Fn;

    //
    void SetFn(char*);
    void Unload();
    soundC();		// constructor for session data
    soundC(char* fn);	// constructor for base data
    ~soundC();

    //
    int Play();
    int Play(int volume, int dir, int dist);
};



void soundUnloadAll();




//EOF =========================================================================
