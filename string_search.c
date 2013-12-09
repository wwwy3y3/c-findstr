#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h> 
#include <pthread.h>

struct threadData
{
	char *searchStr; //str we search
	char *str; //str we search in `searchStr` for
	int *count; //how many match we find
};

int sliceStr(char *str, char *fileStr, long file_size){
	//slice by half
	int mid= file_size/2;
	char *ptr= &fileStr[mid];
	int i=0,test=0;
	// if char not in str, then return the index
	// if in str, then check in sequence
	char* pch=strchr(str, ptr[i]);
	while(pch!=NULL){ //char is in str
		//check if char in middle of string we want to find
		test= strncmp(pch, &ptr[i], strlen(pch));
		if(test == 0){
			//we're in mid of str, so skip it!
			i += strlen(pch);
			pch=strchr(str, ptr[i]);
		}else{
			//break loop, since we're not in middle of string
			break;
		}
	}
	//return index
	return mid+i;
}

void runRegex(char *str, char *searchStr, int* count){
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
}

void *regexFind(void *context){
	struct threadData *tdata = context;
	char *str= tdata->str;
	char *searchStr= tdata->searchStr;
	int *count= tdata->count;
	runRegex(str,searchStr,count);
    return NULL;
}

int main(int argc, char const *argv[])
{
	
	char str[100];
	int x=0,y=0,sum; //count for two threads
	//thread
	pthread_t inc_x_thread;
	struct threadData tdata;
	char *file_contents;// origin file content
	long input_file_size; //file size

	//copy filename to string
	strcpy(str, argv[1]);

	//put file text to a string
	FILE *fp = fopen("oslab.txt", "r");
	fseek(fp, 0, SEEK_END);
	input_file_size = ftell(fp);
	rewind(fp);
	file_contents = malloc(input_file_size * (sizeof(char)));
	fread(file_contents, sizeof(char), input_file_size, fp);
	fclose(fp);

	/*
	*	slice the file text
	*/
	int sliceIdx= sliceStr(str, file_contents, input_file_size);
	char *mid= &file_contents[sliceIdx];
	char *first= malloc(sliceIdx+1 * (sizeof(char)));
	strncpy(first, file_contents, sliceIdx);

	/*
	*	exec first part of text in thread
	*/
	tdata.str=str;
	tdata.searchStr=first;
	tdata.count=&x;
	if(pthread_create(&inc_x_thread, NULL, regexFind, &tdata)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

	//run regex
	runRegex(str,mid,&y);

	//thread join
	if(pthread_join(inc_x_thread, NULL)) {
		fprintf(stderr, "Error joining thread\n");
		return 2;
	}

	//sum two counts
	sum= x+y;
	printf("total found: %d\n", sum);
	return 0;
}