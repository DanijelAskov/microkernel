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

#ifndef _IVTENTRY_H_
#define _IVTENTRY_H_

#include "kernel.h"
#include "kerevent.h"
#include "list.h"

const int ivtSize = 256;

typedef void interrupt (*InterruptRoutinePtr)(...);
typedef unsigned char IVTNo;

class IVTEntry {
	static IVTEntry *entries[ivtSize];

	IVTNo n;
	KernelEv *myEvent;
	InterruptRoutinePtr intRoutine;
public:
	IVTEntry(IVTNo n, InterruptRoutinePtr newIntRoutine);
	~IVTEntry();
	static IVTEntry *getEnty(IVTNo n);
	void signal();
	void callOldIntRoutine();

	friend class KernelEv;
};

#endif // _IVTENTRY_H_