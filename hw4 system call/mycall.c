#include <linux/kernel.h>
asmlinkage long sys_mycall(char* msg)
{
  printk("%.*s\n", 100, msg);
  return 0;
}

