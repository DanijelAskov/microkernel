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
 
#include "pcb.h"
#include "kernel.h"
#include <dos.h>
#include "ssched.h"

ID PCB::idCounter = -1;
LinkedList<PCB> PCB::userThreads;
int PCB::globalSignalsState[] = { PCB::DEFAULT };
LinkedList<PCB> PCB::signalWaitingThreads;

PCB::PCB(Thread *myThread, StackSize stackSize, Time timeSlice) :
		myThread(myThread), stackSize(stackSize / sizeof(unsigned)), timeSlice(
				timeSlice), timeRemaining(timeSlice) {
	stack = 0;
	ss = sp = bp = 0;
	id = idCounter++;
	maxTimeToWait = 0;
	state = NEW;
	if (id <= 0) {
		for (int i = 0; i < signalCount; i++) {
			handlers[i] = ((i == 0) ? Kernel::default0Handler : 0);
			signalsState[i] = DEFAULT;
		}
	} else {
		for (int i = 0; i < signalCount; i++) {
			handlers[i] = Kernel::running->handlers[i];
			signalsState[i] = Kernel::running->signalsState[i];
		}
	}
	deblock = signalsHandled = 0;
	parentThread = (id > 0) ? Kernel::running->myThread : 0;
	if (id >= 0)
		userThreads.insert(userThreads.end(), this);
}

PCB::~PCB() {
	if (stack)
		delete stack;
	for (LinkedList<PCB>::Iterator i = PCB::userThreads.begin();
			i != PCB::userThreads.end(); i++)
		if (&(*i) == this) {
			PCB::userThreads.erase(i);
			break;
		}
	for (LinkedList<Signal>::Iterator j = signalsToHandle.begin();
			j != signalsToHandle.end();) {
		Signal *sginal = &(*j);
		j = signalsToHandle.erase(j);
		delete sginal;
	}
}

void PCB::createStack() {
	stack = new unsigned[stackSize];
	stack[stackSize - 1] = 0x200;
#ifndef BCC_BLOCK_IGNORE
	stack[stackSize - 2] = FP_SEG(&Kernel::runWrapper);
	stack[stackSize - 3] = FP_OFF(&Kernel::runWrapper);
	ss = FP_SEG(stack + stackSize - 12);
	sp = bp = FP_OFF(stack + stackSize - 12);
#endif
}

void PCB::start() {
	createStack();
	state = READY;
	if (id >= 0)
		SScheduler::put(this);
}

int PCB::waitToComplete() {
	if (state == TERMINATED || this == Kernel::running)
		return 0;
	Kernel::running->state = BLOCKED;
	waiting.insert(waiting.end(), Kernel::running);
	return 1;
}

Thread* PCB::getThreadById(ID id) {
	PCB *result = 0;
	for (LinkedList<PCB>::Iterator i = PCB::userThreads.begin();
			i != PCB::userThreads.end(); i++) {
		if (i->id == id) {
			result = &(*i);
			break;
		}
	}
	return result ? result->myThread : 0;
}

void PCB::signal(SignalId signal) {
	if (signal < signalCount) {
		if (!(globalSignalsState[signal] & MASKED)
				&& !(signalsState[signal] & MASKED)) {
			signalsToHandle.insert(signalsToHandle.end(), new Signal(signal));
			deblock = 1;
		}
	}
}

void PCB::registerHandler(SignalId signal, SignalHandler handler) {
	if (signal < signalCount) {
		handlers[signal] = handler;
	}
}

SignalHandler PCB::getHandler(SignalId signal) {
	if (signal < signalCount) {
		return handlers[signal];
	} else
		return 0;
}

void PCB::maskSignal(SignalId signal) {
	if (signal < signalCount) {
		signalsState[signal] |= MASKED;
	}
}

void PCB::maskSignalGlobally(SignalId signal) {
	if (signal < signalCount) {
		globalSignalsState[signal] |= MASKED;
	}
}

void PCB::unmaskSignal(SignalId signal) {
	if (signal < signalCount) {
		signalsState[signal] &= ~MASKED;
	}
}

void PCB::unmaskSignalGlobally(SignalId signal) {
	if (signal < signalCount) {
		globalSignalsState[signal] &= ~MASKED;
	}
}

void PCB::blockSignal(SignalId signal) {
	if (signal < signalCount) {
		signalsState[signal] |= BLOCKED_;
	}
}

void PCB::blockSignalGlobally(SignalId signal) {
	if (signal < signalCount) {
		globalSignalsState[signal] |= BLOCKED_;
	}
}

void PCB::unblockSignal(SignalId signal) {
	if (signal < signalCount) {
		signalsState[signal] &= ~BLOCKED_;
	}
}

void PCB::unblockSignalGlobally(SignalId signal) {
	if (signal < signalCount) {
		globalSignalsState[signal] &= ~BLOCKED_;
	}
}

void PCB::pause() {
	Kernel::running->state = BLOCKED;
	Kernel::running->deblock = 0;
	signalWaitingThreads.insert(signalWaitingThreads.end(), Kernel::running);
}

void PCB::updatePausedThreads() {
	for (LinkedList<PCB>::Iterator i = signalWaitingThreads.begin();
			i != signalWaitingThreads.end();) {
		if (i->deblock != 0) {
			i->state = PCB::READY;
			SScheduler::put(&(*i));
			i = signalWaitingThreads.erase(i);
		} else
			i++;
	}
}