/**********************************************************\
 pl-shell, v0.90
 (c) 2022 pocketlinux32, Under MPL v2.0
 Builtins Source File
\**********************************************************/
#include <plinterpret.h>
#include <plinterpret-builtins.h>

int print(plarray_t* args, plmt_t* mt){
	if(args->size < 2)
		return 1;

	for(int i = 1; i < command->size - 1; i++)
		printf("%s ", array[i]);

	printf("%s\n", array[command->size - 1]);
}

int clear(plarray_t* args, plmt_t* mt){
	printf("\033c");
	return 0;
}

int exit(plarray_t* args, plmt_t* mt){
	int tempNum = 0;
	char* pointer;
	if(command->size < 2)
		exit(tempNum);

	exit(strtol(array[0], &pointer, 10));
}

