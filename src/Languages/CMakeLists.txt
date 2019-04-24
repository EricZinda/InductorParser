target_sources(parser
    PRIVATE
        CssParser.h
        CssParser.cpp
        HtmlParser.h
        HtmlParser.cpp
        XmlCompiler.h
        XmlCompiler.cpp
)
target_include_directories(parser PRIVATE ./)