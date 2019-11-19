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

#include "ivtentry.h"
#include "kernel.h"
#include "kerevent.h"
#include "lock.h"

IVTEntry *IVTEntry::entries[] = { 0 };

IVTEntry *IVTEntry::getEnty(IVTNo n) {
	return entries[n];
}

IVTEntry::IVTEntry(IVTNo n, InterruptRoutinePtr newIntRoutine) :
		n(n), myEvent(0), intRoutine(newIntRoutine) {
	entries[n] = this;
}

IVTEntry::~IVTEntry() {
	entries[n] = 0;
}

void IVTEntry::signal() {
	if (myEvent)
		myEvent->signal();
}

void IVTEntry::callOldIntRoutine() {
	if (intRoutine) {
#ifndef BCC_BLOCK_IGNORE
		lock;
#endif
		intRoutine();
#ifndef BCC_BLOCK_IGNORE
		unlock;
#endif
	}
}