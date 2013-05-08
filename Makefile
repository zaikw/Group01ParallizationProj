#
# Author: Nikos Nikoleris <nikos.nikoleris@it.uu.se>
# Modified by: Jonatan Waern

DEBUG ?= y

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

run: 	interpreter.o
	$(BUILD)/interpreter.o

debug:	interpreter.o
	gdb $(BUILD)/interpreter.o 

interpreter.o: $(SRC)/interpreter.c $(SRC)/parser.h parser
	$(CC) $(CFLAGS) $(SRC)/interpreter.c $(SRC)/parser.tab.c $(SRC)/lex.yy.c         -o $(BUILD)/interpreter.o

parser: $(SRC)/tokenizer.l $(SRC)/parser.y
	bison $(SRC)/parser.y --defines=$(SRC)/parser.tab.h -o $(SRC)/parser.tab.c		
	flex -o $(SRC)/lex.yy.c $(SRC)/tokenizer.l 	

clean:
	$(RM) $(BUILD)/*.o $(BUILD)/*.d $(SRC)/*~
	$(RM) $(SRC)/parser.tab.h $(SRC)/parser.tab.c $(SRC)/lex.yy.c

%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $(SRC)/$*.c -o $(BUILD)/$*.o

run: interpreter.o
	interpreter.o

-include $(wildcard *.d)		