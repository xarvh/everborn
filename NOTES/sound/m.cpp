
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>



#define pe printf
#define pf printf
#include "sound.h"



//soundC sound("/home/nw/usr/KDE_Beep.ogg");
soundC sound("/home/nw/sfx/swamp.ogg");



int main()
{
 SDL_Init(SDL_INIT_AUDIO);

 if(Mix_OpenAudio(44100, AUDIO_S16, 2, 4096))                                   
// if(Mix_OpenAudio(22050, AUDIO_S16, 2, 4096))                                   
 {                                                                              
      pe("%sMix_OpenAudio() failed: %s.\n", "", Mix_GetError());                  
      exit(-1);                                                                   
 }                                                                              
 atexit(Mix_CloseAudio);                                                        
 



 Mix_Music* music = Mix_LoadMUS("/home/nw/Musica/Ehma_Pizzicato.ogg");
// Mix_PlayMusic(music, 0);


 int channel = -1;
 int volume = 128;
 int dir = 0;

 do switch(getchar()) {
    case 'q': exit(0);

    case '+': volume++; Mix_VolumeMusic(volume); break;
    case '-': volume--; Mix_VolumeMusic(volume); break;

    case 'z': dir -= 10; Mix_SetPosition(channel, dir, 0); break;
    case 'x': dir += 10; Mix_SetPosition(channel, dir, 0); break;

    case 'p': channel = sound.Play(128); break;
 } while(1); //Mix_PlayingMusic());



 //
 return 0;
}


// EoF ========================================================================
