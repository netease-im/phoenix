// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>

#include "base/base_switches.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/location.h"
#include "base/memory/scoped_vector.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/stringprintf.h"
#include "base/synchronization/condition_variable.h"
#include "base/synchronization/lock.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread.h"
#include "base/time/time.h"
#include "build/build_config.h"

#if defined(OS_POSIX)
#include <pthread.h>
#endif

namespace base {

namespace {

const int kNumRuns = 100000;

// Same as above, but add observers to test their perf impact.
class MessageLoopObserver : public base::MessageLoop::TaskObserver {
 public:
  void WillProcessTask(const base::PendingTask& pending_task) override {}
  void DidProcessTask(const base::PendingTask& pending_task) override {}
};
MessageLoopObserver message_loop_observer;

class TaskObserverPerfTest : public TaskPerfTest {
 public:
  void Init() override {
    TaskPerfTest::Init();
    for (size_t i = 0; i < threads_.size(); i++) {
      threads_[i]->message_loop()->AddTaskObserver(&message_loop_observer);
    }
  }
};

TEST_F(TaskObserverPerfTest, TaskPingPong) {
  RunPingPongTest("1_Task_Threads_With_Observer", 1);
  RunPingPongTest("4_Task_Threads_With_Observer", 4);
}

// Class to test our WaitableEvent performance by signaling back and fort.
// WaitableEvent is templated so we can also compare with other versions.
template <typename WaitableEventType>
class EventPerfTest : public ThreadPerfTest {
 public:
  void Init() override {
    for (size_t i = 0; i < threads_.size(); i++)
      events_.push_back(new WaitableEventType(false, false));
  }

  void Reset() override { events_.clear(); }

  void WaitAndSignalOnThread(size_t event) {
    size_t next_event = (event + 1) % events_.size();
    int my_hops = 0;
    do {
      events_[event]->Wait();
      my_hops = --remaining_hops_;  // We own 'hops' between Wait and Signal.
      events_[next_event]->Signal();
    } while (my_hops > 0);
    // Once we are done, all threads will signal as hops passes zero.
    // We only signal completion once, on the thread that reaches zero.
    if (!my_hops)
      FinishMeasurement();
  }

  void PingPong(int hops) override {
    remaining_hops_ = hops;
    for (size_t i = 0; i < threads_.size(); i++) {
      threads_[i]->task_runner()->PostTask(
          FROM_HERE, base::Bind(&EventPerfTest::WaitAndSignalOnThread,
                                base::Unretained(this), i));
    }

    // Kick off the Signal ping-ponging.
    events_.front()->Signal();
  }

  int remaining_hops_;
  ScopedVector<WaitableEventType> events_;
};

// Similar to the task posting test, this just tests similar functionality
// using WaitableEvents. We only test four threads (worst-case), but we
// might want to craft a way to test the best-case (where the thread doesn't
// end up blocking because the event is already signalled).
typedef EventPerfTest<base::WaitableEvent> WaitableEventPerfTest;
TEST_F(WaitableEventPerfTest, EventPingPong) {
  RunPingPongTest("4_WaitableEvent_Threads", 4);
}

// Build a minimal event using ConditionVariable.
class ConditionVariableEvent {
 public:
  ConditionVariableEvent(bool manual_reset, bool initially_signaled)
      : cond_(&lock_), signaled_(false) {
    DCHECK(!manual_reset);
    DCHECK(!initially_signaled);
  }

  void Signal() {
    {
      base::AutoLock scoped_lock(lock_);
      signaled_ = true;
    }
    cond_.Signal();
  }

  void Wait() {
    base::AutoLock scoped_lock(lock_);
    while (!signaled_)
      cond_.Wait();
    signaled_ = false;
  }

 private:
  base::Lock lock_;
  base::ConditionVariable cond_;
  bool signaled_;
};

// This is meant to test the absolute minimal context switching time
// using our own base synchronization code.
typedef EventPerfTest<ConditionVariableEvent> ConditionVariablePerfTest;
TEST_F(ConditionVariablePerfTest, EventPingPong) {
  RunPingPongTest("4_ConditionVariable_Threads", 4);
}
#if defined(OS_POSIX)

// Absolutely 100% minimal posix waitable event. If there is a better/faster
// way to force a context switch, we should use that instead.
class PthreadEvent {
 public:
  PthreadEvent(bool manual_reset, bool initially_signaled) {
    DCHECK(!manual_reset);
    DCHECK(!initially_signaled);
    pthread_mutex_init(&mutex_, 0);
    pthread_cond_init(&cond_, 0);
    signaled_ = false;
  }

  ~PthreadEvent() {
    pthread_cond_destroy(&cond_);
    pthread_mutex_destroy(&mutex_);
  }

  void Signal() {
    pthread_mutex_lock(&mutex_);
    signaled_ = true;
    pthread_mutex_unlock(&mutex_);
    pthread_cond_signal(&cond_);
  }

  void Wait() {
    pthread_mutex_lock(&mutex_);
    while (!signaled_)
      pthread_cond_wait(&cond_, &mutex_);
    signaled_ = false;
    pthread_mutex_unlock(&mutex_);
  }

 private:
  bool signaled_;
  pthread_mutex_t mutex_;
  pthread_cond_t cond_;
};

// This is meant to test the absolute minimal context switching time.
// If there is any faster way to do this we should substitute it in.
typedef EventPerfTest<PthreadEvent> PthreadEventPerfTest;
TEST_F(PthreadEventPerfTest, EventPingPong) {
  RunPingPongTest("4_PthreadCondVar_Threads", 4);
}

#endif

}  // namespace

}  // namespace base
