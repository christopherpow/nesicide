JAVA=java
#JAVAC=/usr/bin/javac
GRAMMAR=rc.g4
ANTLR_JAR=antlr-4.7.2-complete.jar
GEN_LANGUAGE=Python3

all: antlr4_python 

antlr4_python: $(GRAMMAR)
	$(JAVA) -jar $(ANTLR_JAR) -visitor -Dlanguage=$(GEN_LANGUAGE) $(GRAMMAR)

#antlr4_java: $(GRAMMAR)
#	$(JAVA) -jar $(ANTLR_JAR) -visitor $(GRAMMAR)
#	$(JAVAC) *.java

clean:
	-rm -f rc[A-Z]*.py *.pyc 
	-rm -f *.class *.java
	-rm -f *.tokens *.interp
