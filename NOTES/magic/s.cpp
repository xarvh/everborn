/*
    3	Major Books:	4 spells
    6	MinorBooks:	5 spells

i,ii,iii,iv,v: starting %known on 1,2,3,4,5 level spell of minor books.
I,II,III,IV: starting %known on 1,2,3,4 level spell of major books.
Cast: spell casting cost bonus

		i   ii   iii   iv   v      I   II   III   IV		Cast
6books:
    6mi		K   80    70   60   50					30
    5mi+1ma	K   70    60   50   40     30  10			25
    4mi+2ma	K   60    50   40   30     50  30    10			20
    3mi+3ma	K   50    40   30   20     K   50    30   10		10

5books:
    5mi		K   60    50   40   30					25
    4mi+1ma	K   50    40   30   20     30  10  			20
    3mi+2ma	K   40    30   20   10     50  30    10			10

4books:
    4mi		K   40    30   20   	   10				20
    3mi+1ma	K   30    20   10	   30  10			10

3,2,1books:
    3mi		K   20    10						10
    2mi		K   10
    1mi		K
*/



int spellPercentualBonusToResearchOnSpellsInMinorBooks(
    int nmaj,	// number of major books of this realm possessed
    int nmin,	// number of minor books of this realm possessed
    int level)	// level of the spell (1-5 minor, 1-4 major)
{
 // one spell is always known
 if(level == 1) return 100;

 signed b = 80;
 b -= 10 * (6-nmin);		// remove 10 for any minor book less than 6
 b -= 10 * (6-nmin-nmaj);	// remove 10 for any book less than 6
 b -= 10 * (level-2);		// remove 10 for any level over ii

 // there you go
 return b < 0 ? 0 : b;
}



int spellPercentualBonusToResearchOnSpellsInMaiorBooks(
    int nmaj,	// number of major books of this realm possessed
    int nmin,	// number of minor books of this realm possessed
    int level)	// level of the spell (1-5 minor, 1-4 major)
{
 // known spells
 if(level == 1 && nmaj == 3 && nmin == 3) return 100;

 signed b = 70;
 b -= 20 * (3-nmaj);		// remove 10 for any minor book less than 3
 b -= 20 * (6-nmin-nmaj);	// remove 10 for any book less than 6
 b -= 20 * (level-1);		// remove 10 for any level over I

 // there you go
 return b < 0 ? 0 : b;
}


void spellC::ApplyStartingResearchBonus(int mi, int ma)
{
 if(!Book) return;

 int b;
 if(Book->Maior)
    b = spellPercentualBonusToResearchOnSpellsInMaiorBooks(ma, mi, Level);
 else
    b = spellPercentualBonusToResearchOnSpellsInMinorBooks(ma, mi, Level);

 ResearchLeft[Book->Pick->Owner->Id] -= (b * ResearchCost) / 100;
}




int spellPercentualBonusToCastingCost(int nmin)
{
 if(nmin == 6) return 30;
 if(nmin == 5) return 25;
 if(nmin == 4) return 20;
 if(nmin == 3) return 10;
 return 0;
}








//EOF =========================================================================