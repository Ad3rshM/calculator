#include <iostream>
#include <sstream>
#include "parser.h"
#include "token.h"
#include "evaluator.h"

Parser::Parser(std::vector<Token> tokens):
    tokens(std::move(tokens))
    {}

const Token& Parser::peek() const{
    return tokens[position];
}

const Token& Parser::advance() {
    Token curr_token = tokens[position];
    if (position >= tokens.size()){
        throw std::runtime_error("Unexpected end to input.");
    }
    return tokens[position++];
}

bool Parser::match(CharType type){
    if (position >= tokens.size() || tokens[position].type != type){
        return false;
    }
    ++position;
    return true;
}

std::unique_ptr<Node> Parser::parseOne() {
    if (peek().type == CharType::Number){
        std::string name = advance().content;
        
        try{
            return std::make_unique<NumberNode>(std::stold(name));
        }
        catch(...){
            throw std::runtime_error("Error with Number token.");
        }
        
    }

    if (peek().type == CharType::Identifier){
        std::string name = advance().content;

        if (match(CharType::LeftParen)){
            std::vector<std::unique_ptr<Node>> args {};
            if (match(CharType::RightParen)){
                throw std::runtime_error("Function must contain variable in parentheses.");
            }
            do {
                auto temp = parseLow();
                args.push_back(std::move(temp));
            }
            while (match(CharType::Comma));

            if (!match(CharType::RightParen)){
                throw std::runtime_error("Functions missing ')'.");
            }
            try{
                return std::make_unique<FunctionNode>(name, std::move(args));
            }
            catch(...){
                throw std::runtime_error("Error with Function token.");
            }
        }
        try{
            return std::make_unique<IdentifierNode>(name);
        }
        catch(...){
            throw std::runtime_error("Error with Identifier token.");
        }
    }

    if (match(CharType::LeftParen)){
        if(match(CharType::RightParen)){
            throw std::runtime_error("Empty parentheses.");
        }
        auto node = parseLow();
        if(!match(CharType::RightParen)){
            throw std::runtime_error("Expected ')'.");
        }
        return node;

    }
    std::ostringstream error_message;
    error_message << "Invalid token '" << tokens[position].content << "'.";
    throw std::runtime_error(error_message.str());
}

std::unique_ptr<Node> Parser::parsePowers() {
    auto left = parseOne();
    if (match(CharType::Power)){
        auto right = parseUnary();

        return std::make_unique<BinaryNode>(CharType::Power, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::parseUnary() {
    if (peek().type == CharType::UnaryMinus || peek().type == CharType::UnaryPlus){
        CharType operation = advance().type;
        auto number = parseUnary();
        try{
            return std::make_unique<UnaryNode>(operation, std::move(number));
        }
        catch(...){
            throw std::runtime_error("Error with Unary token.");
        }
        
    }

    return parsePowers();
}

std::unique_ptr<Node> Parser::parseMiddle() {
        auto left = parseUnary();
        
        while (peek().type == CharType::Multiply || peek().type == CharType::Divide){
            CharType operation = advance().type;
            auto right = parseUnary();

            left = std::make_unique<BinaryNode>(operation, std::move(left), std::move(right));
        }
        return left;
    }

std::unique_ptr<Node> Parser::parseLow() {
    auto left = parseMiddle();
    while (peek().type == CharType::Add || peek().type == CharType::Subtract){
        CharType operation = advance().type;
        auto right = parseMiddle();

        left = std::make_unique<BinaryNode>(operation, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::parseAll() {
    auto left = parseLow();
    if (match(CharType::Equal)){
        auto right = parseLow();
        if (auto identifier = dynamic_cast<IdentifierNode*>(left.get())){
            return std::make_unique<AssignmentNode>(std::move(left), std::move(right));
        }
        if (auto function = dynamic_cast<FunctionNode*>(left.get())){
            std::vector<std::string> parameters {};
            for (auto& arg : function->arguments){
                auto id = dynamic_cast<IdentifierNode*>(arg.get());
                if (!(id)){
                    throw std::runtime_error("Function parameters must be identifiers.");
                }
                parameters.push_back(id->name);
            }

            return std::make_unique<UserFunction>(function->name, parameters, std::move(right));
        }
        throw std::runtime_error("Invalid left side of '='.");
        
    }
    if (match(CharType::Equal)){
        throw std::runtime_error("Extra equal sign.");
    }

    return left;
}