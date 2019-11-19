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

#ifndef _THREAD_H_
#define _THREAD_H_

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;

typedef unsigned int Time;
const Time defaultTimeSlice = 2;

typedef int ID;

typedef void (*SignalHandler)();

typedef unsigned SignalId;

class PCB;

class Thread {
public:
	void start();
	void waitToComplete();
	virtual ~Thread();
	ID getId();
	static ID getRunningId();
	static Thread *getThreadById(ID id);

	void signal(SignalId signal);
	void registerHandler(SignalId signal, SignalHandler handler);
	SignalHandler getHandler(SignalId signal);
	void maskSignal(SignalId signal);
	static void maskSignalGlobally(SignalId signal);
	void unmaskSignal(SignalId signal);
	static void unmaskSignalGlobally(SignalId signal);
	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);
	static void pause();
protected:
	friend class PCB;
	friend class Kernel;
	Thread(StackSize stackSize = defaultStackSize, Time timeSlice =
			defaultTimeSlice);
	virtual void run() {
	}
private:
	PCB *myPCB;
};

void dispatch();

#endif // _THREAD_H_