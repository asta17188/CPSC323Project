#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "project1.cpp"


// bool IDs(const std::string& lexeme) {
//     int state = 1; // starting state
//     if(lexeme.empty()) {
//         return false;
//     }
//     for (size_t i = 0; i < lexeme.size(); ++i) {
//         char character = lexeme[i];
//         switch (state) // switches
//         {
//             case 1:
//                 if (isalpha(character)) {
//                     state = 2;
//                 } else {
//                     state = 13; // rejected
//                 }
//                 break;
//             case 2:
//                 if(isalpha(character)) {
//                     state = 10;
//                 } else if (isdigit(character)) {
//                     state = 4;
//                 } else {
//                     state = 13;
//                 }
//                 break;
//             case 3:
//                 if(isalpha(character)) {
//                     state = 10;
//                 } else if (isdigit(character)){
//                     state = 5;
//                 }
//                 break;
//             case 4:
//                 if(isalpha(character)) {
//                     state = 10;
//                 } else if (isdigit(character)) {
//                     state = 6;
//                 } else {
//                     state = 13;
//                 }
//                 break;
//             case 5:
//                 if(isalpha(character)) {
//                     state = 10;
//                 } else if (isdigit(character)) {
//                     state = 7;
//                 } else {
//                     state = 13;
//                 }
//                 break;
//             case 6:
//                 if(isalpha(character)) {
//                     state = 10;
//                 } else if (isdigit(character)) {
//                     state = 8;
//                 } else {
//                     state = 13;
//                 }
//                 break;
//             case 7:
//                 if(isalpha(character)) {
//                     state = 10;
//                 } else if (isdigit(character)) {
//                     state = 9;
//                 } else {
//                     state = 13;
//                 }
//                 break;
//             case 8:
//                 if(isalpha(character)) {
//                     state = 10;
//                 } else if (isdigit(character)) {
//                     state = 10;
//                 } else {
//                     state = 13;
//                 }
//                 break;
//             case 9:
//                 if(isalpha(character)) {
//                     state = 10;
//                 } else if (isdigit(character)) {
//                     state = 11;
//                 } else {
//                     state = 13;
//                 }
//                 break;
//             case 10:
//                 if(isalpha(character)) {
//                     state = 10; // stay in laste letter state check
//                 } else if (isdigit(character)) {
//                     state = 11; // go to digits state
//                 }
//                 break;
//             case 11:
//                 if(isalpha(character)) {
//                     state = 10; // go to letter state
//                 } else if (isdigit(character)) {
//                     state = 11; // stay in digits state
//                 } else {
//                     state = 13; // reject
//                 }
//                 break;
//             case 13:
//                 return false;
//         }
//     }
//     return (state == 10);
// }


bool IDs(const std::string& lexeme) {
    int state = 0; // starting state
    // int accepting_state = 1; // accepting state

    if(lexeme.empty()) {
        return false;
    }

    for (size_t i = 0; i < lexeme.size() - 1; ++i) {
        char character = lexeme[i];
        switch (state)
        {
            case 0: // starting
                if (isalpha(character)) {
                    state = 1; // state 1 if first character is a letter
                } else {
                    return false; // means first char is not a letter
                }
                break;
            case 1:
                if (isalnum(character)) { // if letter & or digit
                    state = 1; // stay in this state
                } else {
                    return false;
                }
                break;
        }
        if (!isalpha(lexeme[lexeme.size() - 1])) {
            return false;
        }
    }
    return true;
}

bool integers(const std::string& lexeme) {
    if(lexeme.empty()) {
        return false;
    }

    int state = 0;
    int accepting_state = 1;

    for (char character : lexeme) {
        switch (state)
        {
        case 0:
            if (isdigit(character)) {
                state = 1; //goes to integer state
            } else {
                return false;
            }
            break;
        case 1:
            if (isdigit(character)) {
                state = 1; //stays in this state
            } else {
                return false; 
            }
            break;
        }
    }
    return (state==accepting_state);
}

