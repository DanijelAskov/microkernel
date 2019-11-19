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

#include "kernel.h"
#include <dos.h>
#include "lock.h"
#include "ithread.h"
#include "pcb.h"
#include "ssched.h"
#include "kersem.h"
#include "semaphor.h"
#include "kerevent.h"
#include "kersem.h"

volatile SysData *globalSysData = 0;

extern void tick();

PCB *Kernel::running = 0, *Kernel::idle = 0;
InterruptRoutinePtr Kernel::oldTimerIntRoutine = 0;
int Kernel::contextSwitchEnabled = 0, Kernel::contextSwitchRequested = 0;
unsigned Kernel::stack[defaultStackSize];
#ifndef BCC_BLOCK_IGNORE
unsigned Kernel::ss = FP_SEG(Kernel::stack + defaultStackSize);
unsigned Kernel::sp = FP_OFF(Kernel::stack + defaultStackSize);
unsigned Kernel::bp = Kernel::sp;
#endif
InterruptRoutinePtr Kernel::old60IntRoutine = Kernel::swapIntRoutines(0x60,
		sysCallIntRoutine);

InterruptRoutinePtr Kernel::swapIntRoutines(IVTNo n,
		InterruptRoutinePtr newIntRoutine) {
	InterruptRoutinePtr oldIntRoutine = 0;
#ifndef BCC_BLOCK_IGNORE
	lock;
	oldIntRoutine = getvect(n);
	setvect(n, newIntRoutine);
	unlock;
#endif
	return oldIntRoutine;
}

void Kernel::setup() {
	contextSwitchEnabled = 0;
	oldTimerIntRoutine = swapIntRoutines(0x08, newTimerIntRoutine);
	idle = (new IdleThread())->myPCB;
	idle->start();
	PCB *main = (new Thread(0x10000, 1))->myPCB;
	main->state = PCB::READY;
	running = main;
	contextSwitchEnabled = 1;
}

void Kernel::cleanup() {
	contextSwitchEnabled = 0;
	swapIntRoutines(0x08, oldTimerIntRoutine);
	delete idle;
	contextSwitchEnabled = 1;
}

void Kernel::terminateRunningThread() {
	running->state = PCB::TERMINATED;
	LinkedList<PCB>::Iterator j = running->waiting.begin();
	while (j != running->waiting.end()) {
		j->state = PCB::READY;
		SScheduler::put(&(*j));
		j = running->waiting.erase(j);
	}
}

void Kernel::runWrapper() {
	running->myThread->run();
	if (running->parentThread)
		running->parentThread->signal(1);
	running->myThread->signal(2);
	SysData sysData;
	sysData.name = SysData::THREAD_TERMINATE;
	sysCall(sysData);
}

void interrupt Kernel::newTimerIntRoutine(...) {
	if (!KernelSem::locked) {
		KernelSem::updateSemaphores();
	}
	static unsigned ssTemp, spTemp, bpTemp, contextSwitched = 0;
	if (running->timeSlice != 0) {
		if ((running->timeRemaining == 0 ? 0 : --(running->timeRemaining))
				== 0) {
			if (contextSwitchEnabled) {
#ifndef BCC_BLOCK_IGNORE
				asm {
					mov ssTemp, ss
					mov spTemp, sp
					mov bpTemp, bp
				}
#endif
				running->ss = ssTemp;
				running->sp = spTemp;
				running->bp = bpTemp;
				if (running->state == PCB::READY && running != idle) {
					SScheduler::put(running);
				}
				running = SScheduler::get();
				if (!running) {
					running = idle;
				}
				if (running->timeRemaining == 0)
					running->timeRemaining = running->timeSlice;
				ssTemp = running->ss;
				spTemp = running->sp;
				bpTemp = running->bp;
#ifndef BCC_BLOCK_IGNORE
				asm {
					mov ss, ssTemp
					mov sp, spTemp
					mov bp, bpTemp
				}
#endif
				contextSwitched = 1;
			}
		} else
			contextSwitchRequested = 1;
	}
	(*oldTimerIntRoutine)();
	tick();
	if (contextSwitched) {
#ifndef BCC_BLOCK_IGNORE
		asm sti
#endif
		if (!running->signalsHandled)
			handleSignals();
	}
}

