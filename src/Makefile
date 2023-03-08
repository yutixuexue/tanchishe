CC = arm-none-linux-gnueabi-gcc
#CC = gcc

OBJS = *.c
LDFLAGS = -lpthread

GluttonousSnake:$(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)
	mv GluttonousSnake ../
#	mv GluttonousSnake /source/rootfs/

.PHONY:clean
clean:
	rm -rf *.o GluttonousSnake

