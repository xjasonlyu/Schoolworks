#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/stat.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/moduleparam.h>

#define KBUILD_MODNAME "pstree"

static int pid = 1;
module_param(pid, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(pid, "Process ID to inspect");

#define list_length(p, h, l) \
  list_for_each(p, h) (l)++

#define list_for_each_count(pos, head) \
  count = 1; \
	for (pos = (head)->next; pos != (head); pos = pos->next, count++)

#define show_process_info(prefix, task) \
  printk(KERN_INFO prefix "%s-(%d)-(%ld)\n", \
        (task)->comm, \
        (task)->pid,  \
        (task)->state)

static int __init pstree_init(void)
{
  int count;
  int l_sibling = 0;
  int l_children = 0;

  struct pid *t_pid;
  struct list_head *t_pos;

  struct task_struct *task;
  struct task_struct *child;

  t_pid = find_get_pid(pid);
  if (t_pid == NULL)
  {
    printk(KERN_ALERT "PID %d not found\n", pid);
    goto out;
  }

  task = pid_task(t_pid, PIDTYPE_PID);

  // show parent info
  if (task->parent == NULL)
  {
    printk(KERN_INFO "[no parent]");
  }
  else
  {
    show_process_info("[parent]", task->parent);
  }

  list_length(t_pos, &task->children, l_children);
  list_length(t_pos, &task->parent->children, l_sibling);

  // show self info
  if (l_sibling == 0)
    show_process_info("\t\t└─[self]", task);
  else
    show_process_info("\t\t├─[self]", task);

/*
Refer: https://stackoverflow.com/questions/34704761/why-sibling-list-is-used-to-get-the-task-struct-while-fetching-the-children-of-a

In order to organize data as linked list using struct list_head you have to
declare list root and declare list entry for linkage. Both root and child
entries are the same type (struct list_head). children entry of struct task_struct
entry is a root. sibling entry of struct task_struct is a list entry.
To see the differences, you have to read code, where children and sibling are used.
Usage of list_for_each for children means what children is a root. 
Usage of list_entry for sibling means what sibling is a list entry.
*/

  // show children info
  list_for_each_count(t_pos, &task->children)
  {
    child = list_entry(t_pos, struct task_struct, sibling);

    if (l_sibling == 0)
      if (count != l_children)
        show_process_info("\t\t \t├─[children]", child);
      else
        show_process_info("\t\t \t└─[children]", child);
    else
      if (count != l_children)
        show_process_info("\t\t│\t├─[children]", child);
      else
        show_process_info("\t\t│\t└─[children]", child);
  }

  // show sibling info
  list_for_each_count(t_pos, &task->parent->children)
  {
    child = list_entry(t_pos, struct task_struct, sibling);

    // exclude self
    if (child->pid == task->pid)
      continue;

    if (count != l_sibling)
      show_process_info("\t\t├─[sibling]", child);
    else
      show_process_info("\t\t└─[sibling]", child);
  }

out:
  return 0;
}

static void __exit pstree_exit(void) {}

module_init(pstree_init);
module_exit(pstree_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jason Lyu");
MODULE_DESCRIPTION("The one list process family info.");