void interrupt Kernel::sysCallIntRoutine(...) {
	contextSwitchEnabled = 0;
	static unsigned cxTemp, dxTemp;
#ifndef BCC_BLOCK_IGNORE
	asm {
		mov cxTemp, cx
		mov dxTemp, dx
	}
	globalSysData = (SysData *)MK_FP(cxTemp, dxTemp);
#endif
	static unsigned ssTemp, spTemp, bpTemp;
#ifndef BCC_BLOCK_IGNORE
	asm {
		mov ssTemp, ss
		mov spTemp, sp
		mov bpTemp, bp
	}
#endif
	running->ss = ssTemp;
	running->sp = spTemp;
	running->bp = bpTemp;
	ssTemp = ss;
	spTemp = sp;
	bpTemp = bp;
#ifndef BCC_BLOCK_IGNORE
	asm {
		mov ss, ssTemp
		mov sp, spTemp
		mov bp, bpTemp
		sti
	}
#endif
	switch (globalSysData->name) {
	case SysData::THREAD_CREATE:
		globalSysData->thread->myPCB = new PCB(globalSysData->thread,
				globalSysData->stackSize, globalSysData->timeSlice);
		break;
	case SysData::THREAD_START:
		globalSysData->thread->myPCB->start();
		break;
	case SysData::THREAD_DESTROY:
		delete globalSysData->thread->myPCB;
		break;
	case SysData::THREAD_WAIT:
		if (globalSysData->thread->myPCB->waitToComplete())
			contextSwitchRequested = 1;
		break;
	case SysData::THREAD_GET:
		globalSysData->thread = PCB::getThreadById(globalSysData->id);
		break;
	case SysData::DISPATCH:
		contextSwitchRequested = 1;
		break;
	case SysData::THREAD_TERMINATE:
		terminateRunningThread();
		contextSwitchRequested = 1;
		break;
	case SysData::SEM_CREATE:
		globalSysData->semaphore->myImpl = new KernelSem(globalSysData->init);
		break;
	case SysData::SEM_DESTROY:
		delete globalSysData->semaphore->myImpl;
		break;
	case SysData::SEM_WAIT:
		globalSysData->semaphore->myImpl->wait();
		break;
	case SysData::SEM_SIGNAL:
		globalSysData->semaphore->myImpl->signal();
		break;
	case SysData::SEM_VAL:
		globalSysData->init = globalSysData->semaphore->myImpl->value;
		break;
	case SysData::EV_CREATE:
		globalSysData->event->myImpl = new KernelEv(globalSysData->n);
		break;
	case SysData::EV_DESTROY:
		delete globalSysData->event->myImpl;
		break;
	case SysData::EV_WAIT:
		globalSysData->event->myImpl->wait();
		break;
	case SysData::THREAD_SIGNAL:
		globalSysData->thread->myPCB->signal(globalSysData->signal);
		break;
	case SysData::THREAD_REG_HANDLER:
		globalSysData->thread->myPCB->registerHandler(globalSysData->signal,
				globalSysData->handler);
		break;
	case SysData::THREAD_GET_HANDLER:
		globalSysData->handler = globalSysData->thread->myPCB->getHandler(
				globalSysData->signal);
		break;
	case SysData::THREAD_MASK_SIGNAL:
		globalSysData->thread->myPCB->maskSignal(globalSysData->signal);
		break;
	case SysData::THREAD_MASK_SIGNAL_GL:
		PCB::maskSignalGlobally(globalSysData->signal);
		break;
	case SysData::THREAD_UNMASK_SIGNAL:
		globalSysData->thread->myPCB->unmaskSignal(globalSysData->signal);
		break;
	case SysData::THREAD_UNMASK_SIGNAL_GL:
		PCB::unmaskSignalGlobally(globalSysData->signal);
		break;
	case SysData::THREAD_BLOCK_SIGNAL:
		globalSysData->thread->myPCB->blockSignal(globalSysData->signal);
		break;
	case SysData::THREAD_BLOCK_SIGNAL_GL:
		PCB::blockSignalGlobally(globalSysData->signal);
		break;
	case SysData::THREAD_UNBLOCK_SIGNAL:
		globalSysData->thread->myPCB->unblockSignal(globalSysData->signal);
		break;
	case SysData::THREAD_UNBLOCK_SIGNAL_GL:
		PCB::unblockSignalGlobally(globalSysData->signal);
		break;
	case SysData::THREAD_PAUSE:
		PCB::pause();
		Kernel::contextSwitchRequested = 1;
		break;
	default:
		break;
	}
	PCB::updatePausedThreads();
	if (contextSwitchRequested) {
		contextSwitchRequested = 0;
		if (running->state == PCB::READY && running != idle) {
			SScheduler::put(running);
		}
		running = SScheduler::get();
		if (!running) {
			running = idle;
		}
		if (running->timeRemaining == 0)
			running->timeRemaining = running->timeSlice;
		ssTemp = running->ss;
		spTemp = running->sp;
		bpTemp = running->bp;
#ifndef BCC_BLOCK_IGNORE
		asm {
			cli
			mov ss, ssTemp
			mov sp, spTemp
			mov bp, bpTemp
			sti
		}
#endif
	} else {
		ssTemp = running->ss;
		spTemp = running->sp;
		bpTemp = running->bp;
#ifndef BCC_BLOCK_IGNORE
		asm {
			cli
			mov ss, ssTemp
			mov sp, spTemp
			mov bp, bpTemp
			sti
		}
#endif
	}
	contextSwitchEnabled = 1;
	if (running && !running->signalsHandled)
		handleSignals();
}

