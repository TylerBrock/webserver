TARGET=webserver

INCDIR=include
SRCDIR=src
BINDIR=bin
OBJDIR=obj

SRCS = $(wildcard $(SRCDIR)/*.c) 												# Source files
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:.c=.o)))  # Object files
DEPS = $(addprefix $(OBJDIR)/,$(notdir $(OBJS:.o=.d)))  # Depenencies

CC=clang
CFLAGS=\
	-Wall\
	-Wextra\
	-Wpedantic\
	-Wshadow\
	-Wstrict-overflow\
	-Werror\
	-fno-strict-aliasing\
	-flto\
	-march=native\
	-I ./$(INCDIR)\
	-g\
	-O2

all: webserver

-include $(DEPS)

webserver: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJS): | $(OBJDIR)

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.d: src/%.c
	$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

clean:
	rm -fr $(OBJDIR)/*.o
	rm -f webserver

.PHONY: all
