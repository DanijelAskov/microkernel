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

#ifndef _PCB_H_
#define _PCB_H_

#include "list.h"
#include "thread.h"

const unsigned signalCount = 16;

class PCB {
public:
	typedef enum State {
		NEW, READY, BLOCKED, TERMINATED
	};

	typedef enum SignalState {
		DEFAULT = 0, MASKED = 1, BLOCKED_ = 2
	};
private:
	static ID idCounter;
	static LinkedList<PCB> userThreads;

	Thread *myThread;
	unsigned *stack;
	unsigned ss, sp, bp;
	StackSize stackSize;
	Time timeSlice, timeRemaining, maxTimeToWait;
	ID id;
	State state;
	LinkedList<PCB> waiting;

	struct Signal {
		SignalId signalId;
		Signal(SignalId signalId) :
				signalId(signalId) {
		}
	};

	SignalHandler handlers[signalCount];
	int signalsState[signalCount];
	LinkedList<Signal> signalsToHandle;
	static int globalSignalsState[signalCount];
	static LinkedList<PCB> signalWaitingThreads;
	int deblock;
	int signalsHandled;

	Thread *parentThread;

	void createStack();
public:
	PCB(Thread *myThread, StackSize stackSize, Time timeSlice);
	~PCB();
	void start();
	int waitToComplete();
	static Thread *getThreadById(ID id);

	void signal(SignalId signal);
	void registerHandler(SignalId signal, SignalHandler handler);
	SignalHandler getHandler(SignalId signal);
	void maskSignal(SignalId signal);
	static void maskSignalGlobally(SignalId singal);
	void unmaskSignal(SignalId signal);
	static void unmaskSignalGlobally(SignalId signal);
	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);
	static void pause();

	static void updatePausedThreads();

	friend class Kernel;
	friend class KernelSem;
	friend class Semaphore;
	friend class KernelEv;
	friend class Thread;
	friend void dispatch();
};

#endif // _PCB_H_