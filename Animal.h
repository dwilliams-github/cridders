/*
 | Definition of class "Animal"
*/

#ifndef __ANIMAL__
#define __ANIMAL__

#include "land.h"

typedef struct {
	short sum, there;
} GeneAnimal;

typedef struct {
	short strength;
	short gHere, gSum, gThere;
	short aSum, aThere;
	GeneAnimal	animal[3];
} GeneBrain;

typedef struct {
	unsigned char	speed;
	unsigned char   fill;
	short	defenseStrength;
	short	attackStrength;
	short 	childStrength;
	
	unsigned char appearance[7];
	unsigned char animal1[7],
				  animal2[7];
	GeneBrain	brain[4];
} Genes;

typedef enum {
	ERROR, BORN, STAY, MOVE, REPRODUCE, ATTACK_WIN, ATTACK_LOSE
} Action;

class Animal : public Bio {
	public:
	Animal( Land *toLive, int xLive, int yLive, short strength, Genes *parGenes );
	
	void AddIcon( Icon &icon );
	virtual short MatchIcon( const unsigned char mask[7] ) const;
	
	virtual unsigned int DoTurn() const;

	protected:
	Land	*land;
	
	short 	strength;
	short	life;
	
	Genes	genes;
	Action	lastAction;
	
	unsigned short appMask[7];
};

#endif