#include <iostream>
#include "evaluator.h"

Evaluator::Evaluator() {
    variables["pi"] = std::numbers::pi_v<long double>;
    variables["e"] = std::numbers::e_v<long double>;

    builtins["sin"] = [] (const auto& args) -> long double {
        handleError((args.size() != 1), "sin() expects one argument.");

        return std::sin(args[0]);
    };

    builtins["cos"] = [] (const auto& args) -> long double {
        handleError((args.size() != 1), "cos() expects one argument.");

        return std::cos(args[0]);
    };

    builtins["tan"] = [] (const auto& args) -> long double {
        handleError((args.size() != 1), "tan() expects one argument.");

        handleError((std::cos(args[0]) == 0), "tan() domain error.");

        return std::tan(args[0]);
    };

    builtins["csc"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "csc() expects one argument.");

        handleError(std::sin(args[0]) == 0, "csc() domain error.");

        return (1 / std::sin(args[0]));
    };

    builtins["sec"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "sec() expects one argument.");

        handleError(std::cos(args[0]) == 0, "sec() domain error.");

        return (1 / std::cos(args[0]));
    };

    builtins["cot"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "cot() expects one argument.");

        handleError(std::sin(args[0]) == 0, "cot() domain error.");

        return (std::cos(args[0]) / std::sin(args[0]));
    };

    builtins["asin"] = [] (const auto& args) -> long double {
        handleError((args.size() != 1), "asin() expects one argument.");

        handleError((args[0] < -1) || (args[0] > 1), "asin() domain error.");

        return std::asin(args[0]);
    };

    builtins["acos"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "acos() expects one argument.");

        handleError(args[0] < -1 || args[0] > 1, "acos() domain error.");

        return std::acos(args[0]);
    };

    builtins["atan"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "atan() expects one argument.");
        
        return std::atan(args[0]);
    };

    builtins["atan2"] = [] (const auto& args) -> long double {
        handleError((args.size() != 2), "atan2() expects two arguments.");

        handleError(args[0] == 0 && args[1] == 0, "atan2() domain error.");

        return std::atan2(args[0], args[1]);
    };

    builtins["sinh"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "sinh() expects one argument.");

        return std::sinh(args[0]);
    };

    builtins["cosh"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "cosh() expects one argument.");

        return std::cosh(args[0]);
    };

    builtins["tanh"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "tanh() expects one argument.");

        return std::tanh(args[0]);
    };

    builtins["asinh"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "asinh() expects one argument.");

        return std::asinh(args[0]);
    };

    builtins["acosh"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "acosh() expects one argument.");

        handleError(args[0] < 1, "acosh() domain error.");

        return std::acosh(args[0]);
    };

    builtins["atanh"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "atanh() expects one argument.");

        handleError(args[0] <= -1 || args[0] >= 1, "atanh() domain error.");

        return std::atanh(args[0]);
    };

    builtins["sqrt"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "sqrt() expects one argument.");

        handleError(args[0] < 0, "sqrt() domain error.");

        return std::sqrt(args[0]);
    };

    builtins["cbrt"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "cbrt() expects one argument.");

        return std::cbrt(args[0]);
    };

    builtins["abs"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "abs() expects one argument.");

        return std::abs(args[0]);
    };

    builtins["exp"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "exp() expects one argument.");

        return std::exp(args[0]);
    };

    builtins["ln"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "ln() expects one argument.");
        handleError(args[0] <= 0, "ln() domain error.");

        return std::log(args[0]);
    };

    builtins["log"] = [] (const auto& args) -> long double {
        if (args.size() == 1){
            handleError(args[0] <= 0, "log() domain error.");

            return std::log10(args[0]);
        }

        if (args.size() == 2){
            long double base {args[0]};
            long double x {args[1]};

            handleError(base <= 0 || base == 1, "Invalid logarithm base.");

            handleError(x <= 0, "log() domain error.");

            return std::log(x) / std::log(base);
        }

        throw std::runtime_error("log() expects one or two arguments.");
    };

    builtins["log10"] = builtins["log"];

    builtins["log2"] = [] (const auto& args) -> long double {
        handleError(args.size() != 1, "log2() expects one argument.");

        handleError(args[0] <= 0, "log2() domain error.");

        return std::log2(args[0]);
    };

    builtins["pow"] = [] (const auto& args) -> long double {
        handleError(args.size() != 2, "pow() expects two arguments.");

        handleError(args[0] < 0 && std::trunc(args[1]) != args[1], "pow() domain error.");

        return std::pow(args[0], args[1]);
    };
}