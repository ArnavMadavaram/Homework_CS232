CC = gcc
CFLAGS = -g -std=gnu11
OBJS = indexPage.o crawler.o webQuery.o

all: webSearch
webSearch: webSearch.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $< -o $@ -lm
%.o : %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm *.o webSearch.exe