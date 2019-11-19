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

#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "thread.h"

#define SYS_CALL_ENTRY 60h
const unsigned sysCallEntry = 0x60;

class Semaphore;

class Event;

typedef unsigned char IVTNo;

struct SysData {

	typedef enum Name {
		THREAD_CREATE,
		THREAD_START,
		THREAD_DESTROY,
		THREAD_WAIT,
		THREAD_GET,
		DISPATCH,
		THREAD_TERMINATE,
		SEM_CREATE,
		SEM_DESTROY,
		SEM_WAIT,
		SEM_VAL,
		SEM_SIGNAL,
		EV_CREATE,
		EV_DESTROY,
		EV_WAIT,
		THREAD_SIGNAL,
		THREAD_REG_HANDLER,
		THREAD_GET_HANDLER,
		THREAD_MASK_SIGNAL,
		THREAD_MASK_SIGNAL_GL,
		THREAD_UNMASK_SIGNAL,
		THREAD_UNMASK_SIGNAL_GL,
		THREAD_BLOCK_SIGNAL,
		THREAD_BLOCK_SIGNAL_GL,
		THREAD_UNBLOCK_SIGNAL,
		THREAD_UNBLOCK_SIGNAL_GL,
		THREAD_PAUSE
	};

	Name name;
	Thread *thread;
	Time timeSlice;
	ID id;
	StackSize stackSize;
	Semaphore *semaphore;
	int init;
	Time maxTimeToWait;
	Event *event;
	IVTNo n;
	SignalId signal;
	SignalHandler handler;
};

typedef void interrupt (*InterruptRoutinePtr)(...);

class PCB;

extern void tick();

class Kernel {
private:
	static PCB *running, *idle;
	static InterruptRoutinePtr oldTimerIntRoutine, old60IntRoutine;
	static int contextSwitchEnabled, contextSwitchRequested;
	static unsigned stack[];
	static unsigned ss, sp, bp;

	static void runWrapper();
	static void terminateRunningThread();
public:
	static InterruptRoutinePtr swapIntRoutines(IVTNo n,
			InterruptRoutinePtr newIntRoutine);
	static void setup();
	static void cleanup();
	static void interrupt newTimerIntRoutine(...);
	static void interrupt sysCallIntRoutine(...);
	static void sysCall(SysData &sysData);
	static void handleSignals();
	static void default0Handler();

	friend class PCB;
	friend class KernelSem;
	friend class Semaphore;
	friend class KernelEv;
	friend class Thread;
	friend void dispatch();
};

#endif // _KERNEL_H_