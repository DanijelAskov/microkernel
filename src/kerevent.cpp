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

#include "kerevent.h"
#include "kernel.h"
#include "ivtentry.h"
#include "pcb.h"
#include "list.h"
#include "ssched.h"

KernelEv::KernelEv(IVTNo n) :
		n(n), value(0), waiting(0) {
	owner = Kernel::running;
	IVTEntry *myEntry = IVTEntry::entries[n];
	myEntry->myEvent = this;
	if (n != sysCallEntry)
		myEntry->intRoutine = Kernel::swapIntRoutines(n, myEntry->intRoutine);
	else
		myEntry->intRoutine = 0;
}

KernelEv::~KernelEv() {
	waiting->state = PCB::READY;
	SScheduler::put(waiting);
	IVTEntry *myEntry = IVTEntry::entries[n];
	myEntry->myEvent = 0;
	if (n != sysCallEntry)
		Kernel::swapIntRoutines(n, myEntry->intRoutine);
}

void KernelEv::signal() {
	if (!waiting)
		value = 1;
	else {
		waiting->state = PCB::READY;
		SScheduler::put(waiting);
		Kernel::contextSwitchRequested = 1;
		waiting = 0;
	}
}

void KernelEv::wait() {
	if (Kernel::running == owner) {
		if (!value) {
			Kernel::running->state = PCB::BLOCKED;
			waiting = Kernel::running;
			Kernel::contextSwitchRequested = 1;
		} else
			value = 0;
	}
}