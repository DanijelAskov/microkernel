#include "keyevent.h"

#include <iostream.h>
#include <dos.h>

#include "bounded.h"
#include "intLock.h"
#include "user.h"
#include "event.h"
#include "ivtentry.h"

PREPAREENTRY(9,0);
  
char keymap[128] = { 
  0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8 , 9, 
 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 13, 0 , 'a', 's',
 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 0, 0, '`', '\\', 'z', 'x', 'c', 'v', 'b',
 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

KeyboardEvent::KeyboardEvent(BoundedBuffer* bb) : Thread(), myBuffer(bb) 
{
	theEnd = 0;
}

void KeyboardEvent::run()
{
	intLock
	Event event9(9);
	cout << "KeyebordListener started!" << endl;
	intUnlock
	char scancode, status; 
	
	while (!theEnd) {
      
		event9.wait();
		do{
			status = inportb(0x64);


			if (status & 0x01){
				scancode = inportb(0x60);
	
				if (scancode==-127){
					theEnd = 1;
					myBuffer->append('!');
				}else {
					if (scancode&0x80) {
						myBuffer->append(keymap[scancode&0x7F]);
					}
				}
            
			};

			asm{
				cli
				in      al, 61h
				or      al, 10000000b
				out     61h, al
				and     al, 01111111b 
				out     61h, al
				mov     al, 20h    
				out     20h, al
				sti
			}
       
		} while (!theEnd && status & 0x01);

	}
	
	intLock
		cout << endl << "KeyebordListener stopped!" << endl;
	intUnlock
}