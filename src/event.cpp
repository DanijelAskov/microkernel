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

#include "event.h"
#include "kernel.h"
#include "kerevent.h"

Event::Event(IVTNo n) {
	SysData sysData;
	sysData.name = SysData::EV_CREATE;
	sysData.event = this;
	sysData.n = n;
	Kernel::sysCall(sysData);
}

Event::~Event() {
	SysData sysData;
	sysData.name = SysData::EV_DESTROY;
	sysData.event = this;
	Kernel::sysCall(sysData);
}

void Event::wait() {
	SysData sysData;
	sysData.name = SysData::EV_WAIT;
	sysData.event = this;
	Kernel::sysCall(sysData);
}

void Event::signal() {
	myImpl->signal();
}