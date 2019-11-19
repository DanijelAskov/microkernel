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

#include "ssched.h"
#include "schedule.h"
#include "lock.h"

void SScheduler::put(PCB *pcb) {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	Scheduler::put(pcb);
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
}

PCB *SScheduler::get() {
#ifndef BCC_BLOCK_IGNORE
	lock;
#endif
	PCB *result = Scheduler::get();
#ifndef BCC_BLOCK_IGNORE
	unlock;
#endif
	return result;
}