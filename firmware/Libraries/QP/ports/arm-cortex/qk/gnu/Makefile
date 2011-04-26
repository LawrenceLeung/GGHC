#  GNU Makefile for QP/C for ARM Cortex-M3, QK port, GNU compiler
#  Ned Konz, Tue Apr 26 2011
#
#  adjust the following to point to the location where you've installed the
#  GNU_ARM toolset...
# 
GNU_ARM ?= /usr/local/devkitARM
ARM_PREFIX ?= arm-eabi-
ARM_BIN ?= /bin

CC :=$(GNU_ARM)$(ARM_BIN)/$(ARM_PREFIX)gcc
AS :=$(GNU_ARM)$(ARM_BIN)/$(ARM_PREFIX)as
LIB:=$(GNU_ARM)$(ARM_BIN)/$(ARM_PREFIX)ar

QP_INCDIR:=../../../../include
QP_PRTDIR:=.

ARM_CORE:=cortex-m3
LIBDIR=$(BINDIR)
LIBFLAGS=rs

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BINDIR)/%.o,$(SRCS))
PORT_OBJ = $(BINDIR)/qk_port.o

# export everything because I'm lazy
export

.PHONY: dbg rel spy

dbg: BINDIR=$(QP_PRTDIR)/dbg
dbg: CFLAGS=-g -c -mcpu=$(ARM_CORE) -mthumb -Wall -O
dbg: ASFLAGS=-g -mcpu=$(ARM_CORE)
dbg: qep qf qk
	echo "dbg selected"

rel: BINDIR=$(QP_PRTDIR)/rel
rel: CFLAGS=-c -mcpu=$(ARM_CORE) -mthumb -Wall -Os -DNDEBUG
rel: ASFLAGS=-mcpu=$(ARM_CORE)
rel: qep qf qk
	echo "rel selected"

spy: BINDIR=$(QP_PRTDIR)/spy
spy: CFLAGS=-g -c -mcpu=$(ARM_CORE) -mthumb -Wall -O -DQ_SPY
spy: ASFLAGS=-g -mcpu=$(ARM_CORE)
spy: qep qf qk qs
	echo "spy selected"

.PHONY: clean

clean:
	-rm rel/*.a rel/*.o
	-rm dbg/*.a dbg/*.o
	-rm spy/*.a spy/*.o

.PHONY: qk qs qf qep
#  QEP ----------------------------------------------------------------------

qep: SRCDIR=../../../../qep/source
qep: CPPFLAGS=-I$(QP_PRTDIR) -I$(QP_INCDIR) -I$(SRCDIR)
qep:
	$(MAKE) $(LIBDIR)/libqep_$(ARM_CORE).a

$(LIBDIR)/libqep_$(ARM_CORE).a: $(OBJS)
	$(LIB) $(LIBFLAGS) $(LIBDIR)/libqep_$(ARM_CORE).a $(OBJS)
	-rm $^

#  QF ----------------------------------------------------------------------

qf: SRCDIR=../../../../qf/source
qf: CPPFLAGS=-I$(QP_PRTDIR) -I$(QP_INCDIR) -I$(SRCDIR)
qf:
	$(MAKE)	$(LIBDIR)/libqf_$(ARM_CORE).a

$(LIBDIR)/libqf_$(ARM_CORE).a : $(OBJS) $(PORT_OBJ)
	$(LIB) $(LIBFLAGS) $(LIBDIR)/libqf_$(ARM_CORE).a $(OBJS) $(BINDIR)/qk_port.o
	-rm $^

#  QK -----------------------------------------------------------------------
qk: SRCDIR=../../../../qk/source
qk: CPPFLAGS=-I$(QP_PRTDIR) -I$(QP_INCDIR) -I$(SRCDIR)
qk:
	$(MAKE) $(LIBDIR)/libqk_$(ARM_CORE).a

$(LIBDIR)/libqk_$(ARM_CORE).a : $(OBJS) $(PORT_OBJ)
	$(LIB) $(LIBFLAGS) $(LIBDIR)/libqk_$(ARM_CORE).a $(OBJS) $(BINDIR)/qk_port.o
	-rm $^

#  QS -----------------------------------------------------------------------
qs: SRCDIR=../../../../qs/source
qs: CPPFLAGS=-I$(QP_PRTDIR) -I$(QP_INCDIR) -I$(SRCDIR)
qs:
	$(MAKE) $(LIBDIR)/libqs_$(ARM_CORE).a 

$(LIBDIR)/libqs_$(ARM_CORE).a : $(OBJS)
	echo making in $(SRCDIR)
	$(LIB) $(LIBFLAGS) $(LIBDIR)/libqs_$(ARM_CORE).a $(OBJS)
	-rm $^

#  rules -----------------------------------------------------------------------

$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(BINDIR)/%.o: src/%.s
	$(COMPILE.s) $(OUTPUT_OPTION) $<

