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

#include "semaphor.h"
#include "kernel.h"
#include "kersem.h"

Semaphore::Semaphore(int init) {
	SysData sysData;
	sysData.name = SysData::SEM_CREATE;
	sysData.semaphore = this;
	sysData.init = init;
	Kernel::sysCall(sysData);
}

Semaphore::~Semaphore() {
	SysData sysData;
	sysData.name = SysData::SEM_DESTROY;
	sysData.semaphore = this;
	Kernel::sysCall(sysData);
}

int Semaphore::wait(Time maxTimeToWait) {
	int waitsForever = !(Kernel::running->maxTimeToWait = maxTimeToWait);
	SysData sysData;
	sysData.name = SysData::SEM_WAIT;
	sysData.semaphore = this;
	Kernel::sysCall(sysData);
	if (!waitsForever && !Kernel::running->maxTimeToWait)
		return 0;
	else
		return 1;
}

void Semaphore::signal() {
	SysData sysData;
	sysData.name = SysData::SEM_SIGNAL;
	sysData.semaphore = this;
	Kernel::sysCall(sysData);
}

int Semaphore::val() const {
	SysData sysData;
	sysData.name = SysData::SEM_VAL;
	sysData.semaphore = (Semaphore *) this;
	Kernel::sysCall(sysData);
	return sysData.init;
}