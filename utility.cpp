#include "utility.h"

std::unique_ptr<Node> substitute (const Node* node, std::vector<std::unique_ptr<IdentifierNode>>& old_vars, std::vector<std::unique_ptr<Node>>& new_vars) {

    if (auto id = dynamic_cast<const IdentifierNode*>(node)) {
        auto it = std::find_if(old_vars.begin(), old_vars.end(), [id](const std::unique_ptr<IdentifierNode>& var) {
            return var->name == id->name;
        });
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
        std::unique_ptr<Node> left = substitute(binary->left.get(), old_vars, new_vars);
        std::unique_ptr<Node> right = substitute(binary->right.get(), old_vars, new_vars);

        return std::make_unique<BinaryNode>(binary->op, std::move(left), std::move(right));
    }

    if (auto unary = dynamic_cast<const UnaryNode*>(node)) {
        std::unique_ptr<Node> child = substitute (unary->middle.get(), old_vars, new_vars);

        return std::make_unique<UnaryNode>(unary->op, std::move(child));
    }

    if (auto function = dynamic_cast<const FunctionNode*>(node)) {
        std::vector<std::unique_ptr<Node>> sub_args {};
        for (const std::unique_ptr<Node>& arg : function->arguments) {
            std::unique_ptr<Node> new_arg = substitute(arg.get(), old_vars, new_vars);
            sub_args.push_back(std::move(new_arg));
        }
        return std::make_unique<FunctionNode>(function->name, std::move(sub_args));
    }

    throw std::runtime_error("Invalid token.");
}

std::string pretty_print (const Node* node) {
    if (auto id = dynamic_cast<const IdentifierNode*>(node)) {
        return id->name;
    }

    if (auto num = dynamic_cast<const NumberNode*>(node)) {
        long double val = num->value;
        return std::format("{}", val);
    }

    if (auto binary = dynamic_cast<const BinaryNode*>(node)) {
        std::string left = pretty_print(binary->left.get());
        std::string right = pretty_print(binary->right.get());
        switch(binary->op) {
            case CharType::Add:
                return left + "+" + right;
            case CharType::Multiply:
                return "(" + left + ")" + "(" + right + ")";
            case CharType::Divide:
                return "(" + left + ")" + "/" + "(" + right + ")";
            case CharType::Subtract:
                return left + "-" + "(" + right + ")";
            case CharType::Power:
                return "(" + left + ")" + "^" + "(" + right + ")";
            default:
                throw std::runtime_error("Non-binary operation in binary node.");
        }
    }

    if (auto unary = dynamic_cast<const UnaryNode*>(node)) {
        std::string child = pretty_print(unary->middle.get());
        switch(unary->op) {
            case CharType::UnaryMinus:
                return "-(" + child + ")";
            case CharType::UnaryPlus:
                return child;
            default:
                throw std::runtime_error("Non-unary operation in unary node.");
        }
    }

    if (auto function = dynamic_cast<const FunctionNode*>(node)) {
        std::vector<std::string> pretty_print_args {};
        for (const std::unique_ptr<Node>& arg : function -> arguments) {
            pretty_print_args.push_back(pretty_print(arg.get()));
        }

        std::string pretty_print_function = function->name + "(";

        for (size_t i = 0; i < pretty_print_args.size(); i++) {
            if (i != 0) {
                pretty_print_function += ", ";
            }
            pretty_print_function += pretty_print_args[i];
        }

        pretty_print_function += ")";
        return pretty_print_function;
    }

    if (auto user = dynamic_cast<const UserFunction*>(node)) {
        std::string pretty_print_function = user->name + "(";

        for (size_t i = 0; i < user->parameters.size(); i++) {
            if (i != 0) {
                pretty_print_function += ", ";
            }
            pretty_print_function += user->parameters[i];
        }

        pretty_print_function += ")";
        return pretty_print_function;
    }

    if (auto assign = dynamic_cast<const AssignmentNode*>(node)) {
        std::string left = pretty_print(assign->left.get());
        std::string right = pretty_print(assign->right.get());
        return left + "=" + right;
    }

    throw std::runtime_error("Not a node.");
}