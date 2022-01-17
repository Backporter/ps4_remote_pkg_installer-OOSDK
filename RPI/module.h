#pragma once

#include "common.h"

#include <orbis/libkernel.h>

#define DLSYM_MANGLED_NAME 0x1


int32_t sceKernelLoadStartModuleFromSandbox(const char* name, size_t args, const void* argp, unsigned int flags, const OrbisKernelEventFlagOptParam* opts, int* res);

// int sceKernelGetModuleInfo(int32_t handle, OrbisKernelModuleInfo* info);
// int sceKernelGetModuleInfoEx(int32_t handle, OrbisKernelModuleInfo* info);

int sceKernelGetModuleInfoByName(const char* name, OrbisKernelModuleInfo* info);
int sceKernelGetModuleInfoExByName(const char* name, OrbisKernelModuleInfo* info);

int sceKernelDlsymEx(int32_t handle, const char* symbol, const char* lib, unsigned int flags, void** addrp);

bool get_module_base(const char* name, uint64_t* base, uint64_t* size);

typedef void module_patch_cb_t(void* arg, uint8_t* base, uint64_t size);
bool patch_module(const char* name, module_patch_cb_t* cb, void* arg);
