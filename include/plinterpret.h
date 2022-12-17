/*****************************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Header file
\*****************************************************************/
#pragma once
#include <pl32.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PLSHVAR_NULL 0
#define PLSHVAR_INT 1
#define PLSHVAR_STRING 2
#define PLSHVAR_BOOL 3
#define PLSHVAR_FLOAT 4

typedef struct plvariable {
	void* varptr;
	int type;
	char* name;
	bool isMemAlloc;
} plvariable_t;

typedef struct plfunctionptr {
	int (*function)(plarray_t*, plmt_t*);
	char* name;
} plfunctionptr_t;

typedef struct plint plint_t;

uint8_t plIntCom(plarray_t* cmdline, plarray_t* commandBuf);
uint8_t plInt(char* cmdline, plint_t* shellStats);

#ifdef __cplusplus
}
#endif
