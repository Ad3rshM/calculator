#include <iostream>
#include "constant_folder.h"
#include "evaluator.h"
#include "utility.h"

std::unique_ptr<Node> fold_constants (const Node* node) {
    if (auto binary = dynamic_cast<const BinaryNode*>(node)) {
        std::unique_ptr<Node> folded_left = fold_constants(binary->left.get());
        std::unique_ptr<Node> folded_right = fold_constants(binary->right.get());

        auto left = dynamic_cast<const NumberNode*>(folded_left.get());
        auto right = dynamic_cast<const NumberNode*>(folded_right.get());

        if (!left || !right) {
            return std::make_unique<BinaryNode>(binary->op, folded_left, folded_right);
        }

        long double l_val = folded_left->value;
        long double r_val = folded_right->value;

        switch(binary->op) {
            case CharType::Add:
                return std::make_unique<NumberNode>(l_val + r_val);
            case CharType::Multiply:
                return std::make_unique<NumberNode>(l_val * r_val);
            case CharType::Subtract:
                return std::make_unique<NumberNode>(l_val - r_val);
            case CharType::Divide:
                if (r_val == 0) {
                    throw std::runtime_error ("Division by 0 not allowed.");
                }
                return std::make_unique<NumberNode>(l_val / r_val);
            case CharType::Power:
                return std::make_unique<NumberNode>(std::pow(l_val, r_val));
            default:
                throw std::runtime_error("Non-binary operation in binary node.");
        }
    }

    if (auto unary = dynamic_cast<const UnaryNode*>(node)) {
        std::unique_ptr<Node> folded_child = fold_constants(unary->middle.get());

        if (unary->op == CharType::UnaryPlus) {
            return folded_child;
        }

        auto child = dynamic_cast<const NumberNode*>(folded_child.get());

        if (!child) {
            return std::make_unique<UnaryNode>(unary->op, folded_child);
        }

        long double c_val = folded_child->value;

        switch(unary->op) {
            case CharType::UnaryMinus:
                return std::make_unique<NumberNode>(-1 * c_val);
            default:
                throw std::runtime_error("Non-unary operation in unary node.");
        }
    }

    if (auto id = dynamic_cast<const IdentifierNode*>(node)) {
        return variables[id->name];
    }

    if (auto func = dynamic_cast<const FunctionNode*>(node)) {
        bool is_constant {true};
        std::vector<std::unique_ptr<Node>> folded_args {};
        for (const std::unique_ptr<Node>& arg : function -> arguments) {
            std::unique_ptr<Node> folded_arg = fold_constants(arg.get())
            if (!(auto arg_num = dynamic_cast<const NumberNode*>(folded_arg))) {
                is_constant = false;
            }
            folded_args.push_back();
        }
        if (!is_constant) {
            return std::make_unique<FunctionNode>(func->name, folded_args);
        }

        std::vector<long double> arg_vals {};
        for (const std::unique_ptr<Node>& f_arg : folded_args) {
            arg_vals.push_back(f_arg->value);
        }

        return builtins[func->name] (arg_vals);
    }

    if (auto user = dynamic_cast<const UserFunction*>(node)) {
        std::vector<std::unique_ptr<IdentifierNode>> param_vars {};
        for (std::string i : user->parameters) {
            param_vars.push_back(std::make_unique<IdentifierNode>(i));
        }

        
    }
}