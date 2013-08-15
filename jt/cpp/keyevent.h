//===========================================================================//
//   Project: Projekat iz Operativnih sistema 1
//   File:    keyevent.h
//   Date:    June 2012
//===========================================================================//

#ifndef _OS1_KEYBOARD_EVENT_
#define _OS1_KEYBOARD_EVENT_

#include <thread.h>

class BoundedBuffer;



class KeyboardEvent :public Thread{
public:

	KeyboardEvent (BoundedBuffer*);  
	~KeyboardEvent () { waitToComplete(); }
protected:

	virtual void run();

	char znak;

private:
  
	BoundedBuffer* myBuffer;
};




#endif  // _OS1_KEYBOARD_EVENT_