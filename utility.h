#include <iostream>
#include <memory>
#include "ast.h"

std::unique_ptr<Node> substitute (const Node*, std::vector<std::unique_ptr<IdentifierNode>>&, std::vector<std::unique_ptr<Node>>&);