/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Command Executer Source File
\**********************************************************/
#include <plinterpret.h>
#include <plinterpret-builtins.h>

/* Initialization of interpreter frontend */
plint_t* plIntInit(plmt_t* mt, plarray_t* commandBuf){
	plint_t* retStruct = plMTAllocE(mt, sizeof(plint_t));

	/* Add logic here someday... */
}

/* Interpreter frontend */
uint8_t plShell(char* cmdline, plint_t* intStatus){
	if(cmdline == NULL || intStatus == NULL)
		return 1;

	plarray_t* parsedCmdLine = plParser(cmdline, mt);

	if(!parsedCmdLine)
		return 1;

	if(strchr(cmdline, '$') || strchr(cmdline, '=')){
		if(plShellVarMgmt(parsedCmdLine, variableBuf, mt)){
			plMTFreeArray(parsedCmdLine, true, mt);
			return 1;
		}
	}

	char** array = parsedCmdLine->array;
	int retVar = 0;

	if(!cmdlineIsNotCommand){
		if(strcmp(array[0], "version") == 0 || strcmp(array[0], "help") == 0){
			printf("%s\n", productString);
			if(strlen(srcUrlString) > 0)
				printf("src at %s\n", srcUrlString);

			printf("\npl32lib v%s, (c)2022 pocketlinux32, Under LGPLv2.1 or later\n", PL32LIB_VERSION);
			printf("src at https://github.com/pocketlinux32/pl32lib\n");
			if(strcmp(array[0], "help") == 0){
				printf("Built-in commands: print, clear, exit, show-memusg, version, help\n");
				if(!commandBuf || commandBuf->size == 0){
					printf("No user-defined commands loaded\n");
				}else{
					printf("%ld user-defined commands loaded\n", commandBuf->size);
					printf("User-defined commands: ");
					for(int i = 0; i < commandBuf->size - 1; i++)
						printf("%s, ", ((plfunctionptr_t*)commandBuf->array)[i].name);

					printf("%s\n", ((plfunctionptr_t*)commandBuf->array)[commandBuf->size - 1].name);
				}
			}
		}else if(strcmp(array[0], "show-memusg") == 0){
			printf("%ld bytes free\n", plMTMemAmnt(mt, PLMT_GET_MAXMEM, 0) -  plMTMemAmnt(mt, PLMT_GET_USEDMEM, 0));
		}else{
			retVar = plShellComInt(parsedCmdLine, commandBuf, mt);
		}
	}

	plMTFreeArray(parsedCmdLine, true, mt);
	return retVar;
}


// Interactive frontend to plShellFrontEnd()
void plShellInteractive(char* prompt, bool showHelpAtStart, plarray_t* variableBuf, plarray_t* commandBuf, plmt_t* shellMT){
	bool loop = true;
	bool showRetVal = false;

	if(!shellMT)
		shellMT = plMTInit(8 * 1024 * 1024);

	if(!prompt)
		prompt = "(cmd) # ";

	if(showHelpAtStart){
		plShell("help", variableBuf, commandBuf, &shellMT);
	}

	plShell("show-memusg", variableBuf, commandBuf, &shellMT);
	while(loop){
		char cmdline[4096] = "";
		int retVal = 0;
		printf("%s", prompt);
		scanf("%4095[^\n]", cmdline);
		getchar();

		if(strcmp(cmdline, "exit-shell") == 0 || feof(stdin)){
			loop = false;
		}else if(strcmp(cmdline, "show-exitval") == 0){
			if(showRetVal){
				showRetVal = false;
			}else{
				showRetVal = true;
			}
		}else if(strcmp(cmdline, "reset-mem") == 0){
			size_t size = plMTMemAmnt(shellMT, PLMT_GET_MAXMEM, 0);
			plMTStop(shellMT);
			shellMT = plMTInit(size);
			variableBuf = NULL;
			commandBuf = NULL;
			printf("Memory has been reset\n");
		}else if(strlen(cmdline) > 0){
			retVal = plShell(cmdline, variableBuf, commandBuf, &shellMT);
		}

		if(showRetVal)
			printf("\nretVal = %d\n", retVal);

	}

	if(feof(stdin))
		printf("\n");

	plMTStop(shellMT);
}

