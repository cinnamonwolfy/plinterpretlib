/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Source File
\**********************************************************/
#include <pl-interpret.h>

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
