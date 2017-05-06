# Libuthread Makefile
targets := libuthread.a
objs    := queue.o uthread.o context.o preempt.o

CC      := gcc 
CFLAGS  := -Wall -Werror 
CFLAGS  += -g 
PANDOC  := pandoc

ifneq ($(V),1) 
Q = @ 
endif

all: $(targets)

deps := $(patsubst %.o,%.d,$(objs)) 
-include $(deps) 
DEPFLAGS = -MMD -MF $(@:.o=.d)

libuthread.a: $(objs)     
	@echo "CC $@"     
	ar rcs libuthread.a $(objs)  

%.o: %.c     
	@echo "CC $@"     
	$(Q)$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

clean:     
	@echo "clean"     
	$(Q)rm -f $(targets) $(objs) $(deps)
