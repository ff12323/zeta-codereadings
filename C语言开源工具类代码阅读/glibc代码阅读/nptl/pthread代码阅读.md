

## ch0 记录

索引：

- `descr.h` ：**C11 thread**





## pthread_create.c

CONCURRENCY NOTES:

（，理解谁是pthread结构体或参数PD的所有者是至关重要的，在准确地决定哪些操作是允许的以及哪些操作是不允许的，。。。尤其是现实所有会操作PD的。。。函数）Understanding who is the owner of the 'struct pthread' or 'PD'  (refers to the value of the 'struct pthread *pd' function argument)  <u>is critically important in determining exactly which operations are  allowed and which are not and when,</u> particularly when it comes to the  implementation of pthread_create, pthread_join, pthread_detach, and  other functions which all operate on PD.

（，PD的所有者负责释放与PD相关的最终资源，或者检查PD潜在的内存在任何时间点而直到其释放还给OS或在运行时被重用；）The owner of PD is responsible for <u>freeing the final resources</u>  associated with PD, and may examine the memory underlying PD at any  point in time until it frees it back to the OS or to reuse by the  runtime.

（，。。。创建者线程是PD的开始所有者；） The thread which calls pthread_create is called **the creating thread**.  The creating thread begins as the owner of PD.

 During startup the new thread may examine PD in coordination with the  owner thread (which may be itself).

 The four cases of ownership transfer are:  
- (1) Ownership of PD is released to the process (all threads may use it)  after the new thread starts in a joinable state  i.e. pthread_create returns a usable pthread_t.  
- (2) Ownership of PD is released to the new thread starting in a detached  state.  
- (3) Ownership of PD is dynamically released to a running thread via  pthread_detach.  
- (4) Ownership of PD is acquired by the thread which calls pthread_join.

Implementation notes:

The PD->stopped_start and thread_ran variables are used to determine  exactly which of the four ownership states we are in and therefore  what actions can be taken.  <u>For example after (2) we cannot read or  write from PD anymore since the thread may no longer exist and the  memory may be unmapped.</u>

It is important to point out that PD->lock is being used both  similar to a one-shot semaphore and subsequently as a mutex.  The  lock is taken in the parent to force the child to wait, and then the  child releases the lock.  However, this semaphore-like effect is used  only for synchronizing the parent and child.  After startup the lock  is used like a mutex to create a **critical section** during which a  single owner modifies the thread parameters.

> one-shot：单次

The most complicated cases happen during thread startup:

-  (a) If the created thread is in a detached (PTHREAD_CREATE_DETACHED),  or joinable (default PTHREAD_CREATE_JOINABLE) state and  STOPPED_START is true, then the creating thread has ownership of  PD until the PD->lock is released by pthread_create.  If any  errors occur we are in states (c), (d), or (e) below.
-  (b) If the created thread is in a detached state  (PTHREAD_CREATED_DETACHED), and STOPPED_START is false, then the  creating thread has ownership of PD until it invokes the OS  kernel's thread creation routine.  If this routine returns  without error, then the created thread owns PD; otherwise, see  (c) and (e) below.
-  (c) If the detached thread setup failed and THREAD_RAN is true, then  the creating thread releases ownership to the new thread by  sending a cancellation signal.  All threads set THREAD_RAN to  true as quickly as possible after returning from the OS kernel's  thread creation routine.
-  (d) If the joinable thread setup failed and THREAD_RAN is true, then the creating thread retains ownership of PD and must cleanup  state.  Ownership cannot be released to the process via the  return of pthread_create since a non-zero result entails PD is  undefined and therefore cannot be joined to free the resources.  We privately call pthread_join on the thread to finish handling  the resource shutdown (Or at least we should, see bug 19511).
-  (e) If the thread creation failed and THREAD_RAN is false, then the  creating thread retains ownership of PD and must cleanup state.  No waiting for the new thread is required because it never  started.

