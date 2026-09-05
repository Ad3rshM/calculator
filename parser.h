#pragma once

#include <iostream>
#include "token.h"
#include "ast.h"

class Parser{
    public:
        Parser(std::vector<Token>);
        const Token& peek() const;
        std::unique_ptr<Node> parseAll();
    private:
        std::vector<Token> tokens;
        std::size_t position {0};

        const Token& advance();
        bool match(CharType);

        std::unique_ptr<Node> parseOne();
        std::unique_ptr<Node> parsePowers();
        std::unique_ptr<Node> parseMiddle();
        std::unique_ptr<Node> parseLow();
};