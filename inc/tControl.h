// File: tControl.h

#ifndef _TCONTROL_H_
#define _TCONTROL_H_

#include "utility.h"
#include "PCB.h"

// Nit koja se trenutno izvrsava
extern PCB* runningThread;

void dispatch(bool schedule); // thread safe

#endif  // _TCONTROL_H_
