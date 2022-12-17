/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Command Executer Source File
\**********************************************************/
#include <plinterpret.h>
#include <plinterpret-builtins.h>

struct plint {
	plarray_t* commandBuffer;
	plarray_t* variableBuffer;
	plarray_t* blockBuffers;
	uint16_t blockStack;
	plmt_t* memTrack;
}

/* Initialization of interpreter frontend */
plint_t* plIntInit(plmt_t* mt, plarray_t* commandBuf){
	plint_t* retStruct = plMTAllocE(mt, sizeof(plint_t));

	/* Add logic here someday... */
}

/* Interpreter frontend */
uint8_t plInt(char* cmdline, plint_t* intStatus){
	if(cmdline == NULL || intStatus == NULL)
		return 1;

	plarray_t* parsedCmdLine = plParser(cmdline, intStatus->memTrack);

	if(parsedCmdLine == NULL)
		return 1;

	char** array = parsedCmdLine->array;
	int retVar = 0;

	if(strcmp(array[0], "version") == 0 || strcmp(array[0], "help") == 0){
		printf("plinterpretlib v%s, (c)2022 pocketlinux32, Under MPL v2\n", PLINT_VERSION);
		printf("src at https://github.com/pocketlinux32/plinterpretlib")
		printf("pl32lib-ng v%s, (c)2022 pocketlinux32, Under MPL v2\n", PL32LIB_VERSION);
		printf("src at https://github.com/pocketlinux32/pl32lib-ng\n");
		if(strcmp(array[0], "help") == 0){
			if(!commandBuf || commandBuf->size == 0){
				printf("No commands loaded\n");
			}else{
				printf("%ld commands loaded\n", intStatus->commandBuf->size);
				printf("Commands: ");
				for(int i = 0; i < intStatus->commandBuf->size - 1; i++)
					printf("%s, ", ((plfunctionptr_t*)intStatus->commandBuf->array)[i].name);

				printf("%s\n", ((plfunctionptr_t*)intStatus->commandBuf->array)[intStatus->commandBuf->size - 1].name);
			}
		}
	}else{
		retVar = plIntComInt(parsedCmdLine, commandBuf, mt);
	}

	plMTFreeArray(parsedCmdLine, true, mt);
	return retVar;
}
