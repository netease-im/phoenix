// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/message_loop/message_loop_current.h"

#include "base/bind.h"
//#include "base/message_loop/message_pump_for_io.h"
//#include "base/message_loop/message_pump_for_ui.h"
#include "base/no_destructor.h"
#include "base/threading/thread_local.h"
#include "base/thread_task_runner_handle.h"

namespace base {

namespace {

base::ThreadLocalPointer<MessageLoop>* GetTLSMessageLoop() {
  static NoDestructor<ThreadLocalPointer<MessageLoop>> lazy_tls_ptr;
  return lazy_tls_ptr.get();
}

}  // namespace

//------------------------------------------------------------------------------
// MessageLoopCurrent

// static
MessageLoopCurrent MessageLoopCurrent::Get() {
  return MessageLoopCurrent(GetTLSMessageLoop()->Get());
}

// static
MessageLoopCurrent MessageLoopCurrent::GetNull() {
  return MessageLoopCurrent(nullptr);
}

// static
bool MessageLoopCurrent::IsSet() {
  return !!GetTLSMessageLoop()->Get();
}

void MessageLoopCurrent::AddDestructionObserver(
	MessageLoop::DestructionObserver* destruction_observer) {
  DCHECK(current_->IsBoundToCurrentThread());
  current_->AddDestructionObserver(destruction_observer);
}

void MessageLoopCurrent::RemoveDestructionObserver(
	MessageLoop::DestructionObserver* destruction_observer) {
  DCHECK(current_->IsBoundToCurrentThread());
  current_->RemoveDestructionObserver(destruction_observer);
}

std::string MessageLoopCurrent::GetThreadName() const {
  return current_->GetThreadName();
}

scoped_refptr<SingleThreadTaskRunner> MessageLoopCurrent::task_runner() const {
  DCHECK(current_->IsBoundToCurrentThread());
  return current_->GetTaskRunner();
}

void MessageLoopCurrent::SetTaskRunner(
    scoped_refptr<SingleThreadTaskRunner> task_runner) {
  DCHECK(current_->IsBoundToCurrentThread());
  current_->SetTaskRunner(std::move(task_runner));
}

bool MessageLoopCurrent::IsBoundToCurrentThread() const {
  return current_ == GetTLSMessageLoop()->Get();
}

bool MessageLoopCurrent::IsIdleForTesting() {
  DCHECK(current_->IsBoundToCurrentThread());
  return current_->IsIdleForTesting();
}

void MessageLoopCurrent::AddTaskObserver(TaskObserver* task_observer) {
  DCHECK(current_->IsBoundToCurrentThread());
  current_->AddTaskObserver(task_observer);
}

void MessageLoopCurrent::RemoveTaskObserver(TaskObserver* task_observer) {
  DCHECK(current_->IsBoundToCurrentThread());
  current_->RemoveTaskObserver(task_observer);
}

void MessageLoopCurrent::SetAddQueueTimeToTasks(bool enable) {
  DCHECK(current_->IsBoundToCurrentThread());
  current_->SetAddQueueTimeToTasks(enable);
}

void MessageLoopCurrent::SetNestableTasksAllowed(bool allowed) {
  DCHECK(current_->IsBoundToCurrentThread());
  current_->SetTaskExecutionAllowed(allowed);
}

bool MessageLoopCurrent::NestableTasksAllowed() const {
  return current_->IsTaskExecutionAllowed();
}

MessageLoopCurrent::ScopedNestableTaskAllower::ScopedNestableTaskAllower()
    : loop_(GetTLSMessageLoop()->Get()),
      old_state_(loop_->IsTaskExecutionAllowed()) {
  loop_->SetTaskExecutionAllowed(true);
}

MessageLoopCurrent::ScopedNestableTaskAllower::~ScopedNestableTaskAllower() {
  loop_->SetTaskExecutionAllowed(old_state_);
}

// static
void MessageLoopCurrent::BindToCurrentThreadInternal(MessageLoopBase* current) {
  DCHECK(!GetTLSMessageLoop()->Get())
      << "Can't register a second MessageLoop on the same thread.";
  GetTLSMessageLoop()->Set(current);
}

// static
void MessageLoopCurrent::UnbindFromCurrentThreadInternal(
	MessageLoop* current) {
  DCHECK_EQ(current, GetTLSMessageLoop()->Get());
  GetTLSMessageLoop()->Set(nullptr);
}

bool MessageLoopCurrent::operator==(const MessageLoopCurrent& other) const {
  return current_ == other.current_;
}
}  // namespace base
