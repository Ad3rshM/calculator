#include "utility.h"

std::unique_ptr<Node> substitute (const Node* node, std::vector<std::unique_ptr<Node>> old_vars, std::vector<std::unique_ptr<Node>> new_vars) {

    if (auto id = dynamic_cast<const IdentifierNode*>(node)) {
        auto it = std::find(old_vars.begin(), old_vars.end(), id)
        if (it != old_vars.end()) {
            auto index = it-old_vars.begin();
            return new_vars[index] -> clone();
        }

        return id->clone();
    }

    if (auto number = dynamic_cast<const NumberNode*>(node)) {
        return number->clone();
    }

    if (auto binary = dynamic_cast<const BinaryNode*>(node)) {
        std::unique_ptr<Node> left = substitute(binary->left->clone(), old_vars, new_vars);
        std::unique_ptr<Node> right = substitute(binary->right->clone(), old_vars, new_vars);

        return std::make_unique<BinaryNode>(binary->op, std::move(left), std::move(right));
    }

    if (auto unary = dynamic_cast<const UnaryNode*>(node)) {
        std::unique_ptr<Node> child = substitute (unary->middle->clone(), old_vars, new_vars);

        return std::make_unique<UnaryNode>(unary->op, std::move(child));
    }

    if (auto function = dynamic_cast<const FunctionNode*>(node)) {
        std::vector<std::unique_ptr<Node>> sub_args {};
        for (std::unique_ptr<Node> arg : function->arguments) {
            std::unique_ptr<Node> new_arg = substitute(arg.get(), old_vars, new_vars)
        }
    }
}