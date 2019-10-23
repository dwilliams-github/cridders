/*
 | Definition of base class Bio
*/

#ifndef __BIO__
#define __BIO__

typedef unsigned short Icon[16];

class Bio {
	public:
	
	Bio( unsigned short x, unsigned short y ) {
		xLand = x; yLand = y; alive = true;
	}
	virtual ~Bio() {return;}

	virtual void Move( unsigned short x, unsigned short y ) {
		xLand = x; yLand = y;
	}

	virtual void AddIcon( Icon &icon ) {
		// Do nothing!
	}
	
	virtual short MatchIcon( const unsigned char compare[7] ) const = 0;
	
	virtual unsigned int DoTurn(  ) const = 0;
	
	//
	// Access methods
	//
	virtual inline unsigned short XLand() const { return xLand; }
	virtual inline unsigned short YLand() const { return yLand; }
	virtual inline unsigned bool Alive() const { return alive; }
	
	virtual inline void Kill() { alive = false; }
	
	protected:
	unsigned short xLand, yLand;
	bool	alive;
};

#endif