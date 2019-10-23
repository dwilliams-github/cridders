/*
 | Implement class "land"
 |
 | (c) Copyright 1994  David C. Williams
 | All rights reserved
*/

#include "land.h"

/*
 | land (Creator): made a new land. 
 | Rather than worry about implementation specific methods of memory allocation, 
 | we simply ask the caller to supply the memory.
*/
Land::Land( int w, int h )
{
	LandSpot *last;

	width = w;
	height = h;
	
	/* Get some workspace */
	
	ground = new LandSpot[w*h];

	/* Initialize the land */
	
	last = ground + w*h;
	while (--last >= ground) {
		last->plant = 0x100;
		last->bio   = 0;
		
		last->growthRate = 5;
		last->mutateRate = 0;
		last->moveCost = 1;
	}
	
	nAnimal = 0;
	
	/* Build offset table */
	
	static short 	xAround[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1  };
	static short 	yAround[9] = {-1, -1, -1, 0, 0, 0, 1, 1, 1  };
	for( int i=0; i<9; i++ )
		offsets[i] = xAround[i] + yAround[i]*width;
	
	/* Initialize our turn array of bio lists */
	
	turns = new BioList[16];
	currTurn = 0;
}

/*
 | ~Land (destructor)
 | Besides deleting land, we need to delete the workspace.
 | Also note that "delete [expression]" is obsolete - we use the modern version here.
*/
Land::~Land()
{
	delete [] ground;
}


/*
 | setBarrier
 | Change the specified area of land into a barrier 
*/
void Land::setBarrier( int x, int y ) 
{
	LandSpot 		*here = ground + width*y + x;
	
	here->moveCost = 0;
	refreshOne( x, y );
}

/*
 | toggleBarrier
 | Change the specified area of land into or out of a barrier 
*/
void Land::toggleBarrier( int x, int y ) 
{
	LandSpot 		*here = ground + width*y + x;
	
	here->moveCost = here->moveCost = 0 ? 1 : 0;
	refreshOne( x, y );
}

/*
 | setLand
 | Change the specified area of land into the specified characteristics
*/
void Land::setLand( int x, int y )
{
	LandSpot 		*here = ground + width*y + x;
	
	here->moveCost = 1;
	refreshOne( x, y );
}


/*
 | getSpot
 | Return land spot given coordinates
*/
LandSpot *Land::getSpot( int x, int y )
{
	return( ground + width*y + x );
}


/*
 | refresh: draw one square of the land, including its occupants.
 | This inherited class does nothing! Override to take care of displays.
*/
void Land::refreshOne( int x, int y ) 
{
}


/*
 | refreshPlant: update just the grass in one square.
 | This inherited class does nothing! Override to take care of displays.
*/
void Land::refreshPlant( int x, int y, int was, int is ) 
{
}


/*
 | localInfo
 | Return information about the immediate area
*/
void Land::localInfo( int x, int y, LandSpot area[9], unsigned short *inValid )
{
	LandSpot 		*here = ground + width*y + x;
	unsigned short 	i, mask;

	if (x == 0) *inValid = 0x049; else *inValid = 0;
	
	if (x >= width-1) *inValid |= 0x124;

	if (y == 0) *inValid |= 0x007;
	
	if (y >= height-1) *inValid |= 0x1C0;

	for( i = 0, mask=1; i < 9; i++, mask <<= 1 ) {
		if (!((*inValid)&mask)) {
			LandSpot *there = here + offsets[i];
			area[i] = *there;
			if (there->moveCost == 0) *inValid |= mask;
		}
	}
}			

short Land::crop( int x, int y, short eat )
{
	LandSpot *here = ground + width*y + x;

	if (eat > here->plant) {
		eat = here->plant;
		here->plant = 0;
	}
	else
		here->plant -= eat;
	
	return(eat);
}

/*
 | addBio: add a new biological element
*/
int Land::addBio( Bio *aBio, unsigned int turnAhead )
{
	LandSpot *here = ground + width*aBio->YLand() + aBio->XLand();
	BioList	 *turn = turns + ((turnAhead+currTurn) & 0xF);
	
	if (here->bio) return(0);
	
	turn->add( aBio );
	nAnimal++;
	
	here->bio = aBio;
	here->biosTurn = turn;

	refreshOne( aBio->XLand(), aBio->YLand() );
	return(1);
}

