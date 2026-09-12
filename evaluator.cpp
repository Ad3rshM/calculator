#include <iostream>
#include <stdexcept>
#include <numbers>
#include <cmath>
#include "evaluator.h"
#include "ast.h"
#include "utility.h"
#include "differentiator.h"

void handleError(bool condition, std::string_view error_message){
    if (condition){
        throw std::runtime_error(std::string(error_message));
    }
}

long double Evaluator::evaluate(const Node* node) {
    if (auto number = dynamic_cast<const NumberNode*>(node)){
        return number->value;
    }
    if (auto identifier = dynamic_cast<const IdentifierNode*>(node)){
        if (variables.contains(identifier -> name)){
            return variables[identifier -> name];
        }
        throw std::runtime_error("All variables must have been previously defined.");
    }

    if (auto user = dynamic_cast<const UserFunction*>(node)){
        UserDefinedFunc function(user->parameters, user->ast->clone());

        functions.insert_or_assign(user->name, std::move(function));

        return 0;
    }

    if (auto function = dynamic_cast<const FunctionNode*>(node)){
        return evaluate_function(function);
    }

    if (auto unary = dynamic_cast<const UnaryNode*>(node)){
        long double val = evaluate(unary -> middle.get());
        
        switch(unary->op){
            case CharType::UnaryMinus:
                return -val;
            
            case CharType::UnaryPlus:
                return val;

            default:
                throw std::runtime_error("Unknown operator.");
        }
    }

    if (auto assignment = dynamic_cast<const AssignmentNode*>(node)){
        auto left = dynamic_cast<const IdentifierNode*>(assignment -> left.get());
        if (!left){
            throw std::runtime_error("Left of equals sign must be variable.");
        }
        long double value = evaluate(assignment -> right.get());
        variables[left->name] = value;
        return value;
    }

    if (auto binary = dynamic_cast<const BinaryNode*>(node)){
        long double left = evaluate(binary->left.get());
        long double right = evaluate(binary->right.get());

        switch(binary->op){
            case CharType::Add:
                return left + right;
            case CharType::Subtract:
                return left - right;
            case CharType::Multiply:
                return left * right;
            case CharType::Divide:
                if (right != 0){
                    return left/right;
                }
                throw(std::runtime_error("Division by 0 not allowed."));
            case CharType::Power:
                return std::pow(left, right);
            default:
                throw std::runtime_error("Unknown operator.");
        }
    }
    throw std::runtime_error("Unknown expression.");
}

long double Evaluator::evaluate_function(const FunctionNode* function) {
    std::vector<long double> values {};
    for (const std::unique_ptr<Node>& arg : function->arguments) {
        values.push_back(evaluate(arg.get()));
    }

    const auto& userDef {functions.find(function->name)};

    if (userDef != functions.end()) {
        UserDefinedFunc& userFunc {userDef->second};
        std::vector<std::unique_ptr<IdentifierNode>> old_vars {};
        std::vector<std::unique_ptr<Node>> new_vars {};

        for (std::string var : userFunc.parameters) {
            old_vars.push_back(std::make_unique<IdentifierNode>(var));
        }
        for (long double value : values) {
            new_vars.push_back(std::make_unique<NumberNode>(value));
        }

        if (old_vars.size() > new_vars.size()) {
            throw std::runtime_error("Not enough parameters in user defined function.");
        }
        if (old_vars.size() < new_vars.size()) {
            throw std::runtime_error("Too many parameters in user defined function.");
        }

        std::unique_ptr<Node> substituted_ast = substitute(userFunc.ast.get(), old_vars, new_vars);

        long double body = evaluate(substituted_ast.get());

        return body;
    }

    const auto& builtinDef {builtins.find(function->name)};

    if (builtinDef != builtins.end()) {
        auto builtinFunc = builtinDef->second;
        return builtinFunc(values);
    }

    throw std::runtime_error("Not a function.");
}

std::variant<long double, std::string> Evaluator::evaluate_differentiate(const FunctionNode* function) {
    Differentiator differentiator;
    const auto& args = function->arguments;
    if (args.size() > 3) {
        throw std::runtime_error("diff() accepts three arguments max.");
    }
    if (args.size() >= 1) {
        const Node* expression = args[0].get();
    }
    
    else {
        throw std::runtime_error("diff() needs at least one argument.");
    }

    std::string diff_var {"x"};

    if (args.size() >= 2) {
        auto id = dynamic_cast<const IdentifierNode*>(args[1].get());
        if (!id) {
            throw std::runtime_error("Second argument to diff() must be variable.");
        }
        diff_var = id->name;
    }

    const Node* expression = args[0].get();

    std::unique_ptr<Node> differentiated = differentiator.differentiate(expression, diff_var);

    if (args.size() == 3) {
        auto num = dynamic_cast<const NumberNode*>(args[2].get());
        if (!num) {
            throw std::runtime_error("Third argument to diff() must be number.");
        }

        auto var = std::make_unique<IdentifierNode>(diff_var);

        std::vector<std::unique_ptr<Node>> new_vars {};
        std::vector<std::unique_ptr<IdentifierNode>> old_vars {};

        new_vars.push_back(std::make_unique<NumberNode>(num->value));
        old_vars.push_back(std::move(var));
        
        std::unique_ptr<Node> substitute_diff = substitute(differentiated.get(), old_vars, new_vars);
        return evaluate(substitute_diff.get());
    }

    return pretty_print(differentiated.get());

}