> https://sourceware.org/bugzilla/show_bug.cgi?id=19511
>
> [Bug 19511] 8MB memory leak in pthread_create in case of failure when non-root user changes priority

The nptl_db interface:

The interface with nptl_db requires that we enqueue PD into a linked  list and then call a function which the debugger will trap.  The PD  will then be dequeued and control returned to the thread.  The caller  at the time must have ownership of PD and such ownership remains  after control returns to thread. The enqueued PD is removed from the  linked list by the nptl_db callback `td_thr_event_getmsg`.  The debugger must ensure that the thread does not resume execution, otherwise  ownership of PD may be lost and examining PD will not be possible.

 Note that the GNU Debugger as of (December 10th 2015) commit  `c2c2a31fdb228d41ce3db62b268efea04bd39c18` no longer uses  td_thr_event_getmsg and several other related nptl_db interfaces. The  principal reason for this is that nptl_db does not support non-stop  mode where other threads can run concurrently and modify runtime  structures currently in use by the debugger and the nptl_db  interface.

Axioms：

- The create_thread function can never set stopped_start to false.  
- The created thread can read stopped_start but never write to it.  
- The variable thread_ran is set some time after the OS thread  creation routine returns, how much time after the thread is created  is unspecified, but it should be as quickly as possible.

CREATE THREAD NOTES:

 createthread.c defines the create_thread function, and two macros:  START_THREAD_DEFN and START_THREAD_SELF (see below).

 create_thread must initialize PD->stopped_start.  It should be true  if the STOPPED_START parameter is true, or if create_thread needs the  new thread to synchronize at startup for some other implementation  reason.  If STOPPED_START will be true, then create_thread is obliged  to lock PD->lock before starting the thread.  Then pthread_create  unlocks PD->lock which synchronizes-with START_THREAD_DEFN in the  child thread which does an acquire/release of PD->lock as the last  action before calling the user entry point.  The goal of all of this  is to ensure that the required initial thread attributes are applied  (by the creating thread) before the new thread runs user code.  Note  that the the functions pthread_getschedparam, pthread_setschedparam,  pthread_setschedprio, `__pthread_tpp_change_priority`, and  `__pthread_current_priority` reuse the same lock, PD->lock, for a  similar purpose e.g. synchronizing the setting of similar thread  attributes.  These functions are never called before the thread is  created, so don't participate in startup syncronization, but given  that the lock is present already and in the unlocked state, reusing  it saves space.

 The return value is zero for success or an errno code for failure.  If the return value is ENOMEM, that will be translated to EAGAIN,  so create_thread need not do that.  On failure, `*THREAD_RAN` should  be set to true iff the thread actually started up and then got  canceled before calling user code (`*PD->start_routine`). 

#### createthread.c

【文件】 `#include <createthread.c>` ，将c文件包含到当前文件中；

- 文件：被 

```c
#define START_THREAD_DEFN \
  static void __attribute__ ((noreturn)) start_thread (void)
#define START_THREAD_SELF THREAD_SELF
```

create_thread：

```
Para：
	pd：
	attr：
	stopped_start：
	STACK_VARIABLES_PARMS
	thread_ran：
```

源码：

```c
static int
create_thread (struct pthread *pd, const struct pthread_attr *attr,
	       bool *stopped_start, STACK_VARIABLES_PARMS, bool *thread_ran)
{
  /* If the implementation needs to do some tweaks to the thread after
     it has been created at the OS level, it can set STOPPED_START here.  */

  pd->stopped_start = *stopped_start;
  if (__glibc_unlikely (*stopped_start))
    /* See CONCURRENCY NOTES in nptl/pthread_create.c.  */
    lll_lock (pd->lock, LLL_PRIVATE);

  return ENOSYS;

  /* It's started now, so if we fail below, we'll have to cancel it
     and let it clean itself up.  */
  *thread_ran = true;

  return 0;
}
```

