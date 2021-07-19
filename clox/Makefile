# Compiler flags
CC = gcc
CFLAGS += -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter

# Project files
SOURCE_DIR := src
HEADERS := $(wildcard $(SOURCE_DIR)/*.h)
SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJS := $(notdir $(SOURCES:.c=.o))
EXE := clox

# Debug build settings
DBGDIR = build/debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCFLAGS = -g -O0 -DDEBUG

# Release build settings
RELDIR = build/release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCFLAGS = -O3 -DNDEBUG

.PHONY: all clean debug prep release remake

# Default build
all: prep release

# Debug rules
debug: prep $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXE) $^

$(DBGDIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

# Release rules
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELEXE) $^

$(RELDIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(RELCFLAGS) -o $@ $<

# Other rules
prep:
	@ mkdir -p $(DBGDIR) $(RELDIR)

remake: clean all

clean:
	@ rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS)