void Kernel::sysCall(SysData &sysData) {
	unsigned cxTemp, dxTemp;
#ifndef BCC_BLOCK_IGNORE
	asm {
		push cx
		push dx
	}
	cxTemp = FP_SEG(&sysData);
	dxTemp = FP_OFF(&sysData);
	asm {
		mov cx, cxTemp
		mov dx, dxTemp
		int SYS_CALL_ENTRY
		pop dx
		pop cx
	}
#endif
}

void Kernel::default0Handler() {
	if (running->id == 0)
		return;
	if (running->stack)
		delete running->stack;
	running->stack = 0;
	for (LinkedList<PCB::Signal>::Iterator j = running->signalsToHandle.begin();
			j != running->signalsToHandle.end();) {
		PCB::Signal *signal = &(*j);
		j = running->signalsToHandle.erase(j);
		delete signal;
	}
	SysData sysData;
	sysData.name = SysData::THREAD_TERMINATE;
	sysCall(sysData);
}

void Kernel::handleSignals() {
	if (running) {
		running->signalsHandled = 1;
		Kernel::contextSwitchEnabled = 0;
		for (LinkedList<PCB::Signal>::Iterator i =
				running->signalsToHandle.begin();
				i != running->signalsToHandle.end();) {
			SignalId signal = i->signalId;
			if (!(PCB::globalSignalsState[signal] & PCB::BLOCKED_)
					&& !(running->signalsState[signal] & PCB::BLOCKED_)) {
				if (running->handlers[signal]) {
					running->handlers[signal]();
				}
				PCB::Signal *handledSignal = &(*i);
				i = running->signalsToHandle.erase(i);
				delete handledSignal;
			} else
				i++;
		}
		Kernel::contextSwitchEnabled = 1;
		running->signalsHandled = 0;
	}
}