INCLUDES    := -I../include/

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: all clean

all: start.o init.o cache.o

clean:
	rm -rf *.o *~
