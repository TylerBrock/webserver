TARGET=webserver

INCDIR=include
SRCDIR=src
BINDIR=bin
OBJDIR=obj

SRCS = $(wildcard $(SRCDIR)/*.c) # Source files
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:.c=.o)))

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

webserver: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

objdir:
	mkdir -p $(OBJDIR)

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -fr $(OBJDIR)/*.o
	rm webserver

.PHONY: all
