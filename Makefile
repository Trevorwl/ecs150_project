CC = gcc

#add -MMD and -MP to create .d files
CFLAGS = -Wall -Werror -MMD -MP


OBJDIR = obj
OBJS = $(OBJDIR)/sshell.o $(OBJDIR)/cmd.o $(OBJDIR)/utils.o


#Create names for .d files
DEPS = $(OBJS:.o=.d)



#link object files and create executable
sshell: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)



#For each object file, compile its c file and create object file
$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@



#make directory for object files
$(OBJDIR):
	mkdir -p $(OBJDIR)



clean:
	rm -rf sshell $(OBJDIR)



run: sshell
	./sshell



tester: sshell
	chmod +x tester.sh
	./tester.sh


#include .d files for compiler to check headers
-include $(DEPS)