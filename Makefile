#
# Author: Nikos Nikoleris <nikos.nikoleris@it.uu.se>
# Modified by: Jonatan Waern

DEBUG ?=

CFLAGS=-pthread -Wall -Wextra -std=c99 -D_XOPEN_SOURCE=600
LDFLAGS=-pthread -lrt
SRC=./src
BUILD=./build


CC=gcc

ifeq ($(DEBUG), y)
	CFLAGS += -ggdb
	LDFLAGS += -ggdb
else
	CFLAGS += -O2
	LDFLAGS += -O2
endif

all: parser


parser.o: parser
	$(CC) $(CFLAGS) $(SRC)/parser.tab.c $(SRC)/lex.yy.c -o $(BUILD)/parser.o

parser: $(SRC)/tokenizer.l $(SRC)/parser.y
	bison $(SRC)/parser.y --defines=$(SRC)/parser.tab.h -o $(SRC)/parser.tab.c		
	flex -o $(SRC)/lex.yy.c $(SRC)/tokenizer.l 	

clean:
	$(RM) *.o *.d *~
	$(RM) $(SRC)/parser.tab.h $(SRC)/parser.tab.c $(SRC)/lex.yy.c

%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $(SRC)/$*.c -o $(BUILD)/$*.o

run: interpreter.o
	interpreter.o

-include $(wildcard *.d)		