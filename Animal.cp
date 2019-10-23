/*
 | Implementation of "Animal" class
 |
 | (c) Copyright 1994  David C. Williams
 | All rights reserved
*/

#include "Animal.h"
#include <limits.h>

/*
 | Animal (Creater): make an animal
*/
Animal::Animal( Land *toLive, int xLive, int yLive, short strStart, Genes *parGenes ) 
			: Bio( xLive, yLive )
{
	short i;

	land = toLive;
	
	/* Copy genes and validate */
	
	genes = *parGenes;
	
	genes.speed &= 0xf;
	if (genes.speed == 0) genes.speed = 1;
	
	life = 20;
	strength = strStart;
	
	lastAction = BORN;
	
	for( i = 0; i < 7; i++ )
		appMask[i] = (genes.appearance[i]&0x7f)<<4;
	
	land->addBio( this, genes.speed&0xF );
}

/*
 | AddIcon: add animal image to land icon
*/
void Animal::AddIcon( Icon &icon )
{
	unsigned short taskMask;
	unsigned short strengthMeter[8] = { 0x0800, 0x0C00, 0x0E00, 0x0F00,
	                                    0x0F80, 0x0FC0, 0x0FE0, 0x0FF0  };
	
	if (strength > 0)
		icon[3] |= strengthMeter[ strength >> 12 ];                                    
	                                    
	icon[5]  |= appMask[0];
	icon[6]  |= appMask[1];
	icon[7]  |= appMask[2];
	icon[8]  |= appMask[3];
	icon[9]  |= appMask[4];
	icon[10] |= appMask[5];
	icon[11] |= appMask[6];
	
	switch(lastAction) {
		case STAY:  		taskMask = 0x00f0; break;
		case MOVE:			taskMask = 0x0f00; break;
		case ATTACK_LOSE:	taskMask = 0x08f0; break;
		case ATTACK_WIN:	taskMask = 0x0f10; break;
		case REPRODUCE:		taskMask = 0x0810; break;
		case BORN:			taskMask = 0x0180; break;
		case ERROR:
		default:			taskMask = 0x0ff0;
	}
	icon[14] |= taskMask;
}	

/*
 | matchIcon
 | Return degree of agreement (0-49) between animal image and mask
*/
short Animal::MatchIcon( const unsigned char mask[7] ) const
{
	static unsigned char nBitChar[0xFF];
	static int notInitialized = 1;
	char result;
	
	typedef union {
		unsigned long byLong[2];
		unsigned char byChar[8];
	} MaskComp;
	
	MaskComp *mask4 = (MaskComp *)mask,
		     *app4  = (MaskComp *)genes.appearance,
		     eor4;
	
	if (notInitialized) {
		for( int i = 0; i < 0x7F; i++ ) {
			unsigned short toRotate = i, n = 0;
			while(toRotate) {
				if (toRotate&0x1) n++;
				toRotate >>= 1;
			}
			nBitChar[i] = nBitChar[i+0x80] = n;
		}
		notInitialized = 0;
	}
	
	eor4.byLong[0] = mask4->byLong[0]^app4->byLong[0];
	eor4.byLong[1] = mask4->byLong[1]^app4->byLong[1];
	
	result = 49;
	
	result -= nBitChar[ eor4.byChar[0] ];
	result -= nBitChar[ eor4.byChar[1] ];
	result -= nBitChar[ eor4.byChar[2] ];
	result -= nBitChar[ eor4.byChar[3] ];
	result -= nBitChar[ eor4.byChar[4] ];
	result -= nBitChar[ eor4.byChar[5] ];
	result -= nBitChar[ eor4.byChar[6] ];
	
	return(result);
}


/*
 | DoTurn: do something!
*/
// pascal short Random(void) = 0xA861;	// Mac Toolbox routine!! Should replace.

