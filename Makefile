# Thie is working now! Most of credits to: http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
# Also referenced:
# make manual: https://www.gnu.org/software/make/manual/html_node/index.html
# Ruoshui on 20191105

# Email complaints to: rmao10@stuy.edu


# space-separated list of source files
SRCS = main.c # Place all

# name for executable
EXE = main

# USAGE: Change above variables to suit your needs

ifeq ($(filter $(DEBUG), false f FALSE F), )
	DEBUG_FLAG = -ggdb3
endif

# flags to pass compiler
CFLAGS = $(DEBUG_FLAG) -std=c11

# compiler to use
CC = gcc

# Ruoshui: my computer doesn't have gcc; so this will change CC to clang if "which gcc" outputs nothing
ifeq (, $(shell which gcc))
	CC = clang
endif

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


.PHONY: clean run autorun

# housekeeping
clean:
	rm -f $(EXE)
	rm -rf obj

run:
	./$(EXE)

autorun: $(EXE)
	./$(EXE)

# OLD MAKEFILE:

# # compiler to use
# CC = gcc

# ifeq (, $(shell which gcc))
# 	CC = clang
# endif

# # flags to pass compiler
# CFLAGS = -ggdb3 -std=c11

# # name for executable
# EXE = main

# # space-separated list of header files
# HDRS = llist.h songlib.h

# # space-separated list of source files
# SRCS = main.c llist.c songlib.c

# # automatically generated list of object files
# OBJS = $(SRCS:.c=.o)


# # default target
# $(EXE): $(OBJS) $(HDRS) Makefile
# 	$(CC) $(CFLAGS) -o $@ $(OBJS)

# # dependencies
# $(OBJS): $(HDRS) Makefile


# .PHONY: clean run

# # housekeeping
# clean:
# 	rm -f core $(EXE) *.o
# 	rm -rf obj

# run:
# 	./$(EXE)
