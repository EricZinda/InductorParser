#pragma once
#include "FXPlatform/Utilities.h"
#include <list>

namespace FXPlat
{
    class LexerReader;
    class Symbol;

    class LexerTransaction
    {
    public:
        LexerTransaction(int depth) : m_depth(depth) {}
        ValueProperty(public, int, depth);
        typedef std::list<std::shared_ptr<Symbol> > SymbolsType;
        Property(public, SymbolsType, symbols);
    };

    // This is a PEG (Parsing Expression Grammar) parser
    class Lexer
    {
    public:
        friend LexerReader;

        Lexer() {}

        long consumedCharacters() { return m_consumedCharacters; }
        int DeepestFailure() { return (int) m_deepestFailure; }
        bool Eof();
        std::string ErrorMessage() { return m_errorMessage; }
        static void GetLineAndColumn(int charPosition, std::shared_ptr<std::istream> stream, int &lineCount, int &columnCount, std::string *linestring = nullptr);
        void Open(std::shared_ptr<std::istream> stream);
        std::shared_ptr<Symbol> Peek();
        void ReportFailure(const std::string &errorMessage);
        int TransactionDepth();

        std::shared_ptr<std::istream> stream() { return m_stream; }
        
    private:
        void AbortTransaction(long position);
        long BeginTransaction();
        void CommitTransaction(long position);
        long Position() { return (long) m_stream->tellg(); }
        void Seek(long position);
        std::shared_ptr<Symbol> Read();

        long m_consumedCharacters;
        long m_deepestFailure;
        std::string m_errorMessage;
        std::shared_ptr<std::istream> m_stream;
        int m_transactionDepth;
    };
}
