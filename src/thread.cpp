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

#include "thread.h"
#include "kernel.h"
#include "pcb.h"
#include <dos.h>

Thread::Thread(StackSize stackSize, Time timeSlice) {
	SysData sysData;
	sysData.name = SysData::THREAD_CREATE;
	sysData.thread = this;
	sysData.stackSize = stackSize;
	sysData.timeSlice = timeSlice;
	Kernel::sysCall(sysData);
}

Thread::~Thread() {
	waitToComplete();
	SysData sysData;
	sysData.name = SysData::THREAD_DESTROY;
	sysData.thread = this;
	Kernel::sysCall(sysData);
}

void Thread::start() {
	SysData sysData;
	sysData.name = SysData::THREAD_START;
	sysData.thread = this;
	Kernel::sysCall(sysData);
}

void Thread::waitToComplete() {
	SysData sysData;
	sysData.name = SysData::THREAD_WAIT;
	sysData.thread = this;
	Kernel::sysCall(sysData);
}

ID Thread::getId() {
	return myPCB->id;
}

ID Thread::getRunningId() {
	return Kernel::running->id;
}

Thread *Thread::getThreadById(ID id) {
	if (id <= -1)
		return 0;
	SysData sysData;
	sysData.name = SysData::THREAD_GET;
	sysData.id = id;
	Kernel::sysCall(sysData);
	return sysData.thread;
}

void dispatch() {
	SysData sysData;
	sysData.name = SysData::DISPATCH;
	Kernel::sysCall(sysData);
}

void Thread::signal(SignalId signal) {
	SysData sysData;
	sysData.name = SysData::THREAD_SIGNAL;
	sysData.thread = this;
	sysData.signal = signal;
	Kernel::sysCall(sysData);
}

void Thread::registerHandler(SignalId signal, SignalHandler handler) {
	SysData sysData;
	sysData.name = SysData::THREAD_REG_HANDLER;
	sysData.thread = this;
	sysData.signal = signal;
	sysData.handler = handler;
	Kernel::sysCall(sysData);
}

SignalHandler Thread::getHandler(SignalId signal) {
	SysData sysData;
	sysData.name = SysData::THREAD_GET_HANDLER;
	sysData.thread = this;
	sysData.signal = signal;
	Kernel::sysCall(sysData);
	return sysData.handler;
}

void Thread::maskSignal(SignalId signal) {
	SysData sysData;
	sysData.name = SysData::THREAD_MASK_SIGNAL;
	sysData.thread = this;
	sysData.signal = signal;
	Kernel::sysCall(sysData);
}

void Thread::maskSignalGlobally(SignalId signal) {
	SysData sysData;
	sysData.name = SysData::THREAD_MASK_SIGNAL_GL;
	sysData.signal = signal;
	Kernel::sysCall(sysData);
}

void Thread::unmaskSignal(SignalId signal) {
	SysData sysData;
	sysData.name = SysData::THREAD_UNMASK_SIGNAL;
	sysData.thread = this;
	sysData.signal = signal;
	Kernel::sysCall(sysData);
}

void Thread::unmaskSignalGlobally(SignalId signal) {
	SysData sysData;
	sysData.name = SysData::THREAD_UNMASK_SIGNAL_GL;
	sysData.signal = signal;
	Kernel::sysCall(sysData);
}

void Thread::blockSignal(SignalId signal) {
	SysData sysData;
	sysData.name = SysData::THREAD_BLOCK_SIGNAL;
	sysData.thread = this;
	sysData.signal = signal;
	Kernel::sysCall(sysData);
}

void Thread::blockSignalGlobally(SignalId signal) {
	SysData sysData;
	sysData.name = SysData::THREAD_BLOCK_SIGNAL_GL;
	sysData.signal = signal;
	Kernel::sysCall(sysData);
}

void Thread::unblockSignal(SignalId signal) {
	SysData sysData;
	sysData.name = SysData::THREAD_UNBLOCK_SIGNAL;
	sysData.thread = this;
	sysData.signal = signal;
	Kernel::sysCall(sysData);
}

void Thread::unblockSignalGlobally(SignalId signal) {
	SysData sysData;
	sysData.name = SysData::THREAD_UNBLOCK_SIGNAL_GL;
	sysData.signal = signal;
	Kernel::sysCall(sysData);
}

void Thread::pause() {
	SysData sysData;
	sysData.name = SysData::THREAD_PAUSE;
	Kernel::sysCall(sysData);
}