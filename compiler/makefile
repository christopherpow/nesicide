all: pasm.exe pasm.lib

pasm.lib: pasm_tok.l pasm_grm.y pasm_lib.c lex.yy.c pasm_grm.tab.c
	gcc -mno-cygwin -c lex.yy.c pasm_grm.tab.c pasm_lib.c
	ar crv pasm.lib lex.yy.o pasm_grm.tab.o pasm_lib.o /usr/lib/libfl.a

pasm.exe: pasm_tok.l pasm_grm.y pasm_exe.c lex.yy.c pasm_grm.tab.c
	gcc -gstabs -o pasm.exe lex.yy.c pasm_grm.tab.c pasm_exe.c -lfl

clean:
	rm -f lex.yy.c *.tab.* pasm.lib pasm.exe *.o
	
lex.yy.c : pasm_tok.l
	flex -si pasm_tok.l

pasm_grm.tab.c : pasm_grm.y
	bison -dv -r all pasm_grm.y

# for debug use:
# flex -sid
# bison -dvt
