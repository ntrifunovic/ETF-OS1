//===========================================================================//
//   Project: Projekat iz Operativnih sistema 1
//   File:    keyevent.cpp
//   Date:    June 2012
//===========================================================================//
#include <iostream.h>
#include <dos.h>

#include "keyevent.h"
#include "bounded.h"
#include "user.h"
#include "intLock.h"
#include <event.h>

PREPAREENTRY(9,0);

char keName[] = "keyevent";  
  
//---------------------------------------------------------------------------//
//  Podeseno za qwerty tastature
//  Tabela nije potpuna
//---------------------------------------------------------------------------//
char keymap[128] = { 
  0 , 27,'1','2','3','4','5','6','7','8','9','0','-','=', 8 , 9, 
 'q','w','e','r','t','y','u','i','o','p','[',']', 13, 0 ,'a','s',
 'd','f','g','h','j','k','l',';',0,0,'`','\\','z','x','c','v','b',
 'n','m',',','.','/', 0 ,'*', 0 ,' '
};





//---------------------------------------------------------------------------//
KeyboardEvent::KeyboardEvent(BoundedBuffer* bb) : Thread(keName), myBuffer(bb) 
{
	theEnd = 0;
}



void KeyboardEvent::run()
{
	intLock
	Event event9(9);
	intUnlock
	char scancode, status, znak; 

	//dohvati Consumer Thread
	Thread *tCon = Thread::getThreadById(Thread::getIdOf("consumer"));
	
	intLock
	cout<<"consumer == "<<tCon->getName()<<endl;
	intUnlock
	
	
	while (!theEnd) {
      
		event9.wait();
		status = inportb(0x64); // ocitava statusni reg. sa 64h

		while (status & 0x01){ //dok se ocitava takav status da je pritisnut neki taster
      
      //scancode = inportb(0x60);
			if (status & 0x01){           // Can I read?
				scancode = inportb(0x60);
			};
       //////////////////////
			asm{
				cli
				in      al, 61h         //; Send acknowledgment without
				or      al, 10000000b   //;   modifying the other bits.
				out     61h, al         //;
				and     al, 01111111b   //;
				out     61h, al         //;
				mov     al, 20h         //; Send End-of-Interrupt signal
				out     20h, al         //;
				sti
			}
       
       //////////////////////
			if (status & 0x01){
				// press ESC za theEnd
				if (scancode==1){
					theEnd = 1;
					tCon->wakeUp();
        }else {
					if (scancode&0x80) {
						tCon->wakeUp();
						myBuffer->append(keymap[scancode&0x7F]);
					};
				};
            
			};
			
			status = inportb(0x64);
		}//while

	}// while
}
