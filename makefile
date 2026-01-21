CC=gcc
CFLAGS=-Wall -Wextra -pedantic
INC=-Iinclude

SRC=main.c src/arp.c src/utils.c src/icmp.c
OUT=build/tcpstack

all:
	$(CC) $(CFLAGS) $(SRC) $(INC) -o $(OUT)