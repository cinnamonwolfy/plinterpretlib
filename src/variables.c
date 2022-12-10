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

/* Function that fills out the contents of a variable in a command line */
int plShellVarFill(plarray_t* cmdline, plarray_t* variableBuf){
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
		varStrSize = strlen(startVar);

	char* workVar = plMTAllocE(tempMalloc, varStrSize + 1);

	memcpy(workVar, startVar, varStrSize);
	workVar[varStrSize] = '\0';
	varPos = plShellSearchVarBuf(0, workVar, variableBuf);

	if(varPos == -1){
		printf("%s: Non-existent variable\n", workVar);
		plMTStop(tempMalloc);
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
	char* tempStr = plMTAllocE(cmdline->mt, cmdlineSize + 1);

	if(startVar != array[cmdPos])
		memcpy(tempStr, array[cmdPos], startVar - array[cmdPos]);

	memcpy(tempStr, outputString, outputSize);

	if(endPtr != NULL)
		memcpy(tempStr + startOfCmdline + outputString, endPtr, strlen(endPtr));

	plMTFree(mt, array[cmdPos]);
	array[cmdPos] = tempStr;
	array[cmdPos][sizeOfOut] = 0;

	plMTStop(tempMalloc);
}
