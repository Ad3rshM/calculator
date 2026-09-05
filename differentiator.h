#include <iostream>
#include "ast.h"

class Differentiator {
    public:
        std::unique_ptr<Node> differentiate (const Node*, const std::string&);
};