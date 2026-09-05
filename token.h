#pragma once

#include <iostream>
#include <string>

enum class CharType{
    Number = 0,
    Identifier = 10,

    Add = 1,
    Subtract = 2,
    Multiply = 3,
    Divide = 4,
    Power = 5,

    UnaryMinus = -1,
    UnaryPlus = -2,

    Comma = 15,

    LeftParen = 20,
    RightParen = 21,

    Equal = 30,

    End = 100
};

struct Token{
    CharType type;
    std::string content;
};