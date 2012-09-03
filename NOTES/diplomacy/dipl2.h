

/*
 * COMMITTMENT TYPE
 *
 * descrive un impegno che puó essere preso da una delle parti
 */
@@ definire bene la condizione di fallimento!!!!


committmentTypeC
 Istantanei:
f    * dare una cittá
	il contratto segna posizione della cittá, pop, valore totale buildings
	perché il contratto sia rispettato pop e valore totale devono essere
	pari o superiori.

    * dare un pick

    * rivelare la propria EVC all'altra parte

    * cancel a single Wrath of the Everborns

 Continui:
f    * dare XX mana per turno
f    * dare XX gold per turno

f    * non aggressione durante il transito attorno alla cittá X
f    * non aggressione durante il transito attorno alle cittá
f    * non aggressione durante il transito fuori dalle cittá
f    * non transito attorno alla cittá X
f    * non transito attorno alle cittá

    * truce: no spawn battle outside cities
    * alleanza: no spawn battle

    * cambiare la propria EVC verso l'altra parte
    * permettere vittoria
    * subordinated victory (can't win untill the other part wins too)

    * share cities view
    * share troops view

    * don't use Wraith of the Everborns

?    * put enchantement X on Y



/*
 * COMMITTMENT
 *
 * descrive un impegno preso da una delle parti
 */
committmentC
    committmentC*	Next
    committmentTypeC*	Type
    argumentC*		Args
    bool		Evaded	// just to mark what's not needed anymore


/*
 * CLAUSE
 *
 * Two list of committments and their duration
 */
clauseC
    clauseC*		Next
    int			Duration
    committmentC*	ProposerCommittment
    committmentC*	AccepterCommittment



/*
 * CONTRACT
 */
contractC
    brainC*	Proposer
    brainC*	Accepter
    clauseC*	Clauses	// clauses, in chronological order


//[EoF]
