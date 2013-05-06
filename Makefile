#
# Author: Nikos Nikoleris <nikos.nikoleris@it.uu.se>
# Modified by: Jonatan Waern

DEBUG:=

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


parser: $(SRC)/tokenizer.l $(SRC)/parser.y
	flex $(SRC)/tokenizer.l -o $(SRC)/lex.yy.c
	bison $(SRC)/parser.y --defines=$(SRC)/parser.tab.h -o $(SRC)/parser.tab.c

clean:
	$(RM) *.o *.d *~

%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(CC) -MM $(CFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@$(RM) $*.d.tmp

-include $(wildcard *.d)		