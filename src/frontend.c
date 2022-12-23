/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Command Executer Source File
\**********************************************************/
#include <plinterpret.h>
#ifdef win32
	#include <windows.h>
	#define loadExternalLib(a) LoadLibrary(TEXT(a))
	#define getExternalLibSymbol GetProcAddress
#else
	#include <dlfcn.h>
	#define loadExternalLib(a) dlopen(a, RTLD_LAZY)
	#define getExternalLibSymbol dlsym
#endif

struct plint {
	plarray_t* commandBuffer;
	plarray_t* variableBuffer;
	plarray_t* blockBuffers;
	uint16_t blockStack;
	plmt_t* memTrack;
};

/* Initialization of interpreter frontend */
plint_t* plIntInit(plmt_t* memTrack, plarray_t* commandBuffer){
	plint_t* retStruct = plMTAllocE(memTrack, sizeof(plint_t));

	retStruct->memTrack = memTrack;
	retStruct->commandBuffer = commandBuffer;

	return retStruct;
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
		printf("src at https://github.com/pocketlinux32/plinterpretlib\n");
		printf("pl32lib-ng v%s, (c)2022 pocketlinux32, Under MPL v2\n", PL32LIBNG_VERSION);
		printf("src at https://github.com/pocketlinux32/pl32lib-ng\n");
		if(strcmp(array[0], "help") == 0){
			printf("Built-in commands: version, help, load\n");
			if(intStatus->commandBuffer == NULL || intStatus->commandBuffer->size == 0){
				printf("No external commands loaded\n");
			}else{
				printf("%ld external commands loaded\n", intStatus->commandBuffer->size);
				printf("Commands: ");
				for(int i = 0; i < intStatus->commandBuffer->size - 1; i++)
					printf("%s, ", ((plfunctionptr_t*)intStatus->commandBuffer->array)[i].name);

				printf("%s\n", ((plfunctionptr_t*)intStatus->commandBuffer->array)[intStatus->commandBuffer->size - 1].name);
			}
		}
	}else if(strcmp(array[0], "load") == 0){
		printf("This is a work in progress. Please try again at a later time\n");
		return 1;

		void** loadedLibrary = loadExternalLib(array[1]);
		void** libSetup;
	}else{
		retVar = plIntCommandExec(parsedCmdLine, intStatus->commandBuffer, intStatus->memTrack);
	}

	plMTFreeArray(parsedCmdLine, true);
	return retVar;
}
