CC = gcc
CFLAGS = -Iinclude -DPROJECT_ROOT=\"$(shell pwd)\"
SRCDIR = src
OBJDIR = obj
BINDIR = bin

SRCS = $(SRCDIR)/basicStrategy.c $(SRCDIR)/hardTotals.c $(SRCDIR)/pairSplitting.c $(SRCDIR)/settings.c $(SRCDIR)/softTotals.c
OBJS = $(OBJDIR)/basicStrategy.o $(OBJDIR)/hardTotals.o $(OBJDIR)/pairSplitting.o $(OBJDIR)/settings.o $(OBJDIR)/softTotals.o

all: $(BINDIR)/basicStrategyTrainer

$(BINDIR)/basicStrategyTrainer: $(OBJS)
	$(CC) $(OBJS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/*
