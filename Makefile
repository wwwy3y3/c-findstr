# find string
all:find.c
	gcc find.c -o find -lpthread
clean:
	rm -f find