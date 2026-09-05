#include <iostream>
#include <optional>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

#include "lexer.h"
#include "token.h"
#include "evaluator.h"

std::optional<Token> isBinaryOperation(char character){
    switch(character){
        case '+':
            return Token{CharType::Add, "+"};
        case '-':
            return Token{CharType::Subtract, "-"};
        case '*':
            return Token{CharType::Multiply, "*"};
        case '/':
            return Token{CharType::Divide, "/"};
        case '^':
            return Token{CharType::Power, "^"};
        case '(':
            return Token{CharType::LeftParen, "("};
        case ')':
            return Token{CharType::RightParen, ")"};
        case '=':
            return Token{CharType::Equal, "="};
        case ',':
            return Token{CharType::Comma, ","};
        default:
            return std::nullopt;
    }
}

std::optional<Token> isUnaryOperation(char character){
    switch(character){
        case '-':
            return Token{CharType::UnaryMinus, "-"};
        case '+':
            return Token{CharType::UnaryPlus, "+"};
        default:
            return std::nullopt;
    }
}

bool ends_expression(CharType type){
    return type == CharType::Number ||
        type == CharType::RightParen;
    }

bool starts_expression(CharType type){
    return type == CharType::Number ||
        type == CharType::Identifier ||
        type == CharType::LeftParen;
}

bool expecting_unary(CharType type){
    return !(ends_expression(type) ||
        type == CharType::Identifier);
}

std::vector<Token> split_up(const std::string& input){
    std::vector<Token> output {};
    output.reserve(input.size());
    std::string temp {};

    auto addToken = [&](Token token){
        if (!output.empty() && starts_expression(token.type) && ends_expression(output.back().type)){
            output.push_back(Token{CharType::Multiply, "*"});
        }
        output.push_back(token);
    };

    for (size_t i {0}; i<input.size(); i++){
        if (std::isspace(static_cast<unsigned char>(input[i]))){
            continue;
        }

        auto isUnary = isUnaryOperation(input[i]);
        if ((isUnary.has_value()) && (output.empty() || expecting_unary(output.back().type))){
            addToken(isUnary.value());
            continue;
        }

        auto isBinary = isBinaryOperation(input[i]);
        if (isBinary.has_value()){
            addToken(isBinary.value());
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(input[i])) || input[i] == '_'){
            while (i<input.size() && (std::isalnum(static_cast<unsigned char>(input[i])) || input[i] == '_')){
                temp += input[i];
                i++;
            }
            i--;
            std::transform(temp.begin(), temp.end(), temp.begin(), [](unsigned char c){
                return std::tolower(c);
            });
            addToken(Token{CharType::Identifier, temp});
            temp.clear();
            continue;
        }
        
        if (std::isdigit(static_cast<unsigned char>(input[i]))){
            bool decimal {false};
            while (i<input.size() && (std::isdigit(static_cast<unsigned char>(input[i])) || (!decimal && input[i] == '.'))) {
                if (input[i] == '.'){
                    decimal = true;
                }
                temp += input[i];
                i++;
            }
            i--;
            addToken(Token{CharType::Number, temp});
            temp.clear();
            continue;
        }

        std::ostringstream invalid_character;
        invalid_character << "Invalid Character " << input[i] << "at" << i << ".";
        throw std::runtime_error(invalid_character.str());
    }

    output.push_back(Token{CharType::End, ""});

    return output;
}