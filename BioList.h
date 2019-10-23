 /*
 | Class definition of a Bio List
*/

#ifndef __BIOLIST__
#define __BIOLIST__

#include "Bio.h"

typedef struct sItem {
	Bio		*bio;
	sItem	*next;
} Item;

class BioList {
	private:
	Item	*head;
	Item	*tail;
	
	public:
	BioList() { head = 0; tail = 0; }
	~BioList();
	
	void add( Bio *bio );
	void traverse( int (*doMe)( Bio *thisBio, void *doMeArg ), void *doMeArg );
	Bio *getFirst();
	int	remove( Bio *bio );
};

#endif