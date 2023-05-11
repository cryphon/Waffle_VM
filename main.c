#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VM_STACK_CAP 1024

typedef enum {
  TRAP_OK,
  TRAP_STACK_OVERFLOW,
  TRAP_STACK_UNDERFLOW,
  TRAP_ILLEGAL_INST
} trap;

typedef int64_t inst_word;

// virtual machine
typedef struct {
  inst_word stack[VM_STACK_CAP];
  size_t stack_size;
} machine;

// instructions type
typedef enum {
  INST_PUSH,
  INST_ADD,
  INST_SUB,
  INST_EQ,
  INST_READ_PTR
} inst_type;

// instruction
typedef struct {
  inst_type type;
  inst_word operand;
} instruction;

instruction inst_push(inst_word operand) {
  return (instruction){.type = INST_PUSH, .operand = operand};
}

instruction inst_add(void) { return (instruction){.type = INST_ADD}; }

instruction inst_sub(void) { return (instruction){.type = INST_SUB}; }

instruction inst_eq(void) { return (instruction){.type = INST_EQ}; }

instruction inst_read_previous_ptr(void) {
  return (instruction){.type = INST_READ_PTR};
}
machine vm = {0};

trap exec_vm_inst(machine *vm, instruction inst) {
  switch (inst.type) {
  case INST_PUSH:
    if (vm->stack_size >= VM_STACK_CAP) {
      return TRAP_STACK_OVERFLOW;
    }
    vm->stack[vm->stack_size++] = inst.operand;
    break;
  case INST_ADD:
    if (vm->stack_size < 2) {
      return TRAP_STACK_UNDERFLOW;
    }
    vm->stack[vm->stack_size - 2] += vm->stack[vm->stack_size - 1];
    vm->stack_size -= 1;
    break;
  case INST_SUB:
    if (vm->stack_size < 2) {
      return TRAP_STACK_UNDERFLOW;
    }
    vm->stack[vm->stack_size - 2] -= vm->stack[vm->stack_size - 1];
    vm->stack_size -= 1;

  case INST_EQ:
    if (vm->stack_size < 2) {
      return TRAP_STACK_UNDERFLOW;
    }

    // check for conditional ==
    if (vm->stack[vm->stack_size - 2] == vm->stack[vm->stack_size - 1]) {
      vm->stack[vm->stack_size++] = 1;
    } else {
      vm->stack[vm->stack_size++] = 0;
    }
  case INST_READ_PTR:
    if (vm->stack_size < 1) {
      return TRAP_STACK_UNDERFLOW;
    }

    inst_word *ptr = &(vm->stack[vm->stack_size - 1]);
    printf("prev pointer address: %p\n", &ptr);
    printf("prev pointer val: %lu", *ptr);
    vm->stack_size -= 1;
  default:
    return TRAP_ILLEGAL_INST;
    break;
  }
  return TRAP_OK;
}

void dump_machine_regs(const machine *vm) {
  printf("\nStack:\n");
  for (size_t i = 0; i < vm->stack_size; i++) {
    printf("   %ld\n", vm->stack[i]);
  }
}

void program(machine *vm) {
  exec_vm_inst(vm, inst_push(10));
  exec_vm_inst(vm, inst_read_previous_ptr());
  exec_vm_inst(vm, inst_push(10));
  exec_vm_inst(vm, inst_eq());
}

int main(void) {
  // insert instructions
  program(&vm);

  // view stack
  dump_machine_regs(&vm);
  return 0;
}