bool reals(const std::string& lexeme) {
    if(lexeme.empty()) {
        return false;
    }

    int state = 0;
    int accepting_state = 3;

    for (char character : lexeme) {
        switch (state)
        {
        case 0: //starting
            if (isdigit(character)) {
                state = 1;
            } else {
                return false;
            }
            break;
        case 1: // integer state
            if (isdigit(character)) {
                state = 1; // stays in integer state
            } else if (character == '.') {
                state = 2; //goes to decimal state
            } else {
                return false;
            }
            break;
        case 2:
            if (isdigit(character)) {
                state = 3;
            } else {
                return false;
            }
            break;
        case 3:
            if (isdigit(character)) {
                state = 3;
            } else {
                return false;
            }
            break;
        }
    }
    return (state == accepting_state);
}

void real_output() {
    std::cout << "REALS TRANSITION TABLE\n";
    std::cout << " " << std::setw(5) << "|" << std::setw(5) << "d" << std::setw(10) << ".\n";
    std::cout << "-----|--------------------\n";
    std::cout << "1" << std::setw(5) << "|" << std::setw(5) << "2" << std::setw(10) << "5\n";
    std::cout << "2" << std::setw(5) << "|" << std::setw(5) << "2" << std::setw(10) << "3\n";
    std::cout << "3" << std::setw(5) << "|" << std::setw(5) << "4" << std::setw(10) << "5\n";
    std::cout << "4" << std::setw(5) << "|" << std::setw(5) << "4" << std::setw(10) << "5\n";
    std::cout << "5" << std::setw(5) << "|" << std::endl;
}

void ids_output() {
    std::cout << "IDS TRANSITION TABLE\n";
    std::cout << " " << std::setw(5) << "|" << std::setw(5) << "l" << std::setw(10) << "d\n";
    std::cout << "-----|--------------------\n";
    std::cout << "1" << std::setw(5) << "|" << std::setw(5) << "2" << std::setw(10) << "13\n";
    std::cout << "2" << std::setw(5) << "|" << std::setw(5) << "3" << std::setw(10) << "13\n";
    std::cout << "3" << std::setw(5) << "|" << std::setw(5) << "4" << std::setw(10) << "13\n";
    std::cout << "4" << std::setw(5) << "|" << std::setw(5) << "5" << std::setw(10) << "6\n";
    std::cout << "5" << std::setw(5) << "|" << std::setw(5) << "7" << std::setw(10) << "13\n";
    std::cout << "6" << std::setw(5) << "|" << std::setw(5) << "13" << std::setw(10) << "8\n";
    std::cout << "7" << std::setw(5) << "|" << std::setw(5) << "9" << std::setw(10) << "13\n";
    std::cout << "8" << std::setw(5) << "|" << std::setw(5) << "10" << std::setw(10) << "12\n";
    std::cout << "9" << std::setw(5) << "|" << std::setw(5) << "9" << std::setw(10) << "6\n";
    std::cout << "10" << std::setw(4) << "|" << std::setw(5) << "11" << std::setw(10) << "13\n";
    std::cout << "11" << std::setw(4) << "|" << std::setw(5) << "7" << std::setw(10) << "6\n";
    std::cout << "12" << std::setw(4) << "|" << std::setw(5) << "10" << std::setw(10) << "6\n";
    std::cout << "13" << std::setw(4) << "|" << std::endl;
}

void int_output() {
    std::cout << "INTEGERS TRANSITION TABLE\n";
    std::cout << " " << std::setw(5) << "|" << std::setw(5) << "d" << std::setw(10) << "\n";
    std::cout << "-----|----------\n";
    std::cout << "1" << std::setw(5) << "|" << std::setw(5) << "2" << std::setw(10) << "\n";
    std::cout << "2" << std::setw(5) << "|" << std::setw(5) << "2" << std::setw(10) << std::endl;
}