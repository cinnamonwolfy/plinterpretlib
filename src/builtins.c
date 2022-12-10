/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Builtins Source File
\**********************************************************/
#include <plinterpret.h>
#include <plinterpret-builtins.h>

int plIntPrint(plarray_t* args, plmt_t* mt){
	if(args->size < 2)
		return 1;

	for(int i = 1; i < command->size - 1; i++)
		printf("%s ", array[i]);

	printf("%s\n", array[command->size - 1]);
	return 0
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

	exit(strtol(array[0], &pointer, 10));
}

int plIntSetBuiltinBuf(plmt_t* mt){
	plarray_t* tempBuf = plMTAllocE(mt, sizeof(plarray_t));
	tempBuf->array = plMTAllocE(mt, 3 * sizeof(plfunctionptr_t));
	tempBuf->size = 3;
	plfunctionptr_t* holderPtr = tempBuf->array;

	holderPtr[0].function = plIntPrint;
	holderPtr[0].name = "print";
	holderPtr[1].function = plIntClear;
	holderPtr[1].name = "clear";
	holderPtr[2].function = plIntExit;
	holderPtr[2].name = "exit";
}
