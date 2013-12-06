#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>  

int main(int argc, char const *argv[])
{
	//copy filename to string
	char str[100];
	strcpy(str, argv[1]);

	//put file text to a string
	char *file_contents;
	long input_file_size;
	size_t code_s, result;
	FILE *fp = fopen("oslab.txt", "r");
	fseek(fp, 0, SEEK_END);
	input_file_size = ftell(fp);
	rewind(fp);
	file_contents = malloc(input_file_size * (sizeof(char)));
	fread(file_contents, sizeof(char), input_file_size, fp);
	printf("%s\n", file_contents);
	fclose(fp);

	// regex
	regex_t re;
	regmatch_t match;
	 
	if(regcomp(&re, str, REG_EXTENDED) != 0) exit(1);
	 
	while(regexec(&re, file_contents, 1, &match, 0) == 0)
	{
	  printf("%.*s\n", (int)(match.rm_eo - match.rm_so), &file_contents[match.rm_so]);
	  file_contents += match.rm_eo; /* or p = &p[match.rm_eo]; */
	}

    regfree(&re);
	return 0;
}