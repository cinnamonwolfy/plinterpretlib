/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Builtins Header File
\**********************************************************/
#pragma once
#include <plinterpret.h>

plarray_t* builtinCmdBuf;

int plIntPrint(plarray_t* args, plmt_t* mt);
int plIntClear(plarray_t* args, plmt_t* mt);
int plIntExit(plarray_t* args, plmt_t* mt);
int plIntSetBuiltinBuf(plmt_t* mt);
