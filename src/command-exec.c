/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Command Executer Source File
\**********************************************************/
#include <plinterpret.h>

/* Internal command executer for plIntCommandExec */
uint8_t plIntExec(plarray_t* command, plarray_t* commandBuf, plmt_t* mt){
	int retVar = 0;
	int i = 0;
	bool commandBufExists = (commandBuf != NULL && commandBuf->size > 0);
	char** array = command->array;

	while(commandBufExists && strcmp(((plfunctionptr_t*)commandBuf->array)[i].name, array[0]) != 0 && i < commandBuf->size - 1){
		i++;
	}

	if(!commandBufExists || strcmp(((plfunctionptr_t*)commandBuf->array)[i].name, array[0]) != 0){
		return 255;
	}else{
		retVar = ((plfunctionptr_t*)commandBuf->array)[i].function(command, mt);
	}

	return retVar;
}

/* Command Executer */
uint8_t plIntCommandExec(plarray_t* command, plarray_t* commandBuf, plmt_t* mt){
	if(command == NULL)
		return 1;

	char** array = command->array;
	int retVar = 0;

	/* It tries to run a command in the command buffer. If it can't find the program*/
	retVar = plIntExec(command, commandBuf, mt);
	if(retVar == 255)
		printf("%s: command not found\n", array[0]);

	return retVar;
}
