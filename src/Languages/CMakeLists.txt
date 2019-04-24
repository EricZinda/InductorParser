target_sources(parser
    PRIVATE
        CssParser.h
        CssParser.cpp
        HtmlParser.h
        HtmlParser.cpp
        XmlCompiler.h
)
target_include_directories(parser PRIVATE ./)