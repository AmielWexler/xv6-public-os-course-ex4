#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "processInfo.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


int
sys_getNumProc(void)
{
    int count = 0;
    struct proc *p;
    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state != UNUSED)
            count++;
    }
    release(&ptable.lock);
    return count;
}

int
sys_getMaxPid(void)
{
    int max_pid = 0;
    struct proc *p;
    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->state != UNUSED && p->pid > max_pid)
            max_pid = p->pid;
    }
    release(&ptable.lock);
    return max_pid;
}

int
sys_getProcInfo(void)
{
    int pid;
    struct processInfo *pinfo;

    if (argint(0, &pid) < 0 || argptr(1, (char **)&pinfo, sizeof(*pinfo)) < 0)
        return -1;

    struct proc *p;
    acquire(&ptable.lock);
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
        if (p->pid == pid) {
            pinfo->state = p->state;
            pinfo->ppid = p->parent ? p->parent->pid : 0;
            pinfo->sz = p->sz;
            pinfo->nfd = 0; // Calculate number of open FDs (iterate over p->ofile[])
            pinfo->nrswitch = p->ncs; // Update p->ncs as part of scheduler logic
            release(&ptable.lock);
            return 0;
        }
    }
    release(&ptable.lock);
    return -1; // PID not found
}
