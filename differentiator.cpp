#include <iostream>
#include <unordered_map>

#include "differentiator.h"

std::unique_ptr<Node> Differentiator::differentiate(const Node* node, const std::string& variable){
    if (auto binary = dynamic_cast<const BinaryNode*>(node)){
        std::unique_ptr<Node> leftPrime = differentiate(binary->left.get(), variable);
        std::unique_ptr<Node> rightPrime = differentiate(binary->right.get(), variable);

        if (binary->op == CharType::Add){
            return std::make_unique<BinaryNode>(CharType::Add, std::move(leftPrime), std::move(rightPrime));
        }

        if (binary->op == CharType::Subtract){
            return std::make_unique<BinaryNode>(CharType::Subtract, std::move(leftPrime), std::move(rightPrime));
        }

        if(binary->op == CharType::Multiply) {
            std::unique_ptr<Node> leftMultiplication = std::make_unique<BinaryNode>(CharType::Multiply, std::move(leftPrime), binary->right->clone());
            std::unique_ptr<Node> rightMultiplication = std::make_unique<BinaryNode>(CharType::Multiply, binary->left->clone(), std::move(rightPrime));
            return std::make_unique<BinaryNode>(CharType::Add, std::move(leftMultiplication), std::move(rightMultiplication));
        }

        if (binary->op == CharType::Divide) {
            std::unique_ptr<Node> leftTop = std::make_unique<BinaryNode>(CharType::Multiply, std::move(leftPrime), binary->right->clone());
            std::unique_ptr<Node> rightTop = std::make_unique<BinaryNode>(CharType::Multiply, binary->left->clone(), std::move(rightPrime));
            std::unique_ptr<Node> top = std::make_unique<BinaryNode>(CharType::Subtract, std::move(leftTop), std::move(rightTop));
            std::unique_ptr<Node> bottom = std::make_unique<BinaryNode>(CharType::Power, binary->right->clone(), std::make_unique<NumberNode>(2));
            return std::make_unique<BinaryNode>(CharType::Divide, std::move(top), std::move(bottom));
        }

        if (binary->op == CharType::Power){
            if (auto exponent = dynamic_cast<const NumberNode*>(binary->right.get())) {
                std::unique_ptr<Node> power = std::make_unique<BinaryNode>(CharType::Power, binary->left->clone(), std::make_unique<NumberNode>(exponent->value-1));
                std::unique_ptr<Node> coefficient = std::make_unique<BinaryNode>(CharType::Multiply, binary->right->clone(), std::move(power));
                return std::make_unique<BinaryNode>(CharType::Multiply, std::move(coefficient), std::move(leftPrime));
            }
            else {
                std::vector<std::unique_ptr<Node>> lnf {};
                lnf.push_back(binary->left->clone());

                std::unique_ptr<Node> leftLn = std::make_unique<FunctionNode>("ln", std::move(lnf));
                std::unique_ptr<Node> leftFactor = std::make_unique<BinaryNode>(CharType::Multiply, std::move(rightPrime), std::move(leftLn));

                std::unique_ptr<Node> rightFactorFrac = std::make_unique<BinaryNode>(CharType::Divide, std::move(leftPrime), binary->left->clone());
                std::unique_ptr<Node> rightFactor = std::make_unique<BinaryNode>(CharType::Multiply, binary->right->clone(), std::move(rightFactorFrac));

                std::unique_ptr<Node> factor = std::make_unique<BinaryNode>(CharType::Add, std::move(leftFactor), std::move(rightFactor));
                std::unique_ptr<Node> coefficient = std::make_unique<BinaryNode>(CharType::Power, binary->left->clone(), binary->right->clone());

                return std::make_unique<BinaryNode>(CharType::Multiply, std::move(coefficient), std::move(factor));
            }
        }

        throw std::runtime_error("Unknown binary operator.");
    }

    if (auto function = dynamic_cast<const FunctionNode*>(node)){
        if (function->name == "sin") {
            std::unique_ptr<Node> inside = function->arguments[0]->clone();
            std::vector<std::unique_ptr<Node>> args;
            args.push_back(inside->clone());

            std::unique_ptr<Node> insidePrime = differentiate(inside.get(), variable);
            std::unique_ptr<Node> sin_cos = std::make_unique<FunctionNode>("cos", std::move(args));

            return std::make_unique<BinaryNode>(CharType::Multiply, std::move(sin_cos), std::move(insidePrime));
        }

        if (function->name == "cos") {
            std::unique_ptr<Node> inside = function->arguments[0]->clone();
            std::vector<std::unique_ptr<Node>> args;
            args.push_back(inside->clone());

            std::unique_ptr<Node> insidePrime = differentiate(inside.get(), variable);
            std::unique_ptr<Node> cos_sin = std::make_unique<FunctionNode>("sin", std::move(args));
            std::unique_ptr<Node> coefficient = std::make_unique<UnaryNode>(CharType::UnaryMinus, std::move(cos_sin));

            return std::make_unique<BinaryNode>(CharType::Multiply, std::move(coefficient), std::move(insidePrime));
        }

        if (function->name == "tan") {
            std::unique_ptr<Node> inside = function->arguments[0]->clone();
            std::vector<std::unique_ptr<Node>> args;
            args.push_back(inside->clone());

            std::unique_ptr<Node> insidePrime = differentiate(inside.get(), variable);
            std::unique_ptr<Node> tan_sec = std::make_unique<FunctionNode>("sec", std::move(args));
            std::unique_ptr<Node> coefficient = std::make_unique<BinaryNode>(
                CharType::Power,
                std::move(tan_sec),
                std::make_unique<NumberNode>(2)
            );

            return std::make_unique<BinaryNode>(CharType::Multiply, std::move(coefficient), std::move(insidePrime));
        }

        if (function->name == "asin") {
            std::unique_ptr<Node> inside = function->arguments[0]->clone();
            std::vector<std::unique_ptr<Node>> args;
            args.push_back(inside->clone());

            std::unique_ptr<Node> insidePrime = differentiate(inside.get(), variable);
            std::unique_ptr<Node> insideSquared = std::make_unique<BinaryNode>(
                CharType::Power,
                inside -> clone(),
                std::make_unique<NumberNode>(2)
            );

            std::unique_ptr<Node> sqrt_arg = std::make_unique<BinaryNode>(
                CharType::Subtract,
                std::make_unique<NumberNode>(1),
                std::move(insideSquared)
            );
            std::vector<std::unique_ptr<Node>> bottom_args {};
            bottom_args.push_back(std::move(sqrt_arg));

            std::unique_ptr<Node> bottom = std::make_unique<FunctionNode>("sqrt", std::move(bottom_args));
            
            return std::make_unique<BinaryNode>(
                CharType::Divide,
                std::move(insidePrime),
                std::move(bottom)
            );
        }

        if (function->name == "sinh") {
            std::unique_ptr<Node> inside = function->arguments[0]->clone();
            std::vector<std::unique_ptr<Node>> args;
            args.push_back(inside->clone());

            std::unique_ptr<Node> insidePrime = differentiate(inside.get(), variable);
            std::unique_ptr<Node> sinh_cosh = std::make_unique<FunctionNode>("cosh", std::move(args));

            return std::make_unique<BinaryNode>(CharType::Multiply, std::move(sinh_cosh), std::move(insidePrime));
        }

        throw std::runtime_error("Unknown differentiable function.");
    }

    if (auto number = dynamic_cast<const NumberNode*>(node)){
        return std::make_unique<NumberNode>(0);
    }

    if (auto identifier = dynamic_cast<const IdentifierNode*>(node)){
        if (identifier->name == variable){
            return std::make_unique<NumberNode>(1);
        }
        return std::make_unique<NumberNode>(0);
    }

    if (auto unary = dynamic_cast<const UnaryNode*>(node)){
        if (unary->op == CharType::UnaryPlus) {
            return differentiate(unary->middle.get(), variable);
        }
        if (unary->op == CharType::UnaryMinus) {
            std::unique_ptr<Node> child = differentiate(unary->middle.get(), variable);
            return std::make_unique<UnaryNode>(CharType::UnaryMinus, std::move(child));
        }
    }

    if (auto user = dynamic_cast<const UserDefinedFunc*>(node)){
        return differentiate(user->ast.get(), variable);
    }

    throw std::runtime_error("Invalid derivative argument.");
}
