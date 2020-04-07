#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

#define _cmd(task) \
  ((task)->comm) /* process name */

#define _pid(task) \
  ((task)->pid) /* process ID */

// parent == real_parent if process isn't being traced
#define _ppid(task) \
  (((task)->parent) == NULL ? -1 : (task)->parent->pid) /* parent pid */

// 1. prio 动态优先级
// 2. static_prio 静态优先级
// 3. normal_prio 归一化优先级
// 4. rt_priority 实时优先级
#define _prio(task) \
  ((task)->prio) /* priority */

#define _state(task) \
  ((task)->state) /* -1 unrunnable, 0 runnable, >0 stopped: */

#define _cond_helper(cond, ret, inner) \
  (_state(task) == cond ? ret : (inner))

#define _task_state(task) \
  (_cond_helper(TASK_RUNNING, "running", \
    _cond_helper(TASK_INTERRUPTIBLE, "interruptible", \
      _cond_helper(TASK_UNINTERRUPTIBLE, "uninterruptible", \
        _cond_helper(__TASK_STOPPED, "stopped", \
          _cond_helper(__TASK_TRACED, "traced", \
            _cond_helper(TASK_DEAD, "dead", \
              _cond_helper(TASK_WAKEKILL, "wakekill", \
                (_state(task) > 0 ? "runnable" : "unrunnable")))))))))

#define show_title(level) \
  printk(level "PID\tPPID\tPRIO\tSTATE\tSTATE INFO\tCMD\n")

#define show_process_info(level, task) \
  printk(level "%d\t%4d\t%4d\t%5ld\t%-15s\t%s\n", \
         _pid(task),  \
         _ppid(task), \
         _prio(task), \
         _state(task), \
         _task_state(task), \
         _cmd(task))

static int __init psinfo_init(void)
{
  struct task_struct *task;

  show_title(KERN_INFO);
  for_each_process(task)
  {
    // kernel threads
    if (task->mm == NULL)
      show_process_info(KERN_INFO, task);
  }

  return 0;
}

static void __exit psinfo_exit(void) {}

module_init(psinfo_init);
module_exit(psinfo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jason Lyu");
MODULE_DESCRIPTION("The one list all kernel threads info.");
