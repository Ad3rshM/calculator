#include <iostream>
#include "ast.h"

std::unique_ptr<Node> fold_constants (const Node*);