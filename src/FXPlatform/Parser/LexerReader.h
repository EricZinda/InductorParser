#pragma once
#include "Lexer.h"

namespace FXPlat
{
    class Symbol;
    
    // We assume the tokens are consumed, and they can be rolled back which inserts them back at the beginning of the queue
    // This class should always be a local stack based class, (i.e. never created with new()) because otherwise it could mess
    // up the ordering of tokens when it gets destructed
    // When Consume is called
    class LexerReader
    {
    public:
        LexerReader(std::shared_ptr<Lexer> lexer);
        ~LexerReader();

        void Abort();
        void Begin();
        void Commit();
        std::shared_ptr<Symbol> Peek();
        unsigned short Peek(std::shared_ptr<Symbol> &symbol);
        std::shared_ptr<Symbol> Read();
        unsigned short Read(std::shared_ptr<Symbol> &symbol);

    private:
        long m_originalPosition;
        int m_transactionDepth;
        std::shared_ptr<Lexer> m_lexer;
    };
}
