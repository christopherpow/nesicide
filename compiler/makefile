
ifeq ("$(OSTYPE)", "cygwin")
	CC := gcc
	CFLAGS_LIB  := -g
	LIB_PREFIX  := lib
	LIB_POSTFIX := .a
	LIB_CREATE  := ar crvs
else
	CC := gcc
	CFLAGS_LIB       := -mno-cygwin 
   LIB_PREFIX   := lib
   LIB_POSTFIX  := .a
	EXEC_POSTFIX := .exe
	LIB_CREATE   := ar crv
endif

ASM=asm
PP=pp

 # TODO: make a better test for os x
ifdef Apple_PubSub_Socket_Render
	CFLAGS     := $(CFLAGS) -arch i386 -arch x86_64 -arch ppc
	LIB_CREATE := libtool -static -o
#	LIBFL_COPY := lipo -extract i386 -extract x86_64 -output libfl.a /usr/lib/libfl.a
endif

all: $(LIB_PREFIX)pasm$(LIB_POSTFIX) pasm$(EXEC_POSTFIX)

$(LIB_PREFIX)pasm$(LIB_POSTFIX): pasm_$(ASM).l pasm_$(ASM).y pasm_lib.c lex.$(ASM).c lex.$(PP).c pasm_$(ASM).tab.c
	$(CC) $(CFLAGS) $(CFLAGS_LIB) -DPASM_LIB -c lex.$(ASM).c lex.$(PP).c pasm_$(ASM).tab.c pasm_lib.c
	$(LIB_CREATE) $(LIB_PREFIX)pasm$(LIB_POSTFIX) lex.$(ASM).o lex.$(PP).o pasm_$(ASM).tab.o pasm_lib.o 

pasm$(EXEC_POSTFIX): pasm_$(ASM).l pasm_$(PP).l pasm_$(ASM).y pasm_exe.c lex.$(ASM).c lex.$(PP).c pasm_$(ASM).tab.c
	$(CC) $(CFLAGS) -O2 -ggdb -DPASM_EXE -o pasm$(EXEC_POSTFIX) lex.$(ASM).c lex.$(PP).c pasm_$(ASM).tab.c pasm_exe.c 
#	$(CC) $(CFLAGS) -DPASM_EXE -O2 -o pasm$(EXEC_POSTFIX) lex.$(ASM).c lex.$(PP).c pasm_$(ASM).tab.c pasm_exe.c

clean:
	rm -f lex.*.c *.tab.* $(LIB_PREFIX)pasm$(LIB_POSTFIX) pasm$(EXEC_POSTFIX) *.o
	
lex.$(ASM).c : pasm_$(ASM).l
	flex -sil -P$(ASM) pasm_$(ASM).l

lex.$(PP).c : pasm_$(PP).l
	flex -sil -P$(PP) pasm_$(PP).l

pasm_$(ASM).tab.c : pasm_$(ASM).y
	bison -dv -p$(ASM) -r all pasm_$(ASM).y

# for debug use:
# flex -sid
# bison -dvt
