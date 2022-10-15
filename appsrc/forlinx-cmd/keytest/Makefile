TARGET=fltest_keytest

OBJS=keytest.o

CFLAGS+=-c -Wall -g

$(TARGET):$(OBJS)
	$(CC) $^ -o $@  

%.o:%.c                  
	$(CC) $^ $(CFLAGS) -o $@

clean:
	$(RM) *.o $(TARGET) -r

install:
	cp $(TARGET)  ../../../OK8MP-linux-fs/rootfs/usr/bin/
