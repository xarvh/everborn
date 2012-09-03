



#include <string.h>
#include <stdlib.h>
#include <SDL/SDL_mixer.h>
#include "sound.h"
#include "data.h"
#include "main.h"



/*=============================================================================
 *
 */
soundC* soundBaseHook = 0;
soundC* soundTempHook = 0;





/*=============================================================================
 * CONSTRUCTORS, DESTRUCTOR
 */
soundC::soundC()
{
 FailedAlready = false;
 Next = soundTempHook;
 soundTempHook = this;
 Fn = 0;
 Sound = 0;
}

soundC::soundC(char* fn)
{
 FailedAlready = false;
 Next = soundBaseHook;
 soundBaseHook = this;
 Fn = strdup(fn);
 Sound = 0;
}



soundC::~soundC()
{
 //@@ unregister!!!
 if(Fn) free(Fn);
 if(Sound) Mix_FreeChunk(Sound);
}


void soundC::Unload()
{
 if(Sound) Mix_FreeChunk(Sound);
 Sound = 0;
}





/*=============================================================================
 *
 */
void soundUnloadAll()
{
 for(soundC* s = soundBaseHook; s; s = s->Next) s->Unload();
 for(soundC* s = soundTempHook; s; s = s->Next) s->Unload();
}



void soundC::SetFn(char* fn)
{
 if(Fn) free(Fn);
 Fn = strdup(fn);
}





/*=============================================================================
 *
 */
int soundC::Play(int volume, int dir, int dist)
{
 if(FailedAlready) return -1;

 if(!Sound)
    if(char* path = dataFindPath(Fn))
    {
	Sound = Mix_LoadWAV(path);
	if(!Sound)
	    pf("Mix_LoadWAV: failed to load '%s': %s.\n", path, Mix_GetError());
	delete[] path;
    }
    else pf("soundC::Play: can't find '%s'.\n", Fn);

 if(!Sound)
 {
    FailedAlready = true;
    return -1;
 }



 Mix_VolumeChunk(Sound, volume);
 int ch = Mix_PlayChannelTimed(-1, Sound, 0, -1);//5000);
 if(ch == -1) return -1;
    //what to do? o_O

//@@ crashes after restarting menues (not in fast mode!?)!!!!!
//@@ Mix_SetPosition(ch, dir, dist);
 return ch;
}



int soundC::Play()
{
 return Play(MIX_MAX_VOLUME, 0, 0);
}



//EOF =========================================================================
