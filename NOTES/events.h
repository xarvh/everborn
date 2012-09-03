


// no permanent events
//	1. come cazzo faccio a eliminarli?
//	2. clutter e basta, non é quello per cui voglio usare gli eventi

// come faccio a mettere delle immagini?
// long imagecode





    eventC::eventC(
	cellC*		pos,	// 0 for global events
	brainC*		player,	// 0 for all players able to see pos

	char* cmd,	// cmd to exec (after focusing) when event is selected

	char* fmt,
	...);


    if(self->focus == pos)
	event pops up immediatly
	and is discarded







        -eventi: frase, posizione-> automatico per tutti quelli che             
	vedono "posizione"                                                  
	>devono poter  essere dichiarati con una maschera che indica        
	CHI pu. vederli!                                                    
	-popup per gli eventi                                                   
	-un evento deve poter essere aggiornato                                 
	(tipo battaglia che finisce, creare due eventi incasina il giocatore)   

    completamento di una unitá/edificio/crescita in una cittá								    


    event to players with view "city proclamed indipendent"




    popup to team owner "city razed, population slaughtered or scattered"



    event to players with view "city was destroyed by wizard"




     new event(FADEAWAY_MESSAGE, tm->Owner->Bit, "Can't get there...")
      pf("Unable to access mirror location!\n");

    global event
    global event forecast
    