pasm.lib: pasm_tok.l pasm_grm.y pasm_lib.c
	make clean
	flex -si pasm_tok.l
	bison -d pasm_grm.y
	gcc -mno-cygwin -c lex.yy.c pasm_grm.tab.c pasm_lib.c
	ar crv pasm.lib lex.yy.o pasm_grm.tab.o pasm_lib.o /usr/lib/libfl.a

pasm.exe: pasm_tok.l pasm_grm.y pasm_exe.c
	make clean
	flex -si pasm_tok.l
	bison -d -r solved pasm_grm.y
	gcc -gstabs -o pasm.exe lex.yy.c pasm_grm.tab.c pasm_exe.c -lfl

clean:
	rm -f lex.yy.c *.tab.* pasm.lib pasm.exe *.o

all: pasm.exe pasm.lib
