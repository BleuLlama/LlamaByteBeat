# LlamaByteBeat
#
#	Scott Lawrence
#	yorgle@gmail.com
#	2011-Dec

# 
# Copyright (C) 2011 Scott Lawrence
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
# ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
# CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 


TARG := lbb

SRCS := main.c \
	paHelper.c

LDFLAGS += 
CFLAGS += -G -Wall -pedantic
LIBS += -lncurses -lportaudio
INCS += 

################################################################################

all: $(TARG) simple
.PHONY: all

################################################################################

OBJS := $(SRCS:%.c=build/%.o)

$(TARG): $(OBJS)
	@echo link $@
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@


################################################################################
TARGS += simple 

simple: build/simple.o build/paHelper.o
	@echo link $@
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@


################################################################################

build/%.o: %.c
	@echo build -- $(CC) $<	
	@$(CC) $(CFLAGS) $(DEFS) $(INCS) -c -o $@ $<

$(OBJS): build

build:
	@echo Making Build Directiory
	@mkdir build/


clean: 
	@echo removing all build files.
	@rm -rf build $(OBJS) $(TARG) $(TARGS)

test: $(TARG) $(TARGS)
	./$(TARG)
	