
ifdef CROSS_COMPILE
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar $(CROSS_COMPILE)cr
STRIP = $(CROSS_COMPILE)strip
else
CC = arm-linux-gcc
AR = arm-linux-ar arm-linux-cr
STRIP = arm-linux-strip
endif

MAKE = make
RM = -rm -rf
CFLAGS = -D _GNU_SOURCE -Wall -I./
LIBS = -lpthread -lc -lgcc

#设置最终目标
TARGET = httpd.cgi
BIN_DIR = ./

#获取路径
SRCS = $(shell ls -l | grep -i \.c$$ | awk '{print $$9}')
OBJS = $(SRCS:%.c=%.o)
INCS = $(wildcard *.h)
#INCS := $(patsubst %,\"%\".,$(INCS))

#定义伪目标(标签)
.PHONY: all clean

all: $(OBJS) $(TARGET)

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET):$(OBJS)
	$(CC) -static -o $@ $^ $(LIBS)
	$(STRIP) $(TARGET)
	rm -rf *.o
clean:
	$(RM) $(TARGET) $(OBJS) 
	rm -rf *.o
