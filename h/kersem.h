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

#ifndef _KERSEM_H_
#define _KERSEM_H_

#include "kersem.h"

#include "list.h"
#include "pcb.h"

class KernelSem {
private:
	static LinkedList<KernelSem> kernelSemaphores;

	int value;
	LinkedList<PCB> waiting;
protected:
	void block();
	void deblock();
public:
	static int locked;

	KernelSem(int init);
	~KernelSem();
	void wait();
	void signal();

	static void updateSemaphores();

	friend class Semaphore;
	friend class Kernel;
	friend class IVTEntry;
};

#endif // _KERSEM_H_