/*
 | Class definition for "Land"
 | Also defines base class "Bio"
 |
*/
#ifndef __LAND__
#define __LAND__

#include "Bio.h"
#include "BioList.h"

typedef struct {
	short 	plant;					// plant growth: ranging from 0 to B00
	unsigned char	growthRate;		// growth rate
	unsigned char	mutateRate;		// mutation rate
	short	moveCost;				// cost of movement, or a barrier if zero
	Bio    	*bio;
	BioList	*biosTurn;
} LandSpot;

class Land {
   public:
	int			width, 
				height;      	// Size of land
	long		nAnimal;		// Number of animals
	unsigned long	currTurn;
	
   private:
	LandSpot 	*ground;		// The land itself
	
	BioList			*turns;
	
	short		offsets[9];
	
   public:
	Land( int w, int h );
	~Land();
	
	void setBarrier( int x, int y );
	void toggleBarrier( int x, int y );
	void setLand( int x, int y );
	LandSpot	*getSpot( int x, int y );
	
	virtual void refreshOne( int x, int y );
	virtual void refreshPlant( int x, int y, int was, int isNow );
	
	int addBio( Bio *aBio, unsigned int turnAhead );
	int moveBio( int x, int y, int newX, int newY );
	int removeBio( int x, int y );
	int condemnBio( int x, int y );
	Bio *getBio( int x, int y );
	
	void localInfo( int x, int y, LandSpot area[9], unsigned short *inValid );
	short crop( int x, int y, short eat );
	
	void processTurn();
	
   private:
	int procOne( Bio *thisBio );
};

#endif