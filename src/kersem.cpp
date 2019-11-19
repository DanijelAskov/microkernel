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

#include "kersem.h"
#include "pcb.h"
#include "kernel.h"
#include "ssched.h"

LinkedList<KernelSem> KernelSem::kernelSemaphores;
int KernelSem::locked = 0;

KernelSem::KernelSem(int init) :
		value(init) {
	KernelSem::locked = 1;
	kernelSemaphores.insert(kernelSemaphores.end(), this);
	KernelSem::locked = 0;
}

KernelSem::~KernelSem() {
	KernelSem::locked = 1;
	LinkedList<PCB>::Iterator i = waiting.begin();
	while (i != waiting.end()) {
		i->state = PCB::READY;
		SScheduler::put(&(*i));
		i = waiting.erase(i);
	}
	for (LinkedList<KernelSem>::Iterator j = kernelSemaphores.begin();
			j != kernelSemaphores.end(); j++) {
		if (&(*j) == this) {
			kernelSemaphores.erase(j);
			break;
		}
	}
	KernelSem::locked = 0;
}

void KernelSem::wait() {
	if (--value < 0) {
		KernelSem::locked = 1;
		block();
		KernelSem::locked = 0;
	}
}

void KernelSem::signal() {
	KernelSem::locked = 1;
	if (value++ < 0)
		deblock();
	KernelSem::locked = 0;
}

void KernelSem::block() {
	Kernel::running->state = PCB::BLOCKED;
	waiting.insert(waiting.end(), Kernel::running);
	Kernel::contextSwitchRequested = 1;
}

void KernelSem::deblock() {
	LinkedList<PCB>::Iterator i = waiting.begin();
	i->state = PCB::READY;
	SScheduler::put(&(*i));
	waiting.erase(i);
}

void KernelSem::updateSemaphores() {
	for (LinkedList<KernelSem>::Iterator i = kernelSemaphores.begin();
			i != kernelSemaphores.end(); i++) {
		LinkedList<PCB>::Iterator j = i->waiting.begin();
		while (j != i->waiting.end()) {
			if (j->maxTimeToWait != 0 && --(j->maxTimeToWait) == 0) {
				j->state = PCB::READY;
				SScheduler::put(&(*j));
				j = i->waiting.erase(j);
			} else
				j++;
		}
	}
}