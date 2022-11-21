#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>

#include <linux/sched.h>

#include <linux/utsname.h> // kernel version

struct task_struct *task;

#define MAX_PROC_SIZE 100
static char proc_data[MAX_PROC_SIZE];
char *endp;
int ipid=0;

static struct proc_dir_entry *proc_write_entry;

int read_proc(char *buf,char **start,off_t offset,int count,int *eof,void *data )
{
  int len=0;
  printk("READing PID= %d\n",ipid);
  for_each_process( task ){
      if(task->pid == ipid){
		
		int ppid = task_pid_nr(task->parent);
		int count = 0;
	    struct list_head * p;
		list_for_each(p, &(task)->children){
			count++;
		}

		len = sprintf(buf,"Parent pid: %d\nCode Segment start = 0x%lx, end =0x%lx\nData Segment start = 0x%lx,end = 0x%lx\nHeap  Segment start = 0x%lx, end = 0x%lx\nKernel Version %s\nChild count %d\n",ppid,task->mm->start_code, task->mm->end_code,task->mm->start_data, task->mm->end_data,task->mm->start_brk,task->mm->brk,utsname()->release,count);
		/*
		int ppid = task_pid_nr(task->parent);
		printk("=======Reading Proc=======\n");
		printk("The ipid is %d\n", ipid);
		printk("The Current process id is %d\n", ipid);
		printk("The Parent process id is %d\n", ppid);
		printk("Code  Segment start = 0x%lx, end = 0x%lx \n"
                "Data  Segment start = 0x%lx, end = 0x%lx\n"
				"Heap  Segment start = 0x%lx, end = 0x%lx\n",
                task->mm->start_code, task->mm->end_code,
                task->mm->start_data, task->mm->end_data,
				task->mm->start_brk,task->mm->brk);
		printk("Kernel Version = %s\n",utsname()->release);
		struct list_head * p;
		int count = 0;
		list_for_each(p, &(task)->children){
			count++;
		}
		printk("The Child Number is : %d\n",count);
		printk("===========================\n"); 
		*/
      }
   }

	
    return len;
}

int write_proc(struct file *file,const char *buf,int count,void *data )
{

if(count > MAX_PROC_SIZE)
    count = MAX_PROC_SIZE;
if(copy_from_user(proc_data, buf, count))
      return -EFAULT;
    ipid=simple_strtol(proc_data,&endp,10);
	printk("Data from the user: %d\n", ipid);
return count;
}

void create_new_proc_entry()
{
    proc_write_entry = create_proc_entry("myproc",0666,(void *)NULL);
if(!proc_write_entry)
      {
    printk(KERN_INFO "Error creating proc entry");
    return;
//    return -ENOMEM;
    }
proc_write_entry->read_proc = (void *)read_proc ;
proc_write_entry->write_proc = (void *)write_proc;
printk(KERN_INFO "proc initialized\n");
}



int proc_init (void) {
    create_new_proc_entry();
	int pid = task_pid_nr(current);
	int ppid = task_pid_nr(current->parent);
    printk("The Current process id is %d\n", pid);
    printk("The Parent process id is %d\n", ppid); 
	
	///////
	
    return 0;
}

void proc_cleanup(void) {
    printk(KERN_INFO " Inside cleanup_module\n");
    remove_proc_entry("myproc",NULL);
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);
