#
# Author: Nikos Nikoleris <nikos.nikoleris@it.uu.se>
# Modified by: Jonatan Waern

DEBUG ?=y

CFLAGS=-pthread -std=c99 -D_XOPEN_SOURCE=600
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

all: 	interpreter

run: 	all
	$(BUILD)/interpreter

debug:	all
	gdb $(BUILD)/interpreter

interpreter: parser $(SRC)/interpreter.c $(SRC)/hashmap.c $(SRC)/hashmap.h
	$(CC) $(CFLAGS) $(SRC)/interpreter.c $(SRC)/parser.tab.c $(SRC)/structures.c $(SRC)/lex.yy.c $(SRC)/hashmap.c -o $(BUILD)/interpreter

parser: $(SRC)/tokenizer.l $(SRC)/parser.y $(SRC)/structures.h $(SRC)/structures.c
	bison $(SRC)/parser.y --defines=$(SRC)/parser.tab.h -o $(SRC)/parser.tab.c		
	flex -o $(SRC)/lex.yy.c $(SRC)/tokenizer.l 	

clean:
	ls $(BUILD)/ | grep -v "\." | (cd $(BUILD); xargs $(RM)) 
	$(RM) $(BUILD)/* $(BUILD)/*.o $(BUILD)/*.d $(SRC)/*~
	$(RM) $(SRC)/parser.tab.h $(SRC)/parser.tab.c $(SRC)/lex.yy.c

-include $(wildcard *.d)		