static void Mutate( Genes *genes )
{
	unsigned char *fibbo = (unsigned char *)genes,
			      *fibboDone = fibbo + sizeof(Genes);
	long totalStrength;
	
	/* Randomly set bits */
	
	for(;;) {
		unsigned short skip = Random()>>5;
		fibbo += skip >> 4;
		if (fibbo >= fibboDone) break;
		*fibbo ^= (1<<(skip&0xf));
	}
	
	/* Normalize strengths */
	
	if (genes->defenseStrength < 0) genes->defenseStrength = -genes->defenseStrength;
	if (genes->attackStrength  < 0) genes->attackStrength  = -genes->attackStrength;
	if (genes->childStrength   < 0) genes->childStrength   = -genes->childStrength;
	
	totalStrength = genes->defenseStrength + genes->attackStrength + genes->childStrength;
	if (totalStrength == 0) 
		genes->childStrength = 1000;
	else {
		genes->defenseStrength = 1000*(long)genes->defenseStrength/totalStrength;
		genes->attackStrength  = 1000*(long)genes->attackStrength/totalStrength;
		genes->childStrength   = 1000*(long)genes->childStrength/totalStrength;
	}
}
		

typedef struct {
	short growth;
	short animal;
	short match[3];
} NeighborInfo;

typedef struct {
	unsigned short 	inValid,
					inValidGo;
	short			oppAnimal;
	short			strength;
	NeighborInfo	neighbors[9];
} CalcInfo;


static long CalcGeneAnimal( CalcInfo *info, GeneBrain *brain, short *where )
{
	NeighborInfo	*thisNeighbor;
	short		mask, i;
	long		sum, hMax, here; 
	
	sum = 0;
	
	/* Loop over valid neighbors */
	
	*where = -1;
	for(i=0, mask=1; i < 9; i++, mask <<= 1 ) {
		if (info->inValid&mask) continue;
		
		/* Keep track of summed quantities */
		
		thisNeighbor = info->neighbors + i;
		
		sum += brain->gSum*thisNeighbor->growth + brain->aSum*thisNeighbor->animal;
		
		sum += brain->animal[0].sum*thisNeighbor->match[0];
		sum += brain->animal[1].sum*thisNeighbor->match[1];
		sum += brain->animal[2].sum*thisNeighbor->match[2];
		
		/* How about direction specific quantities? */
		
		if (info->inValidGo&mask) continue;
		
		here = brain->gThere*thisNeighbor->growth;
		
		if (info->oppAnimal) thisNeighbor = info->neighbors + 8 - i;
		if (thisNeighbor->animal) {
			here += brain->aThere;
			here += brain->animal[0].there*thisNeighbor->match[0];
			here += brain->animal[1].there*thisNeighbor->match[1];
			here += brain->animal[2].there*thisNeighbor->match[2];
		}
		
		if (*where == -1 || here > hMax) {
			hMax = here;
			*where = i;
		}
	}
	
	/* Normalize a bit by dividing sum by 8, and then add strength and local terms */
	
	sum = (sum>>3) + brain->strength*(info->strength>>4) + brain->gHere*info->neighbors[4].growth;
	
	/* Put everything together */
	
	if (*where != -1) sum += hMax;
	
	return(sum);
}

