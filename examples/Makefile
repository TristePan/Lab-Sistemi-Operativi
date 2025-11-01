CC            = gcc
CFLAGS        =
LDFLAGS       = -lm
C_SOURCES     = $(wildcard *.c)
OBJS          = $(patsubst %.c,%.o,$(C_SOURCES))
PROGS         = $(patsubst %.c,%,$(C_SOURCES))

all: $(PROGS)

#$(OBJS): $(C_SOURCES)
%.o: %.c 
	$(CC) $(CFLAGS) -o $@ -c $(patsubst %.o,%.c,$@)

clean:
	rm -f ${PROGS} ${OBJS}
