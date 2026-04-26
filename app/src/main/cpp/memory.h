#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

int get_pid(const char* process_name);
uintptr_t get_module_base(int pid, const char* module_name);
bool write_mem(int pid, uintptr_t addr, void* buffer, size_t size);
bool read_mem(int pid, uintptr_t addr, void* buffer, size_t size);

#endif
