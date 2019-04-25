#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include "FXPlatform/Parser/Compiler.h"
#include "FXPlatform/Parser/ParserDebug.h"
#include "Languages/XmlCompiler.h"
#include <string>

// Simply parses the document and stores the Symbol tree unprocessed
template<class Parser>
class BasicCompiler : public Compiler<Parser>
{
protected:
    virtual bool ProcessAst( shared_ptr<typename BasicCompiler::CompileResultType> ast)
    { 
    	m_result = ast;
    	return true;
    }

	ValueProperty(protected, shared_ptr<vector<shared_ptr<Symbol>>>, result);
};

int main (int argc, char *argv[])
{
//	SetTraceFilter(SystemTraceType::Parsing, TraceDetail::Detailed);

	if(argc != 3)
	{
		  fprintf(stdout, 
		  	"IndParse Parser example from https://github.com/EricZinda/InductorParser. \r\n"
		  	"Parses several types of documents\r\n"
		  	"and outputs the string representation of their parsed data structure to\r\n"
		  	"prove it worked.\r\n\r\n"
		  	"Command line arguments: gparse [language] [file] \r\n"
		  	"Examples: \r\n"
		  	"	gparse xml ./Foo.xml \r\n"
		  	"	gparse html ./Bar.html \r\n"
		  	"	gparse css ./Goo.css \r\n"
			);
	}
	else
	{
	    string language = string(argv[1]);
	    string targetFileAndPath = string(argv[2]);

	    if(language == "xml")
	    {
	   		fprintf(stdout, 
	   			"Printing out parsed tree:  \r\n\r\n"
	   			);

	    	XmlCompiler compiler;
	    	if(compiler.CompileDocument(targetFileAndPath))
	    	{
	    		fprintf(stdout, "%s\r\n", compiler.result()->ToString().c_str());
	    		return 0;
	    	}
	    	else
	    	{
	    		fprintf(stdout, "%s\r\n", compiler.GetErrorString().c_str());
	    		return 1;
	    	}
	    }
	    else if(language == "html")
	    {

	   		fprintf(stdout, 
	   			"Printing out symbol tree.  Format is:\r\n" 
	   			"	[SymbolID] ([FlattenType]): [String representation of node and all children]\r\n"
	   			"	followed by all children, recursively \r\n\r\n"
	   			);
	    	BasicCompiler<Html::HtmlDocumentRule> compiler;
	    	if(compiler.CompileDocument(targetFileAndPath))
	    	{
	    		fprintf(stdout, "%s\r\n", ParserDebug::PrintTree(*compiler.result()).c_str());
	    		return 0;
	    	}
	    	else
	    	{
	    		fprintf(stdout, "%s\r\n", compiler.GetErrorString().c_str());
	    		return 1;
	    	}	    	
	    }
	}

  	return 0;
}