#include <iostream>

#include "lexer.h"
#include "parser.h"
#include "evaluator.h"

int main(){
    long double result {};
    Evaluator evaluator;
    while (true) {
        std::cout << ">> ";

        std::string input {};
        if(!std::getline(std::cin, input)){
            break;
        }

        auto spaces = input.find_last_not_of(" \t\n\r\f\v");

        if (spaces == std::string::npos){
            input.clear();
        }
        else{
            input.erase(spaces + 1);
        }

        if (input.empty()){
            continue;
        }

        if (input == "exit" || input == "quit"){
            break;
        }
        try {
            auto tokens = split_up(input);
            Parser parser(tokens);

            auto tree = parser.parseAll();
            
            if (parser.peek().type != CharType::End){
                throw std::runtime_error("Unexpected token at end of expression.");
            }
            result = evaluator.evaluate(tree.get());
            if (input.find('=') == std::string::npos){
                evaluator.variables["result"] = result;
                std::cout << "Result: " << result << '\n';
            }
        }
        catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << '\n';
        }
        catch(...) {
            std::cout << "Unexpected error.\n";
            break;
        }
    }

    std::cout << "Exiting application..." << std::endl;
    return 0;
}