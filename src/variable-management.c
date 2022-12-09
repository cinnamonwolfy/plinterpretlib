/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Source File
\**********************************************************/
#include <pl-interpret.h>

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
