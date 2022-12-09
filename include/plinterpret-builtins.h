/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Builtins Header File
\**********************************************************/
#pragma once
#include <plinterpret.h>

int print(plarray_t* args, plmt_t* mt);
int clear(plarray_t* args, plmt_t* mt);
int exit(plarray_t* args, plmt_t* mt);
