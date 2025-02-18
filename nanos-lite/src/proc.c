#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);
int current_game = 2;
void switch_current_game() {
  current_game = 2-current_game;
  Log("current_game=%d",current_game);
}
void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);
  // TODO: remove the following three lines after you have implemented _umake()
  //_switch(&pcb[i].as);
  //current = &pcb[i];
  //((void (*)(void))entry)();
  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);
  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);

}

_RegSet* schedule(_RegSet *prev) {
  if(current != NULL) 
    current->tf = prev;
  else
    current = &pcb[current_game];
  //current = &pcb[0];
  static int num = 0;
  static const int frequency = 100000;
  if(current == &pcb[current_game])
    num++;
  else
    current = &pcb[current_game];
  if(num >= frequency) {
    current = &pcb[0];
    num = 0;
  }
  //Log("ptr=0x%x\n",(uint32_t)current->as.ptr);
  _switch(&current->as);
  return current->tf;
}
