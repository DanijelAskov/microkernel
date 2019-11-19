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

#ifndef _SEMAPHOR_H_
#define _SEMAPHOR_H_

typedef unsigned int Time;

class KernelSem;

class Semaphore {
public:
	Semaphore(int init = 1);
	virtual ~Semaphore();
	virtual int wait(Time maxTimeToWait);
	virtual void signal();
	int val() const;
private:
	KernelSem* myImpl;
	friend class Kernel;
};

#endif // _SEMAPHOR_H_