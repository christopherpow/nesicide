`rc2qt.py` relies on Python 2.

- `sudo apt install antlr4` (`from rcLexer/Visitor/Parser import ...`)
- `pip install --user antlr4-python2-runtime` (`from antlr4 import *`)
- in dir: `antlr4 -Dlanguage=Python2 -visitor rc.g4`
    - https://jakubdziworski.github.io/java/2016/04/01/antlr_visitor_vs_listener.html#listener-vs-visitor
- `python path/to/rc2qt.py path/to/file.rc`
