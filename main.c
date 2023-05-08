#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define VM_STACK_CAP 1024

typedef enum {
  TRAP_OK,
  TRAP_STACK_OVERFLOW,
  TRAP_STACK_UNDERFLOW,
  TRAP_ILLEGAL_INST
} trap;


typedef int64_t inst_word;



//virtual machine
typedef struct {
  inst_word stack[VM_STACK_CAP];
  size_t stack_size;
} machine;


// instructions type
typedef enum {
  INST_PUSH,
  INST_ADD,
  INST_SUB
} inst_type;



//instruction
typedef struct {
  inst_type type;
  inst_word operand;
} instruction;



instruction inst_push(inst_word operand) {
  return (instruction) {
    .type = INST_PUSH,
    .operand = operand
  };
}

instruction inst_add(void){
  return (instruction) {
    .type = INST_ADD
  };
}


instruction inst_sub(void) {
  return (instruction) {
    .type = INST_SUB
  };
}



machine vm = {0};

trap exec_vm_inst(machine *vm, instruction inst) {
  switch(inst.type){
    case INST_PUSH:
      if(vm->stack_size >= VM_STACK_CAP) {
        return TRAP_STACK_OVERFLOW;
      }
      vm->stack[vm->stack_size++] = inst.operand;
      break;
    case INST_ADD:
      if(vm->stack_size < 2) {
        return TRAP_STACK_UNDERFLOW;
      }
      vm->stack[vm->stack_size - 2] += vm->stack[vm->stack_size - 1];
      vm->stack_size -= 1;
      break;
    case INST_SUB:
      if(vm->stack_size < 2) {
        return TRAP_STACK_UNDERFLOW;
      }
    vm->stack[vm->stack_size - 2] -= vm->stack[vm->stack_size - 1];
    vm->stack_size -= 1;
    default:
      return TRAP_ILLEGAL_INST;
      break;
  }
  return TRAP_OK;
}

void dump_machine_regs(const machine *vm) {
  printf("Stack:\n");
  for(size_t i = 0; i < vm->stack_size; i++) {
    printf("   %ld\n", vm->stack[i]);
  }
}


void program(machine *vm){
  exec_vm_inst(vm, inst_push(10));
  exec_vm_inst(vm, inst_push(40));
  exec_vm_inst(vm, inst_sub());
}

int main(int argc, char* **argv) {
  //insert instructions
  program(&vm);

  //view stack
  dump_machine_regs(&vm);
  return 0;
}