/*
 | moveBio: move the same biological element from one spot to another
*/
int Land::moveBio( int x, int y, int newX, int newY )
{
	LandSpot *oldSpot = ground + width*y + x,
			 *newSpot = ground + width*newY + newX;
			 
	if (newX >= width || newY >= height || newX < 0 || newY < 0)
		return 0;
			 
	if (newSpot == oldSpot) return(1);
	if (newSpot->bio) return(0);
	
	if (oldSpot->bio->XLand() != x || oldSpot->bio->YLand() != y) 
		return(0);
			 
	newSpot->bio = oldSpot->bio;
	newSpot->biosTurn = oldSpot->biosTurn;
	newSpot->bio->Move( newX, newY );
	refreshOne( newX, newY );
	
	oldSpot->bio = 0;
	refreshOne( x, y );
	
	return(1);
}

/*
 | getBio: return bio in this square
*/
Bio *Land::getBio( int x, int y )
{
	LandSpot *here = ground + width*y + x;

	return(here->bio);
}

/*
 | removeBio: remove a biological element from a spot
 | Do not use while traversing the same list this bio is on!!
*/
int Land::removeBio( int x, int y )
{
	LandSpot *here = ground + width*y + x;
	
	if (here->bio == 0)
		return(0);
	if (here->bio->XLand() != x || here->bio->YLand() != y) 
		return(0);
	
	if (here->biosTurn->remove(here->bio) == 0)
		return(0);
		
	here->bio = 0;
	nAnimal--;
	refreshOne( x, y );
	return(1);
}

/*
 | condemnBio: mark a bio for death at its next turn.
 | This routine is safe while traversing its turn list.
*/
int Land::condemnBio( int x, int y )
{
	LandSpot *here = ground + width*y + x;
	
	if (here->bio == 0)
		return(0);
	if (here->bio->XLand() != x || here->bio->YLand() != y) 
		return(0);
		
	here->bio->Kill();
	here->bio = 0;
	nAnimal--;
	refreshOne( x, y );
	return(1);
}


/*
 | procOne
 | Process the turn of a current bio
*/
int Land::procOne( Bio *thisBio )
{
	BioList 		*nextTurn;
	unsigned int    turnAhead;
	LandSpot 		*here;
			
	/* Do this bio's turn. First: is the bio on death row? */
	
	if (thisBio->Alive()) {
		delete thisBio;
		return(1);
	}
	
	/* Nope - so where is it? */
	
	here = ground + width*thisBio->YLand() + thisBio->XLand();
	if (thisBio != here->bio) 
		return(0);				// should never happen
	
	/* Still kicking? So what do you want to do? */
	
	turnAhead = thisBio->DoTurn();
	if (turnAhead == 0) {
		here->bio = 0;
		nAnimal--;
		refreshOne( thisBio->XLand(), thisBio->YLand() );
		delete thisBio;
		return(1);
	}
	if (turnAhead < 1) 
		turnAhead = 1;
	else if (turnAhead > 15) 
		turnAhead = 15;
	
	/* A survivor: add to the next turn */
	
	nextTurn = turns + ( (currTurn+turnAhead)&0xf);
	nextTurn->add( thisBio );
	here->biosTurn = nextTurn;
	
	return(1);
}

/*
 | processTurn: process bios on the current turn
*/
void Land::processTurn()
{
	BioList		*now = turns + (currTurn&0xf);
	LandSpot 	*here = ground;
	int 		x = 0, y = 0;
	Bio			*next;
	
	/* Next bio, please */
	
	next = now->getFirst();
	if (next) 
		procOne( next );
	else {
	
		/* 
		 | No more bios? Perform end of turn tasks:
         | Adjust plant growths and touch-up growth chart, if necessary 
        */
	
		do {
			if (here->moveCost) {
				int is, was = (here->plant>>8);
				
				if (here->plant > (0xB00>>1))
					here->plant += ( 0xB00 - here->plant + (1<<5)-1 ) >> 5;
				else
					here->plant += (here->plant >> here->growthRate) + 1;
				
				is = (here->plant)>>8;
				while(was++ < is) refreshPlant( x, y, was, is );
			}
			if (++x == width) { x = 0; y++; }
		} while( here++, y < height );
		
		/* Increment turn counter */
		
		currTurn++;
	}
}
