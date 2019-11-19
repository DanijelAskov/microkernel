#ifndef _KEYBOARD_EVENT_
#define _KEYBOARD_EVENT_

#include <thread.h>

class BoundedBuffer;

class KeyboardEvent :public Thread{
public:
	KeyboardEvent (BoundedBuffer*);  
	virtual ~KeyboardEvent () {waitToComplete();}
	Thread* clone() const { return new KeyboardEvent(myBuffer); };
protected:
	virtual void run();
	char znak;
private:
	BoundedBuffer* myBuffer;
};

#endif // _KEYBOARD_EVENT_