#include <iostream>
#include <memory>
#include <unordered_map>
#include "ast.h"

std::unique_ptr<Node> substitute (const Node*, std::vector<std::unique_ptr<IdentifierNode>>&, std::vector<std::unique_ptr<Node>>&);

std::string pretty_print (const Node*);