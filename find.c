#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h> 
#include <pthread.h>

struct threadData
{
	char *searchStr;
	char *str;
	int *count;
};

int sliceStr(char *str, char *fileStr, long file_size){
	int mid= file_size/2;
	char *ptr= &fileStr[mid];
	int i=0;
	//printf("%s\n", ptr);
	// if char not in str, then return the index
	// if in str, then check in sequence
	char* pch=strchr(str, ptr[i]);
	while(pch!=NULL){
		i++;
		pch=strchr(str, ptr[i]);
	}
	//return index
	return mid+i;
}

void *regexFind(void *context){
	struct threadData *tdata = context;
	char *str= tdata->str;
	char *searchStr= tdata->searchStr;
	int *count= tdata->count;

	regex_t re;
	regmatch_t match;

	if(regcomp(&re, str, REG_EXTENDED) != 0) exit(1);
	 
	while(regexec(&re, searchStr, 1, &match, 0) == 0)
	{
		int len= (int)(match.rm_eo - match.rm_so);
		printf("%.*s\n", len, &searchStr[match.rm_so]);
		searchStr += match.rm_eo-len+1; /* or p = &p[match.rm_eo]; */
		(*count)++;
		//printf("%c\n", mid[0]);
	}
	//printf("%s\n", first);
    regfree(&re);
    return NULL;
}

int main(int argc, char const *argv[])
{
	//copy filename to string
	char str[100];
	strcpy(str, argv[1]);

	//put file text to a string
	char *file_contents;
	long input_file_size;
	size_t code_s, result;
	FILE *fp = fopen("overlap.txt", "r");
	fseek(fp, 0, SEEK_END);
	input_file_size = ftell(fp);
	rewind(fp);
	file_contents = malloc(input_file_size * (sizeof(char)));
	fread(file_contents, sizeof(char), input_file_size, fp);
	//printf("%s\n", file_contents);
	fclose(fp);

	//slice
	int sliceIdx= sliceStr(str, file_contents, input_file_size);
	char *mid= &file_contents[sliceIdx];
	char *first= malloc(input_file_size-sliceIdx * (sizeof(char)));
	strncpy(first, file_contents, sliceIdx);

	//thread
	int x=0,y=0;
	pthread_t inc_x_thread;
	struct threadData tdata;
	tdata.str=str;
	tdata.searchStr=first;
	tdata.count=&x;
	if(pthread_create(&inc_x_thread, NULL, regexFind, &tdata)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	//run regex
	regex_t re;
	regmatch_t match;

	if(regcomp(&re, str, REG_EXTENDED) != 0) exit(1);
	 
	while(regexec(&re, mid, 1, &match, 0) == 0)
	{
		int len= (int)(match.rm_eo - match.rm_so);
		printf("%.*s\n", len, &mid[match.rm_so]);
		mid += match.rm_eo-len+1; /* or p = &p[match.rm_eo]; */
		y++;
		//printf("%c\n", mid[0]);
	}
	regfree(&re);

	//thread join
	if(pthread_join(inc_x_thread, NULL)) {
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}
	int sum= x+y;
	printf("total found: %d\n", sum);
	return 0;
}