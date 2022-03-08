#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if (argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if (argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;

  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

#ifdef LAB_PGTBL
int sys_pgaccess(void)
{
  printf("start pgaccess!\n");
  // lab pgtbl: your code here.

  // Get the parameters
  uint64 va;
  if (argaddr(0, &va) < 0)
    return -1;
  int num_page;
  if (argint(1, &num_page) < 0)
    return -1;
  uint64 return_address;
  if (argaddr(2, &return_address) < 0)
    return -1;

  // buffer stores temporay access bits
  int num_byte;
  if (num_page % 8 != 0)
  {
    num_byte = num_page / 8 + 1;
  }
  else
  {
    num_byte = num_page / 8;
  }
  char buf[num_byte];
  for (int i = 0; i < num_byte; i++)
  {
    buf[i] = '\0';
  }

  for (int i = 0; i < num_page; i++)
  {
    // printf("calcuate addr\n");
    uint64 addr = va + i * PGSIZE;
    pagetable_t page = myproc()->pagetable;
    // modified from walk()
    // printf("start walking\n");
    for (int level = 2; level > 0; level--)
    {
      // printf("walk at level %d\n", level);
      pte_t *pte = &page[PX(level, addr)];
      if (*pte & PTE_V)
      {
        page = (pagetable_t)PTE2PA(*pte);
      }
      else
      {
        continue;
      }
    }

    // check access bit
    if ((page[PX(0, addr)] & PTE_V) && (page[PX(0, addr)] & PTE_A))
    {
      printf("access bit %d\n", i);
      int d, r = i % 8;
      if (r == 0)
      {
        d = i / 8 + 1;
      }
      else
      {
        d = i / 8;
      }
      buf[d] = buf[d] | (1 << r);
      page[PX(0, addr)] = page[PX(0, addr)] & (~PTE_A); // turn off access bit
    }
  }

  uint32 tem = (int)*buf;
  printf("%x \n", tem);

  if (copyout(myproc()->pagetable, return_address, buf, num_byte) < 0)
    return -1;
  printf("done\n");
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
