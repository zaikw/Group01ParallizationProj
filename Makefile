#
# Author: Nikos Nikoleris <nikos.nikoleris@it.uu.se>
# Modified by: Jonatan Waern

DEBUG:=

CFLAGS=-pthread -Wall -Wextra -std=c99 -D_XOPEN_SOURCE=600
LDFLAGS=-pthread -lrt

CC=gcc

ifeq ($(DEBUG), y)
	CFLAGS += -ggdb
	LDFLAGS += -ggdb
else
	CFLAGS += -O2
	LDFLAGS += -O2
endif

all: bst_test

bst_test: bst_test.o bst.o timing.o hasp.o

clean:
	$(RM) *.o *.d *~

distclean: clean
	$(RM) bst_test

.PHONY: all clean distclean

%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o
	$(CC) -MM $(CFLAGS) $*.c > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | \
	sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@$(RM) $*.d.tmp

-include $(wildcard *.d)