#ifndef _BOUNDED_BUFFER_
#define _BOUNDED_BUFFER_

#include "semaphor.h"

class BoundedBuffer {
public:
	BoundedBuffer (unsigned size);
	virtual ~BoundedBuffer ();
	int append (char);
	char take ();
	int fullCount(){ return itemAvailable.val(); };
private:
	unsigned Size;
	Semaphore mutexa, mutext;
	Semaphore spaceAvailable, itemAvailable;
	char* buffer;
	int head, tail;
};

#endif // _BOUNDED_BUFFER_