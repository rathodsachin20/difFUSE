OBJDIR = ./obj
BINDIR = ./bin
SRCDIR = ./src
INCDIR = ./include

CC = gcc
CFLAGS = -D_FILE_OFFSET_BITS=64
LDFLAGS = -D_FILE_OFFSET_BITS=64
#SSOURCES = temp.c temp1.c
#SOURCES = $(patsubst %, $(SRCDIR)/%, $(SSOURCES))
##OOBJ = $(SSOURCES:.c=.o)
OOBJ = temp.o temp1.o
OBJ = $(patsubst %, $(OBJDIR)/%, $(OOBJ))
#DDEPS = temp.h
#DEPS = $(patsubst %, $(INCDIR)/%, $(DDEPS))

all: $(BINDIR)/exec

#$(EXEC): $(OBJ) 
#	$(CC) $(LDFLAGS) $(OBJ) -o $@
$(BINDIR)/exec: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

#$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
#	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

#$(OBJ): $(SOURCES)
#	$(CC) -c $(CFLAGS) -o $@ $<

#temp.o: temp.c
#	$(CC) -c $(CFLAGS) temp.c
#
#temp1.o: temp1.c
#	$(CC) -c $(CFLAGS) temp1.c

clean:
	rm -rf $(OBJDIR)/*.o
	rm -rf $(BINDIR)/*
