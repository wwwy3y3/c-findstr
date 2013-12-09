# find string
all:string_search.c
	gcc string_search.c -o string_search -lpthread
clean:
	rm -f string_search