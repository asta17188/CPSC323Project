#include <iostream>
#include <fstream>
#include <string>
// #include <regex>
// regex: regular expression object, can create a pattern
// regex_search: searchs for particular pattern in string, returns a boolean value
// smatch: holds results of regex matches when working with strings

#include "project1.cpp"


// std::regex id_pattern("[a-zA-Z][a-zA-Z0-9]*[a-zA-Z]");
// starts with any letter, any letter or digit that can occur zero times or more, ends with any letter

// std::regex int_pattern("[0-9]+");
// any int 0-9 that can occur once or more

// std::regex real_pattern("[0-9]+\\.[0-9]+");
// any int 0-9 that can occur once or more, \\. refers to a dot (need two slashes cuz one backslash represents smthg else), any int 0-9 that can occur once or more


// note: must construct fsm (RE, nondet, then det.)
//       must write REs for Identifiers, Real, and Integer
//       must also show the NFSM using Thompson  
//       should have transition table at the end

// std::ifstream srcFile; // grab input from file
// std::smatch match; // holds results of regex matches (not sure if it even functions in code :P)

void IDs(std::string& input, Token& type, std::fstream&) {

    int i=0;

    if (input.size() == 0) {
        return; // not valid string size of 0
    } else if (!isalpha(input[i])) {
        return; // if it is not alphabetic, not a valid string
    }

    for (i = 1; i < input.size() - 1; i++) {
        if (!isalnum(input[i])) {
            return; // if it is not alphabetic/numeric, then return, not valid string
        }
    }

    if (isalpha(input[input.size()-1])) {
            type = ID;
    }

    

    // sequence of letters/digits, first & last characters must be letters
    // upper/lowercase letters are different
    // if (std::regex_search(input, /*match,*/ id_pattern)) {
        // type = ID;
        // std::cout << "identifier" << "         " << match.str() << "\n";
    // }
}

void integers(std::string& input, Token& type, std::fstream&) {

    int i = 0;

    if (input.size() == 0) {
        return; // not valid string size of 0
    }

    for (i = 0; i < input.size(); i++) {
        if(!isdigit(input[i])) {
            return; // if there is not an integer, invalid integer
        }
    }

    type = INTEGER;

    // integer division ignores any remainders
    // sequence of decimal digits
    // if (std::regex_search(input, /*match,*/ int_pattern)) {
        // type = INTEGER;
        // std::cout << "integer" << "         " << match.str() << "\n";
    // }
}

void reals(std::string& input, Token& type, std::fstream&) {

    int i = 0;

    if (input.size() == 0) {
        return; // not valid string size of 0
    }

    while (i < input.size() && isdigit(input[i])) {
        i++; // increment i if the size is valid and if the item is a digit
    }

    if (i < input.size() && input[i] == '.') {
        i++; // if size is valid and there is a decimal at i's location, increment i
    } else {
        return; // if there is no decimal, it is not a real
    }

    while (i < input.size() && isdigit(input[i])) {
        i++; // repeat the process again for integers after the decimal
    }

    if (i == input.size()) {
        type = REAL; // if we checked the entire string and it is valid, type=real
    }

    // integer followed by "."
    // if (std::regex_search(input, /*match,*/ real_pattern)) {
        // type = REAL;
        // std::cout << "real" << "         " << match.str() << "\n";
    // }
}