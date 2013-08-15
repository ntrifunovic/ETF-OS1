//===========================================================================//
//   Project: Projekat iz Operativnih sistema 1
//   File:    user.cpp
//   Date:    June 2012
//===========================================================================//
#include <iostream.h>
#include <stdlib.h>

#include "keyevent.h"
#include "bounded.h"
#include "user.h"
#include "intLock.h"
#include <event.h>

//---------------------------------------------------------------------------//
//  Ovo se menja u testu
//---------------------------------------------------------------------------//
int semPreempt = 0;

Time TIME_SLICE = 2;   // 0 ili defaultTimeSlice

int N = 3;            // 1 <= N <= 19

//---------------------------------------------------------------------------//

int theEnd=0;

class Producer : public Thread {
public:
	Producer (BoundedBuffer* bb, TName name, char y, Time time_slice);
	~Producer() {waitToComplete();}
	static int num;
protected:
	virtual void run ();
	char produce() {return x;}; // Produce an item

private:
	BoundedBuffer* myBuffer;
	char x;
};

TName consumerName = "consumer";

//---------------------------------------------------------------------------//
class Consumer : public Thread {
public:
	Consumer (BoundedBuffer* bb) : Thread(consumerName, defaultStackSize, 0), myBuffer(bb) {}
	~Consumer() {waitToComplete();}

protected:
	virtual void run ();
	void consume(char p); // Consume an item

private:
	BoundedBuffer* myBuffer;
};




//---------------------------------------------------------------------------//
// Korisnicki program mora obavezno da definise ovu f-ju
//---------------------------------------------------------------------------//
void tick(){

}

//---------------------------------------------------------------------------//

int Producer::num=0;

Producer::Producer (BoundedBuffer* bb, TName name, char y, Time time_slice) : Thread(name, defaultStackSize, time_slice), 
																						myBuffer(bb), x(y) {
																									//broj aktivnih proizvodjaca
																									Producer::num++;
																						}
		
void Producer::run () {
	int i;
	while(!theEnd) {
		char d = produce();
		myBuffer->append(d);
		for(i=0;i<1000;i++);
	}
	Producer::num--;
}


//---------------------------------------------------------------------------//


void Consumer::consume(char p) {
	intLock
	cout<<p<<" ";
	intUnlock
} // Consume an item

void Consumer::run () {
		
	int i = 0, j;
	while(!theEnd) {
		char d = myBuffer->take();
		consume(d);
		if (i++ == 40) {
			Thread::sleep(92);
			i = 0;
		}else for(j=0;j<200;j++);
	}
   
	while (myBuffer->fullCount()||Producer::num){
		char d = myBuffer->take();
		consume(d);
		dispatch();
	}
	intLock
	cout<<endl<<"Happy End"<<endl;
	intUnlock
}





int userMain (int argc, char* argv[])
{
	TName producerName = "producerx";
	BoundedBuffer *buff;
	Consumer *con;
	intLock
	buff = new BoundedBuffer(atoi(argv[1]));
	N = atoi(argv[2]);
	N = N>19 ? 19 : N;
	TIME_SLICE = atoi(argv[3]);
	Producer **pro = new Producer*[N];
	KeyboardEvent* kev;
	int i;
	
	con = new Consumer(buff);
	  
	con->start();

	for (i=0; i<N; i++){
		producerName[8]='0'+i;
		pro[i] = new Producer(buff, producerName ,'0'+i, TIME_SLICE);
		pro[i]->start();
	}
  
  	kev = new KeyboardEvent(buff);
	intUnlock
	
	kev->start();
	
	for (i=0; i<N; i++){
		delete pro[i];
	}  
	delete [] pro;
	delete kev;
	delete con;

	intLock
	cout<<"Kraj userMain"<<endl;
	intUnlock
  
	
  return 0;
}
//---------------------------------------------------------------------------//