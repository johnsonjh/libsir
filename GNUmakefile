# SPDX-License-Identifier: MIT
# Copyright (c) 2018 Ryan M. Lederman
# Copyright (c) 2022 Jeffrey H. Johnson <trnsz@pobox.com>

# Set DEBUG=1 for debugging

BUILDDIR   = build
DOCSDIR    = docs
TESTSDIR   = tests
EXAMPLEDIR = example
INTERDIR   = $(BUILDDIR)/obj
LIBDIR     = $(BUILDDIR)/lib

ifeq ($(INSTALLDIR),)
	INSTALLDIR = /usr/local/lib
endif
ifeq ($(INSTALLINC),)
	INSTALLINC = /usr/local/include
endif

LIBS    = -pthread

OFLAGS ?= -O3

ifeq ($(DEBUG),1)
	CFLAGS  += -Wall -Wextra -std=gnu11 -I. -g3 -O0 -DNDEBUG -fPIC -DLOG_SELFLOG \
			   -fno-inline -ffunction-sections -fdata-sections -flto=auto
else
	CFLAGS  += -Wall -Wextra -std=gnu11 -I. -DNDEBUG -fPIC $(OFLAGS) -flto=auto
endif

LDFLAGS     += -Wl,-flto=auto $(LIBS) -L$(LIBDIR) -lsir_s

TUS := $(wildcard ./*.c)
_OBJ = $(patsubst %.c, %.o, $(TUS))
OBJ  = $(patsubst %, $(INTERDIR)/%, $(_OBJ))

OBJ_SHARED      = $(patsubst %.o, $(INTERDIR)/%.lo, $(_OBJ))
OUT_SHARED	    = $(LIBDIR)/libsir.so
LDFLAGS_SHARED  = $(LIBS)
OBJ_STATIC      = $(OBJ_SHARED)
OUT_STATIC      = $(LIBDIR)/libsir_s.a
_OBJ_EXAMPLE    = example.o
OBJ_EXAMPLE     = $(patsubst %.o, $(INTERDIR)/%.eo, $(_OBJ_EXAMPLE))
OUT_EXAMPLE     = $(BUILDDIR)/sirexample
EXAMPLETU       = $(TESTSDIR)/example.c
_OBJ_TESTS      = tests.o
OBJ_TESTS       = $(patsubst %.o, $(INTERDIR)/%.to, $(_OBJ_TESTS))
OUT_TESTS       = $(BUILDDIR)/sirtests
TESTSTU         = $(TESTSDIR)/tests.c

.PHONY: all
all: shared static example tests

-include $(INTERDIR)/*.d

$(BUILDDIR)    : prep
$(INTERDIR)    : $(BUILDDIR)
$(LIBDIR)      : $(BUILDDIR)

$(OBJ_EXAMPLE) : $(INTERDIR)
$(OBJ_SHARED)  : $(INTERDIR) $(LIBDIR)
$(OBJ_TESTS)   : $(OBJ_SHARED)

$(INTERDIR)/%.eo: $(EXAMPLEDIR)/%.c $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS)

$(INTERDIR)/%.to: $(TESTSDIR)/%.c $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS)

$(INTERDIR)/%.lo: %.c $(DEPS)
	$(CC) -MMD -c -o $@ $< $(CFLAGS)

.PHONY: prep
prep:
	$(shell set -x ;                \
	        mkdir -p $(BUILDDIR) && \
			mkdir -p $(INTERDIR) && \
	        mkdir -p $(LIBDIR))

.PHONY: shared
shared: $(OBJ_SHARED)
	$(CC) -shared -o $(OUT_SHARED) $^ $(CFLAGS) $(LDFLAGS_SHARED)

.PHONY: static
static: shared
	$(AR) cr $(OUT_STATIC) $(OBJ_SHARED)

.PHONY: example
example: static $(OBJ_EXAMPLE)
	$(CC) -o $(OUT_EXAMPLE) $(OUT_STATIC) $(OBJ_EXAMPLE) $(CFLAGS) $(LDFLAGS)

.PHONY: tests check test
tests check test: static $(OBJ_TESTS)
	$(CC) -o $(OUT_TESTS) $(OUT_STATIC) $(OBJ_TESTS) $(CFLAGS) $(LDFLAGS)

.PHONY: clean
clean:
	$(shell set -x ;                                   \
	        rm -rf ./$(BUILDDIR)/*  >/dev/null 2>&1 && \
	        rm -rf ./$(LIBDIR)/*    >/dev/null 2>&1 && \
			rm -rf ./$(INTERDIR)/*  >/dev/null 2>&1 && \
			rm -f  ./*.log          >/dev/null 2>&1)
	-@true
