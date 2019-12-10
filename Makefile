# Thie is working now! Most of credits to: http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# Also referenced:
# make manual: https://www.gnu.org/software/make/manual/html_node/index.html
# Ruoshui on 20191205

# Email complaints to: rmao10@stuy.edu


# space-separated list of source files
SRCS = main.c shell.c utils.c parser.c cmds.c lexer.c ast.c

# name for executable
EXE = main

# USAGE: Change above variables to suit your needs

ifeq ($(filter $(DEBUG), false f FALSE F), )
	DEBUG_FLAG = -ggdb3
endif

CSTD = gnu11 # not c11

# flags to pass compiler
CFLAGS = $(DEBUG_FLAG) -std=$(CSTD)

# compiler to use
CC = gcc

LEXFILE = lexer

OBJDIR := obj

# automatically generated list of object files
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

# default target
$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)



DEPDIR := $(OBJDIR)/.deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c

$(OBJDIR)/%.o : %.c $(DEPDIR)/%.d | $(DEPDIR)
	$(COMPILE.c) $(OUTPUT_OPTION)  $<

$(DEPDIR): ; @mkdir -p $@

DEPFILES := $(SRCS:%.c=$(DEPDIR)/%.d)
$(DEPFILES):

include $(wildcard $(DEPFILES))

lexer: $(LEXFILE).l
	flex --header-file=$(LEXFILE).h -o $(LEXFILE).c $(LEXFILE).l

.PHONY: clean run autorun

# housekeeping
clean:
	rm -f $(EXE)
	rm -rf obj

run:
	./$(EXE)

autorun: $(EXE)
	./$(EXE)

memcheck:
	valgrind --leak-check=summary ./$(EXE)