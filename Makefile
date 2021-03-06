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


#######################################################################
# Build options

# uncomment *1* of these.  (pick between portAudio and libSDL for audio)
# if neither of them are uncommented, then this works in "silent mode"
USEPORTAUDIO := 1
#USESDL	     := 1

TARG := lbb

SRCS := main.c \
	glitch.c 

ifeq ($(USEPORTAUDIO),1)
SRCS += paHelper.c
endif

####################
# additions for portaudio
ifeq ($(USEPORTAUDIO),1)
LIBS += -lportaudio
DEFS += -DUSEPORTAUDIO
endif


####################
# additions for libsdl
ifeq ($(USESDL),1)
LIBS += $(shell sdl-config --static-libs)
DEFS += -DUSESDL
endif


####################
# general build rules

LDFLAGS += 
DEFS += 
CFLAGS += $(DEFS) -Wall -pedantic
LIBS += -lncurses 
INCS += 


################################################################################

all: $(TARG)
.PHONY: all

################################################################################

OBJS := $(SRCS:%.c=build/%.o)

$(TARG): $(OBJS)
	@echo link $@
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@


################################################################################
TARGS += pasimple 

pasimple: build/simple.o build/paHelper.o
	@echo link $@
	@$(CC) $(CFLAGS) $^ $(LDFLAGS) $(LIBS) -o $@


TARGS += sdlsimple 

sdlsimple: build/sdlsimple.o
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

# XYZ Accelerometer  xy pad 1  vw pad2

testa: $(TARG) 
	./$(TARG) glitch://Hello!abcdefg!hijklmn!opqrstu!vwxy!XYZ!12.34.4.5
	./$(TARG) GLITCH://Hello!abcdefg!hijklmn!opqrstu!vwxy!XYZ!12.34.4.5
	./$(TARG) Hello!abcdefg!hijklmn!opqrstuvwxyz!12.34.4.5

testb: $(TARG)
	./$(TARG) glitch://genmusic2_atari!a7kaa6kmmAd!aaDka6kmmd!x
	./$(TARG) the_42_melody!aAk2Alad!y
	./$(TARG) glitch://2muchop!1!0!aFFe6had!!aaadda4eg8k!aada2000eh!a400e3h2fqad80h!0!!2.1.2.1.4.1.2.1!2.1.2.1.4.1.3.2!!a400e10hqFf!q
	./$(TARG) glitch://guitar2!a3kal!a3000h400sl!80qD0h3d!ff4eFFl!p
	./$(TARG) -volume 0.1 -novis glitch://glitch4life!a1k1000.8eha80!a80dmlp0k1000.2dh!mr!!e
	
test: $(TARG)
	./$(TARG) glitch://glitch4life!a1k1000.8eha80!a80dmlp0k1000.2dh!mr!!e
	./$(TARG) glitch://malordy!a40hea2kr

testm: $(TARG)

testnv:
	./$(TARG) -volume 0.0 -novis glitch://glitch4life!a1k1000.8eha80!a80dmlp0k1000.2dh!mr!!e
