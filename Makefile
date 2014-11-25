OBJDIR = ./obj
BINDIR = ./bin
SRCDIR = ./src
INCDIR = ./include
CC = gcc -I$(INCDIR) 
FUSEFLAGS = $(shell echo `pkg-config fuse --cflags --libs`)
#CFLAGS = -Wall -D_FILE_OFFSET_BITS=64 `pkg-config fuse --cflags --libs`
CFLAGS = -Wall
LDFLAGS = -Wall
#SSOURCES = temp.c temp1.c
#SOURCES = $(patsubst %, $(SRCDIR)/%, $(SSOURCES))
##OOBJ = $(SSOURCES:.c=.o)
#OOBJ = layer0.o mkfs1.o layer1.o layer2.o layer3.o test.o
OOBJ = layer0.o mkfs1.o layer1.o layer2.o layer3.o
OBJ = $(patsubst %, $(OBJDIR)/%, $(OOBJ))
#DDEPS = temp.h
#DEPS = $(patsubst %, $(INCDIR)/%, $(DDEPS))

all: $(BINDIR)/exec

debug: CC += -g
debug: $(BINDIR)/exec

#$(EXEC): $(OBJ) 
#	$(CC) $(LDFLAGS) $(OBJ) -o $@
$(BINDIR)/exec: $(OBJ)
#	$(CC) $(LDFLAGS) $^ -o $@ $(BINDIR)/exec
	$(CC) $(LDFLAGS) $(OBJ) -o $(BINDIR)/exec $(FUSEFLAGS)

#$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
#	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) -o $@ $< $(FUSEFLAGS)

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
	rm -rf test.txt