unsigned int Animal::DoTurn() const
{
	static short xAround[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1  };
	static short yAround[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1  };
	
	LandSpot		area[9], *here;
	short			inValidGo, inValidEat;
	CalcInfo		info;
	NeighborInfo	*thisNeighbor;
	long			wStay, wGo, wRep, wEat, largest, newStrength;
	short			goWhere, repWhere, eatWhere, i, mask;
	Action			doAction;
	
	/* Are we out of turns? */
	
	if (--life == 0) return 0;
	
	/* 
	 | What should our animal do? 
	 | To help decide, get information on our neighbors	
	*/
	
	land->localInfo( xLand, yLand, area, &info.inValid );
	
	/* Do our neighbors look like anything we recognize? */
	
	inValidGo = inValidEat = info.inValid;
	
	here = area + 8;
	thisNeighbor = info.neighbors + 8;
	mask = 1<<8;
	do {
		if (info.inValid&mask) continue;
		
		thisNeighbor->growth = here->plant;
		if (here->bio) {
			inValidGo |= mask;
			thisNeighbor->animal = 1;
			thisNeighbor->match[0] = here->bio->MatchIcon( genes.appearance );
			thisNeighbor->match[1] = here->bio->MatchIcon( genes.animal1 );
			thisNeighbor->match[2] = here->bio->MatchIcon( genes.animal2 );
		}
		else {
			inValidEat |= mask;
			thisNeighbor->animal = 0;
			thisNeighbor->match[0] = 0;
			thisNeighbor->match[1] = 0;
			thisNeighbor->match[2] = 0;
		}
	} while( mask >>= 1, thisNeighbor--, here-- > area );
	
	/* How badly do we want to stay? */
	
	info.strength = strength;
	info.inValidGo = 0xffff;
	wStay = CalcGeneAnimal( &info, &genes.brain[0], &i );
	
	/* How badly do we want to move? (and do we have enough strength?) */
	
	if (strength >= 50) {
		info.inValidGo = inValidGo;
		info.oppAnimal = 1;
		wGo = CalcGeneAnimal( &info, &genes.brain[1], &goWhere );
	}
	else
		goWhere = -1;
	
	/* How badly do we want to reproduce? (and do we have enough strength?) */
	
	if (strength >= genes.childStrength) 
		wRep = CalcGeneAnimal( &info, &genes.brain[2], &repWhere );
	else
		repWhere = -1;
			
	/* How badly do we want to eat someone */
	
	info.inValidGo = inValidEat;
	info.oppAnimal = 0;
	wEat = CalcGeneAnimal( &info, &genes.brain[3], &eatWhere );
	
	/* What wins? */
	
	largest = wStay;
	doAction = STAY;
	if (repWhere != -1 && wRep > largest) { largest = wRep; doAction = REPRODUCE;  }
	if (goWhere  != -1 && wGo  > largest) { largest = wGo;  doAction = MOVE;       }
	if (eatWhere != -1 && wEat > largest) { largest = wEat; doAction = ATTACK_WIN; }

	newStrength = strength;
	if (doAction == REPRODUCE) {
		unsigned short xNew = xLand+xAround[repWhere],
					   yNew = yLand+yAround[repWhere],
					   xWas = xLand,
					   yWas = yLand;
	
		/* Have a baby! */
	
		lastAction = REPRODUCE;
		if (land->moveBio( xLand, yLand, xNew, yNew )) {
			Genes newGenes = genes;
			Mutate(&newGenes);
			new Animal( land, xWas, yWas, genes.childStrength, &newGenes );
			newStrength -= genes.childStrength;
		}
		else {
			lastAction = ERROR;
			land->refreshOne( xLand, yLand );
		}
	}
	else if (doAction == MOVE) {
		unsigned short xNew = xLand+xAround[goWhere],
					   yNew = yLand+yAround[goWhere];
	
		/* Go west, young cridder! */
	
		lastAction = MOVE;
		if (land->moveBio( xLand, yLand, xNew, yNew )) 
			newStrength -= 50;
		else {
			lastAction = ERROR;
			land->refreshOne( xLand, yLand );
		}
	}
	else if (doAction == ATTACK_WIN) {
		unsigned short xNew = xLand+xAround[eatWhere],
					   yNew = yLand+yAround[eatWhere];
		Animal	*target = (Animal *)area[eatWhere].bio;
		short	targetDefense, attackPower;
	
		/* Attack!! */
		
		targetDefense = target->genes.defenseStrength;
		if (targetDefense > target->strength) targetDefense = target->strength;
		attackPower = genes.attackStrength;
		if (attackPower > strength) attackPower = strength;
		
		if (attackPower > (targetDefense<<1)) {
			lastAction = ATTACK_WIN;
			newStrength += target->strength;
			land->condemnBio( xNew, yNew );
			land->moveBio( xLand, yLand, xNew, yNew );
		}
		else {
			newStrength -= attackPower;
			lastAction = ATTACK_LOSE;
			land->refreshOne( xLand, yLand );
		}
	}
	else {
	
		/* Stay where we are, and pig out */
		
		newStrength += land->crop( xLand, yLand, 200 );
		if (newStrength < 20) return(0);
		lastAction = STAY;
		land->refreshOne( xLand, yLand );
	}
	
	newStrength -= 10;
	strength = (newStrength > SHRT_MAX) ? SHRT_MAX : newStrength;
	          
	
	return(genes.speed);
}