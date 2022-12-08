/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Source File
\**********************************************************/
#include <pl32-shell.h>

char productString[4096] = "PocketLinux Shell, (c)2022 pocketlinux32";
char srcUrlString[4096] = "";

void setProductStrings(char* productStr, char* srcUrl){
	strcpy(productString, productStr);
	if(srcUrl)
		strcpy(srcUrlString, srcUrl);
}

/* Internal function to search a variable with a pointer or a name */
long plShellVarSearchBuf(int opt, char* stringOrPtr, plarray_t* variableBuf){
	long retVar = 0;
	plvariable_t* workVarBuf = variableBuf->array;

	if(opt == 0){
		/* Search with a name */
		while(retVar < variableBuf->size && strcmp(workVarBuf[retVar].name, stringOrPtr) != 0)
			retVar++;
	}else{
		/* Search with a pointer */
		while(retVar < variableBuf->size && workVarBuf[retVar].varptr != stringOrPtr)
			retVar++;
	}

	if(retVar == variableBuf->size)
		return -1;

	return retVar;
}

void plShellVarFill(plarray_t* cmdline, plarray_t* variableBuf){
	int cmdPos = 0, varPos = -1;
	char** array = cmdline->array;
	plvariable_t* workVarBuf = variableBuf->array;
	void* valToExpand = NULL;
	char holderString[1024] = "";
	char* outputString = NULL;
	plmt_t* tempMalloc = plMTInit(8192);

	while(cmdPos < cmdline->size && strchr(array[cmdPos], '$') == NULL)
		cmdPos++;

	char* startVar = strchr(array[cmdPos], '$') + 1;
	char* endVar = strchr(startVar, ' ');
	size_t varStrSize = endVar - startVar;
	if(endVar == NULL)
		strSize = strlen(startVar);
	char* workVar = plMTAlloc(tempMalloc, strSize + 1);

	if(workVar == NULL){
		plMTStop(tempMalloc);
		return 1;
	}

	memcpy(workVar, startVar, strSize);
	workVar[strSize] = '\0';
	varPos = plShellSearchVarBuf(0, workVar, variableBuf);

	if(varPos == -1){
		printf("%s: Non-existent variable\n", workVar);
		return 255;
	}

	valToExpand = workVarBuf[varPos].varptr;

	switch(workVarBuf[varPos].type){
		case PLSHVAR_INT: ;
			snprintf(holderString, 1023, "%d", *((int*)valToExpand));
			break;
		case PLSHVAR_STRING: ;
			outputString = (char*)valToExpand;
			break;
		case PLSHVAR_BOOL: ;
			bool tempBool = *((bool*)valToExpand);

			if(tempBool)
				strcpy(holderString, "true");
			else
				strcpy(holderString, "false");
			break;
		case PLSHVAR_FLOAT: ;
			snprintf(holderString, 1023, "%f", *((float*)valToExpand));
			break;
	}

	if(!outputString)
		outputString = holderString;

	size_t outputSize = strlen(outputString);
	size_t cmdlineSize = (strlen(array[cmdPos]) - varStrSize) + outputSize;
	char* tempStr = plMTAlloc(cmdline->mt, cmdlineSize + 1);

	if(tempStr == NULL){
		plMTStop(tempMalloc);
		return 1;
	}

	plMTFree(mt, array[cmdPos]);
	array[cmdPos] = tempStr
	memcpy(array[cmdPos], outputString, sizeOfOut);
	array[cmdPos][sizeOfOut] = 0;

	plMTStop(tempMalloc);
}

