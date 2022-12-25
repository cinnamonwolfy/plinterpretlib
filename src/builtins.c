/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Builtins Source File
\**********************************************************/
#include <plinterpret.h>

int plIntPrint(plarray_t* args, plmt_t* mt){
	if(args->size < 2)
		return 1;

	for(int i = 1; i < args->size - 1; i++)
		printf("%s ", ((char**)args->array)[i]);

	printf("%s\n", ((char**)args->array)[args->size - 1]);
	return 0;
}

int plIntClear(plarray_t* args, plmt_t* mt){
	printf("\033c");
	return 0;
}

int plIntExit(plarray_t* args, plmt_t* mt){
	int tempNum = 0;
	char* pointer;
	if(args->size < 2)
		exit(tempNum);

	exit(strtol(((char**)args->array)[0], &pointer, 10));
}

void plIntExternalSetup(plarray_t* commandBuf){
	commandBuf->array = plMTAllocE(commandBuf->mt, 3 * sizeof(plfunctionptr_t));
	plfunctionptr_t* holderPtr = commandBuf->array;
	size_t hPtrSize = commandBuf->size;

	holderPtr[hPtrSize].function = plIntPrint;
	holderPtr[hPtrSize].name = "print";
	holderPtr[hPtrSize + 1].function = plIntClear;
	holderPtr[hPtrSize + 1].name = "clear";
	holderPtr[hPtrSize + 2].function = plIntExit;
	holderPtr[hPtrSize + 2].name = "exit";

	commandBuf->size += 3;
}
