/*
 | Implement BioList class
 |
 | (c) Copyright 1994  David C. Williams
 | All rights reserved
*/
#include "BioList.h"

/*
 | Add.
 | Add a new entry to the end of the list.
*/
void BioList::add( Bio *bio )
{
	Item	*thisItem = new Item;
	
	thisItem->bio = bio;
	thisItem->next = 0;
	
	if (tail) 
		tail->next = thisItem;
	else
		head = thisItem;
		
 	tail = thisItem;
}

/*
 | ~BioList (destructor)
 | Delete the list
*/
BioList::~BioList()
{
	Item	*here, *toDie;
	
	here = head;
	while(here) {
		toDie = here;
		here = here->next;
		delete toDie;
	}
}

/*
 | Traverse.
 | Traverse the list and optionally delete the items along the way.
 | Return value of "doMe":
 |               0) do nothing
 |               1) delete the item
 | Please note that the traverse function must not attempt to add or remove items
 | from the list, except the current item (by returning 1).
*/
void BioList::traverse( int (*doMe)( Bio *thisBio, void *doMeArg ), void *doMeArg )
{
	Item	*here, *prev, *toNuke;
	
	prev = 0;
	here = head;
	
	while (here) {
		if ((*doMe)( here->bio, doMeArg ) == 1) {
			if (prev) {
				prev->next = here->next;
				if (prev->next == 0) tail = prev;
			}
			else {
				head = here->next;
				if (head == 0) tail = 0;
			}
			
			toNuke = here;
			here = here->next;
			
			delete toNuke;
		}
		else {
			prev = here;
			here = here->next;
		}
	}
}

/*
 | getFirst
 | Remove first bio in list and return it.
 | Returns zero if the list is empty
*/
Bio *BioList::getFirst()
{
	Item *here;
	Bio	 *thisBio;
	
	here = head;
	if (here) {
		head = here->next;
		if (!head) tail = 0;
		thisBio = here->bio;
		delete here;
		return(thisBio);
	}
	else
		return(0);
}

/*
 | remove.
 | Remove a bio from the list. Returns zero if the bio isn't on the list.
*/
int BioList::remove( Bio *thisBio )
{
	Item	*here, *prev;
	
	prev = 0;
	here = head;
	
	while (here) {
		if (here->bio == thisBio) {
			if (prev) {
				prev->next = here->next;
				if (prev->next == 0) tail = prev;
			}
			else {
				head = here->next;
				if (head == 0) tail = 0;
			}
				
			delete here;
			return(1);
		}
		else {
			prev = here;
			here = here->next;
		}
	}
	return(0);
}	