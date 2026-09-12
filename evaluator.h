#pragma once

#include <iostream>
#include <unordered_map>
#include <functional>
#include <string>
#include <variant>
#include "ast.h"

void handleError(bool, std::string_view);

class Evaluator{
    public:
        Evaluator();
        std::unordered_map<std::string, long double> variables;
        long double evaluate (const Node*);
        long double evaluate_function(const FunctionNode*);
        std::variant<long double, std::string> evaluate_differentiate (const FunctionNode*);
        std::unordered_map<std::string, std::function<long double(const std::vector<long double>&)>> builtins;
    private:
        std::unordered_map<std::string, UserDefinedFunc> functions;
};