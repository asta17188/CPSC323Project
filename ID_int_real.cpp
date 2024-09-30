#include <iostream>
#include <fstream>
#include <string>
#include <regex>
// regex: regular expression object, can create a pattern
// regex_search: searchs for particular pattern in string, returns a boolean value
// smatch: holds results of regex matches when working with strings

#include "project1.cpp"


std::regex id_pattern("[a-zA-Z][a-zA-Z0-9]*[a-zA-Z]");
// starts with any letter, any letter or digit that can occur zero times or more, ends with any letter

std::regex int_pattern("[0-9]+");
// any int 0-9 that can occur once or more

std::regex real_pattern("[0-9]+\\.[0-9]+");
// any int 0-9 that can occur once or more, \\. refers to a dot (need two slashes cuz one backslash represents smthg else), any int 0-9 that can occur once or more


// note: must construct fsm (RE, nondet, then det.)
//       must write REs for Identifiers, Real, and Integer
//       must also show the NFSM using Thompson  
//       should have transition table at the end

std::ifstream srcFile; // grab input from file
std::smatch match; // holds results of regex matches (not sure if it even functions in code :P)

void IDs(std::string input, Token& type) { 
    // sequence of letters/digits, first & last characters must be letters
    // upper/lowercase letters are different
    if (std::regex_search(input, /*match,*/ id_pattern)) {
        type = ID;
        // std::cout << "identifier" << "         " << match.str() << "\n";
    }
}

void integers(std::string input, Token& type) {
    // integer division ignores any remainders
    // sequence of decimal digits
    if (std::regex_search(input, /*match,*/ int_pattern)) {
        type = INTEGER;
        // std::cout << "integer" << "         " << match.str() << "\n";
    }
}

void reals(std::string input, Token& type) {
    // integer followed by "."
    if (std::regex_search(input, /*match,*/ real_pattern)) {
        type = REAL;
        // std::cout << "real" << "         " << match.str() << "\n";
    }
}