/* Internal function to manage variables */
uint8_t plShellVarMgmt(plarray_t* cmdline, plarray_t* variableBuf, plmt_t* mt){
	if(!mt || !cmdline || !cmdlineIsNotCommand || !variableBuf)
		return 255;

	char** array = cmdline->array;
	int assignVal = -1;
	plvariable_t* workVarBuf = variableBuf->array;

	if(strchr(array[0], '=') != NULL || (cmdline->size > 1 && strchr(array[1], '=') == array[1])){
		if(strchr(array[0], '=') != NULL)
			assignVal = 0;
		else if(strchr(array[1], '=') == array[1])
			assignVal = 2;
		else
			assignVal = 1;
	}


	if(assignVal > -1){
		char* varToAssign = NULL;
		char* valToAssign = NULL;
		char* workVar = strchr(array[0], '=');
		int valType = PLSHVAR_NULL;
		int assignVarPos = 0;
		bool isMemAlloc = true;

		if(assignVal < 2 && workVar){
			size_t sizeToCopy = workVar - array[0];
			varToAssign = plMTAlloc(mt, sizeToCopy + 1);
			memcpy(varToAssign, array[0], sizeToCopy);
			varToAssign[sizeToCopy] = 0;
		}else{
			varToAssign = array[0];
		}

		assignVarPos = plShellSearchVarBuf(0, varToAssign, variableBuf);

		if(assignVarPos == -1){
			int nullPos = plShellSearchVarBuf(1, NULL, variableBuf);

			if(nullPos == -1 && !variableBuf->isMemAlloc){
				printf("plShellVarMgmt: Variable buffer is full\n");
				return ENOMEM;
			}else if(nullPos == -1){
				void* tempPtr = plMTRealloc(mt, variableBuf->array, (variableBuf->size + 1) * sizeof(plvariable_t));
				if(!tempPtr)
					return ENOMEM;

				nullPos = variableBuf->size;
				variableBuf->array = tempPtr;
				variableBuf->size++;
				workVarBuf = variableBuf->array;
				workVarBuf[nullPos].varptr = NULL;
				workVarBuf[nullPos].name = NULL;
			}

			assignVarPos = nullPos;
		}

		if(varPos == -1){
			switch(assignVal){
				case 0: ;
					size_t sizeToCopy = (array[0] + strlen(array[0])) - workVar;
					valToAssign = plMTAlloc(mt, sizeToCopy + 1);
					memcpy(valToAssign, workVar + 1, sizeToCopy);
					valToAssign[sizeToCopy] = 0;
					break;
				case 1: ;
					if(workVar){
						valToAssign = array[1];
					}else{
						size_t sizeToCopy = strlen(array[1]) - 1;
						valToAssign = plMTAlloc(mt, sizeToCopy + 1);
						memcpy(valToAssign, workVar + 1, sizeToCopy);
						valToAssign[sizeToCopy] = 0;
					}
					break;
				case 2: ;
					valToAssign = array[2];
					break;
			}
		}else{
			valToAssign = workVarBuf[varPos].varptr;
		}

		if(varPos == -1){
			char* leftoverStr = NULL;
			int number;
			bool boolean;
			float decimal;
			void* pointer;

			number = strtol(valToAssign, &leftoverStr, 10);
			if(leftoverStr != NULL && *leftoverStr != '\0'){
				decimal = strtof(valToAssign, &leftoverStr);
				if(leftoverStr != NULL && *leftoverStr != '\0'){
					if(strcmp("true", valToAssign) == 0){
						boolean = true;
						if(!(pointer = plMTAlloc(mt, sizeof(bool)))){
							printf("plShellVarMgmt: Out of memory\n");
							return ENOMEM;
						}

						*((bool*)pointer) = boolean;
						valType = PLSHVAR_BOOL;
					}else if(strcmp("false", valToAssign) == 0){
						boolean = false;
						if(!(pointer = plMTAlloc(mt, sizeof(bool)))){
							printf("plShellVarMgmt: Out of memory\n");
							return ENOMEM;
						}

						*((bool*)pointer) = boolean;
						valType = PLSHVAR_BOOL;
					}else{
						if(!(pointer = plMTAlloc(mt, (strlen(valToAssign) + 1) * sizeof(char)))){
							printf("plShellVarMgmt: Out of memory\n");
							return ENOMEM;
						}

						strcpy(pointer, valToAssign);
						valType = PLSHVAR_STRING;
					}
				}else{
					if(!(pointer = plMTAlloc(mt, sizeof(float)))){
						printf("plShellVarMgmt: Out of memory\n");
						return ENOMEM;
					}

					*((float*)pointer) = decimal;
					valType = PLSHVAR_FLOAT;
				}
			}else{
				if(!(pointer = plMTAlloc(mt, sizeof(int)))){
					printf("plShellVarMgmt: Out of memory\n");
					return ENOMEM;
				}

				*((int*)pointer) = number;
				valType = PLSHVAR_INT;
			}
			valToAssign = pointer;
		}else{
			valType = workVarBuf[varPos].type;
		}

		if(workVarBuf[assignVarPos].varptr != NULL && workVarBuf[assignVarPos].isMemAlloc){
			plMTFree(mt, workVarBuf[assignVarPos].varptr);
		}else{
			char* name = plMTAlloc(mt, strlen(varToAssign));
			if(!name){
				printf("plShellVarMgmt: Out of memory\n");
				return ENOMEM;
			}

			workVarBuf[assignVarPos].name = name;
			strcpy(workVarBuf[assignVarPos].name, varToAssign);
		}

		workVarBuf[assignVarPos].varptr = valToAssign;
		workVarBuf[assignVarPos].type = valType;
		workVarBuf[assignVarPos].isMemAlloc = true;
	}

	return 0;
}

/* Internal function to interpret commands */
uint8_t plShellComInt(plarray_t* command, plarray_t* commandBuf, plmt_t* mt){
	if(!mt || !command)
		return 1;

	char** array = command->array;
	int retVar = 0;

	/* Checks if command are either of the 3 built-ins. If not, it executes an externally defined command */
	if(strcmp(array[0], "print") == 0){
		if(command->size < 2)
			return 1;

		for(int i = 1; i < command->size - 1; i++)
			printf("%s ", array[i]);

		printf("%s\n", array[command->size - 1]);
	}else if(strcmp(array[0], "clear") == 0){
		printf("\033c");
	}else if(strcmp(array[0], "exit") == 0){
		int tempNum = 0;
		char* pointer;
		if(command->size < 2)
			exit(tempNum);

		exit(strtol(array[0], &pointer, 10));
	}else{
		int i = 0;

		if(!commandBuf || commandBuf->size == 0){
			printf("%s: command not found\n", array[0]);
			return 255;
		}

		while(strcmp(((plfunctionptr_t*)commandBuf->array)[i].name, array[0]) != 0 && i < commandBuf->size - 1){
			i++;
		}

		if(strcmp(((plfunctionptr_t*)commandBuf->array)[i].name, array[0]) != 0){
			printf("%s: command not found\n", array[0]);
			return 255;
		}else{
			retVar = ((plfunctionptr_t*)commandBuf->array)[i].function(command, mt);
		}
	}

	return retVar;
}

/* Interpreter frontend */
uint8_t plShell(char* cmdline, plarray_t* variableBuf, plarray_t* commandBuf, plmt_t* mt){
	if(!mt)
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
