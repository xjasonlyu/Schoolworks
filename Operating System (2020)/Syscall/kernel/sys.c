...

SYSCALL_DEFINE5(renice, pid_t, pid, int, flag, int, nicevalue, void __user *, prio, void __user *, nice)
{
	int _nice;
	int _prio;
	// set default errno to zero
	int error = 0;

	struct pid * kpid;
    struct task_struct * task;

	// get kernel pid from pid
    kpid = find_get_pid(pid);

	// kpid validation check
	if (!kpid)
	{
		error = -EFAULT;
		goto final;
	}

	// get task struct from kpid
    task = pid_task(kpid, PIDTYPE_PID);

	switch (flag)
	{
	case 0: /* read pid nice value */
		// do nothing
		break;

	case 1: /* write pid nice value */
		// modify task nice
		set_user_nice(task, nicevalue);
		break;

	default: /* unknown flag */
		error = -EFAULT;
		break;
	}

final:
	if (!error)
	{
		// get nice & prio value from task
		_nice = task_nice(task);
		_prio = task_prio(task);

		// copy nice & prio value to user space
		error = copy_to_user(nice, &_nice, sizeof(_nice)) ||
			copy_to_user(prio, &_prio, sizeof(_prio)) ? -EFAULT : 0;
	}
    return error;
}
