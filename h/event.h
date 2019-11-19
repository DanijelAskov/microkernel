/*
 * Copyright (C) 2017  Danijel Askov
 *
 * This file is part of MicroKernel.
 *
 * MicroKernel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MicroKernel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _EVENT_H_
#define _EVENT_H_

#include "ivtentry.h"

typedef unsigned char IVTNo;

class KernelEv;

class Event {
public:
	Event(IVTNo ivtNum);
	~Event();
	void wait();
protected:
	friend class KernelEv;
	friend class IVTEntry;
	void signal();
private:
	KernelEv* myImpl;
	friend class Kernel;
};

#define PREPAREENTRY(numEntry, callOld)\
void interrupt inter##numEntry(...);\
IVTEntry newEntry##numEntry(numEntry, inter##numEntry);\
void interrupt inter##numEntry(...) {\
	newEntry##numEntry.signal();\
	if (callOld)\
		newEntry##numEntry.callOldIntRoutine();\
}

#endif //_EVENT_H_