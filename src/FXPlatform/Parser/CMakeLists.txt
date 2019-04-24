target_sources(parser
    PRIVATE
    	Compiler.h
    	Lexer.h
    	Lexer.cpp
    	LexerReader.h
    	LexerReader.cpp
    	Parser.h
    	Parser.cpp
    	ParserDebug.h
)
target_include_directories(parser PRIVATE .)