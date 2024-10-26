# 自动编译DES算法的代码
NAME = server
.DEFAULT_GOAL = all
# Compilation flags
CC = g++
LD = g++
CFLAGS   += -O2 -Wall -Werror

OBJS_S = des.o server.o
OBJS_C = des.o client.o

client.o : 
	@$(CC) -c client.cpp
server.o : 
	@$(CC) -c server.cpp
des.o : 
	@$(CC) -c include/des.cpp

all: server client

server:$(OBJS_S)
	@echo + LD $@
	@$(LD) $(OBJS_S) -o server

client:$(OBJS_C)
	@echo + LD $@
	@$(LD) $(OBJS_C) -o client

clean:
	@rm -rf $(OBJS_S) $(OBJS_C) server client
