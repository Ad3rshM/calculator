#include <iostream>
#include <memory>
#include "ast.h"

std::unique_ptr<Node> substitute (const Node*, std::vector<string>, std::vector<string>);