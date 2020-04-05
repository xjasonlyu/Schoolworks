...

// Custom syscall: renice
asmlinkage long sys_renice(pid_t pid, int flag, int nicevalue,
			  void __user *prio, void __user *nice);
