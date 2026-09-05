#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "token.h"

struct Node{
    virtual ~Node() = default;

    virtual std::unique_ptr<Node> clone() const = 0;
};

struct NumberNode : Node{
    long double value;
    
    NumberNode(long double val):
        value(std::move(val)) {}
    
    std::unique_ptr<Node> clone() const override {
        return std::make_unique<NumberNode>(value);
    }
};

struct IdentifierNode : Node{
    std::string name;

    IdentifierNode(std::string name):
        name(std::move(name)) {}

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<IdentifierNode>(name);
    }
};

struct AssignmentNode : Node{
    std::unique_ptr<Node> left {};
    std::unique_ptr<Node> right{};

    AssignmentNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right):
        left(std::move(left)), right(std::move(right)) {}

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<AssignmentNode>(left->clone(), right->clone());
    }
};

struct FunctionNode : Node {
    std::string name {};
    std::vector<std::unique_ptr<Node>> arguments {};

    FunctionNode(std::string name, std::vector<std::unique_ptr<Node>> arguments):
        name(std::move(name)), arguments(std::move(std::move(arguments))) {}

    std::unique_ptr<Node> clone() const override {
        std::vector<std::unique_ptr<Node>> new_args{};
        for (const auto& arg : arguments){
            new_args.push_back(arg->clone());
        }
        return std::make_unique<FunctionNode>(name, std::move(new_args));
    }
};

struct UserFunction : Node {
    std::string name{};
    std::vector<std::string> parameters {};
    std::unique_ptr<Node> ast {};
    UserFunction(std::string name, std::vector<std::string> parameters, std::unique_ptr<Node> ast):
        name(std::move(name)), parameters(std::move(parameters)), ast(std::move(std::move(ast))) {}

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<UserFunction>(name, parameters, ast->clone());
    }
};

struct BinaryNode : Node {
    CharType op {};
    std::unique_ptr<Node> left {};
    std::unique_ptr<Node> right {};

    BinaryNode(CharType op, std::unique_ptr<Node> left, std::unique_ptr<Node> right):
        op(std::move(op)), left(std::move(left)), right(std::move(right)) {}
    
    std::unique_ptr<Node> clone() const override {
        return std::make_unique<BinaryNode>(op, left->clone(), right->clone());
    }
};

struct UnaryNode : Node {
    CharType op {};
    std::unique_ptr<Node> middle {};
    
    UnaryNode(CharType op, std::unique_ptr<Node> middle):
    op(std::move(op)), middle(std::move(middle)) {}

    std::unique_ptr<Node> clone() const override {
        return std::make_unique<UnaryNode>(op, middle->clone());
    }
};

struct UserDefinedFunc {
    std::vector<std::string> parameters {};
    std::unique_ptr<Node> ast {};
    UserDefinedFunc(std::vector<std::string> parameters, std::unique_ptr<Node> ast):
        parameters(std::move(parameters)), ast(std::move(std::move(ast))) {}
};