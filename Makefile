OBJDIR = ./obj
BINDIR = ./bin
SRCDIR = ./src
INCDIR = ./include

CC = gcc -I$(INCDIR)
CFLAGS = -D_FILE_OFFSET_BITS=64
LDFLAGS = -D_FILE_OFFSET_BITS=64
#SSOURCES = temp.c temp1.c
#SOURCES = $(patsubst %, $(SRCDIR)/%, $(SSOURCES))
##OOBJ = $(SSOURCES:.c=.o)
#OOBJ = layer0.o mkfs1.o layer1.o layer2.o layer3.o test.o
OOBJ = layer0.o mkfs1.o layer1.o layer2.o layer3.o
OBJ = $(patsubst %, $(OBJDIR)/%, $(OOBJ))
#DDEPS = temp.h
#DEPS = $(patsubst %, $(INCDIR)/%, $(DDEPS))

all: $(BINDIR)/exec

#$(EXEC): $(OBJ) 
#	$(CC) $(LDFLAGS) $(OBJ) -o $@
$(BINDIR)/exec: $(OBJ)
#	$(CC) $(LDFLAGS) $^ -o $@ $(BINDIR)/exec
	$(CC) $(LDFLAGS) $(OBJ) -o $(BINDIR)/exec

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
	rm -rf test.txt

