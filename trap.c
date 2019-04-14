#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;
int mticks;
int total_w;
int tw;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  struct proc *p;
  struct proc *p1;
  

  //char *pstate_string[6] = {"UNUSED  ", "EMBRYO  ", "SLEEPING", "RUNNABLE", "RUNNING ", "ZOMBIE  "};
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit();
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      mticks = ticks *1000;
      p = myproc();
      if(p != 0 && p->state == RUNNING){
        p->runtime++;
      }
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;

  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
    
    
  if(tf->trapno == T_IRQ0+IRQ_TIMER){
  
  	p1 = myproc();
  	
  	
  	
  	
  	
  	if(p1 !=0 && p1->state ==RUNNING){
  	
  	
  		if(total_w !=0){
	        p1->time_slice = 1000 * 10 * (p1->weight)/(total_w);
	        }
  	
  		if(p1->weight !=0){
	  		
  		
  		p1->vruntime = p1->fvruntime + (p1->runtime) * 1000 * 335/(p1->weight);
  		
  		
  			
  		
  		
  		
  		cprintf("fvruntime %d :   %d\n",p1->pid ,p1->fvruntime);
  		/*cprintf("vruntime %d :   %d\n",p1->pid ,p1->vruntime);
  		cprintf("runtime %d :    %d \n",p1->pid ,p1->runtime);
  		cprintf("weight %d :    %d \n",p1->pid ,p1->weight);*/
  		}
  		
  		
  		
  		if(p1->time_slice <= (p1->nruntime) * 1000){
  			total_w = tw;
  			yield();
  		
  		}
  		
  		p1->nruntime++;
  		
  		
  	}
  	
  	
  	
  
  }

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  /* if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();*/

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
}
