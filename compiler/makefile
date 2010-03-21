
ifeq ("$(WINDOWS)", "YES")
	CFLAGS_LIB       := -mno-cygwin
	LIB_PREFIX   := 
	LIB_POSTFIX  := .lib
	EXEC_POSTFIX := .exe
	LIB_CREATE   := ar crv
else
	CFLAGS_LIB       := -mno-cygwin
	LIB_PREFIX  := lib
	LIB_POSTFIX := .a
	LIB_CREATE  := ar crvs
endif

 # TODO: make a better test for os x
ifdef Apple_PubSub_Socket_Render
	CFLAGS     := $(CFLAGS) -arch i386 -arch x86_64 -arch ppc -arch ppc64
	LIB_CREATE := libtool -static -o
#	LIBFL_COPY := lipo -extract i386 -extract x86_64 -output libfl.a /usr/lib/libfl.a
endif

all: $(LIB_PREFIX)pasm$(LIB_POSTFIX) pasm$(EXEC_POSTFIX)

$(LIB_PREFIX)pasm$(LIB_POSTFIX): pasm_tok.l pasm_grm.y pasm_lib.c lex.yy.c pasm_grm.tab.c
	gcc $(CFLAGS) $(CFLAGS_LIB) -c lex.yy.c pasm_grm.tab.c pasm_lib.c
	$(LIB_CREATE) $(LIB_PREFIX)pasm$(LIB_POSTFIX) lex.yy.o pasm_grm.tab.o pasm_lib.o /usr/lib/libfl.a

pasm$(EXEC_POSTFIX): pasm_tok.l pasm_grm.y pasm_exe.c lex.yy.c pasm_grm.tab.c
	gcc $(CFLAGS) -gstabs -o pasm$(EXEC_POSTFIX) lex.yy.c pasm_grm.tab.c pasm_exe.c -lfl

clean:
	rm -f lex.yy.c *.tab.* $(LIB_PREFIX)pasm$(LIB_POSTFIX) pasm$(EXEC_POSTFIX) *.o
	
lex.yy.c : pasm_tok.l
	flex -si pasm_tok.l

pasm_grm.tab.c : pasm_grm.y
	bison -dv -r all pasm_grm.y

# for debug use:
# flex -sid
# bison -dvt
