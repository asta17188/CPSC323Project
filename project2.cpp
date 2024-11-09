// Project 2: Syntax Analyzer
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <iomanip>

// Declaring Enums for Token Types
enum Token
{
    OPERATOR,   // 0
    SEPARATOR,  // 1
    INTEGER,    // 2
    REAL,       // 3
    KEYWORD,    // 4
    ID,         // 5    
    UNKNOWN,    // 6
    DEFAULT     // 7

};

/*
    NOTE: When testing program, make sure to delete the output.txt file
          and create a new one. Otherwise, old (and possibly failed) attempts
          will still be on the output.txt when running the program again
          and create erroneous results.
*/

// Declaring Lists of Operators, Separators, Keywords, and Unknown Chars
const int ARRAY_SIZE = 11;
char unknownCharArray[ARRAY_SIZE] = {'!', '#', '$', '%', '&', '`', '?', '^', '_', '~'};

std::set<std::string> keywords{"if", "switch", "else","double","fi","new","return","asm",
                            "get","auto","while","extern","put","typedef","break","integer",
                            "break","boolean","operator","real","template","goto","enum",
                            "class", "this","function","case","string","throw","const","float",
                            "try","char","void","for","signed","register","unsigned","friend",
                            "catch","union","public","continue","protected","virtual","private",
                            "default","inline","sizeof","delete","int","static","volatile","do",
                            "long","struct","char16_t","char32_t","explicit","nullptr","static_cast",
                            "overload","short", "true", "false"};

std::set<std::string> operators{"=", "==", ">", "<", "<=",">=","!=","+","-","*","/"};


std::set<std::string> separators{"{","}","(",")",R"(")", ":"," ",";","::","\n","//","[","]","|",",", "@"};

// Function Prototypes
void findOperator(Token& , std::fstream& , std::string&);
void findOperator(Token& , std::fstream& , char&);
void findSeparator(Token& , std::fstream& , std::string&);
void findSeparator(Token& , std::fstream& , char&);
void findKeyword(Token&, std::fstream&, std::string&);
bool IDs(const std::string&);
bool integers(const std::string&);
bool reals(const std::string&);
void findUnknownChar(char&, std::string&, Token&, std::fstream&); 
template <typename T> void logLexeme(Token, std::fstream&, T&);          
void logLexeme(Token, std::fstream&, std::string&);  
void real_output();
void ids_output();
void int_output();

// Holds name of temp file, which will hold results from lexer
const std::string tempName = "temp.txt";

int main(int argc, char const *argv[])
{
    // Declare Local Variables
    std::ifstream srcFile;      // "Source" File
    char c;                     // Character in Current Loop
    Token type{DEFAULT};        // Type of Token
    std::string temp{""};       // Holds a string of int/real/id/keyword/multi-char unknown
    std::string fileName;
    bool isComment{false};      // True if detects beginning of comment
    bool isEndComment{false};   // True if detects end of comment

    // i.e. "testcase1.txt", "testcase2.txt", "testcase3.txt"
    std::cout << "Enter Test File Name: ";
    std::cin >> fileName;
        
    // Open Test File (src)
    srcFile.open(fileName);        
    if (!srcFile)
        throw std::invalid_argument("ERROR: FILE CANNOT BE FOUND, ENSURE YOU INPUT THE CORRECT FILE NAME");

    // i.e. "output.txt"
    std::cout << "Enter Output File Name: ";
    std:: cin >> fileName;

    // Declare "Destination" File
    std::fstream dstFile(fileName, std::ios::in | std::ios::out);     
    if (!dstFile)
        throw std::invalid_argument("ERROR: FILE CANNOT BE FOUND, ENSURE YOU INPUT THE CORRECT FILE NAME");

    // Declare Temp File
    std::fstream tempFile(tempName, std::ios::in | std::ios::out);
    if (!tempFile)
        throw std::invalid_argument("ERROR: FILE CANNOT BE FOUND, ENSURE TEMP FILE IS IN SAME FOLDER AS .EXE");

    // Print Transition Tables to Terminal
    real_output();
    ids_output();
    int_output();

    // While Loop: Run Through Test File Character By Character (!eof)
    while (!srcFile.eof())
    {
        c = srcFile.get();
        //std::cout << c << ' ';      // TEMP: This line is just for testing purposes, can be removed later

        // CODE START: Ignores comments (4 steps)
        if(isEndComment == true)   //  (4) Skips over ], the end of comment. Afterwards turns isEndComment back to false
        {
            isEndComment = false;
            continue;
        }
        if (srcFile.peek() == ']' && c == '*')  // (3) Detects the end of comments, skips *, marks end of comment true to skip ] in next iteration of loop. isComment is turned back to false
        {
            isComment = false;
            isEndComment = true;
            continue;
        }
        if(srcFile.peek() == '*' && c == '[')   // (1) Detects the start of comment, sets isComment true to skip following iterations until end of comment
        {
            isComment = true;
            continue;
        }
        if (isComment)  // (2) if isComment is true, then it skips following iterations until isComment is false
            continue;
        // CODE END

        /*
            If delimiter is met, it signals the end of the string.
            Afterwards, checks for int/real/id/keyword/multi-char unknown
        */
        if((c == ' ' || c == ';' || c == '}' || c == ')' || c == ']' || c == ',' || c == '*' || c == '(' || c == '\n') && temp != "")
        {
            // Integer Function
            if(integers(temp))
            {
                type = INTEGER;
                logLexeme(type, dstFile, temp);
            }

            // Real Function
            if(reals(temp))
            {
                type = REAL;
                logLexeme(type, dstFile, temp);
            }

            // Keyword Function
            findKeyword(type, dstFile, temp);
            
            // ID Function
            if(IDs(temp))
            {
                type = ID;
                logLexeme(type, dstFile, temp);
            }
            // Unknown Function
            if(type == DEFAULT)
            {
                type = UNKNOWN;
                logLexeme(type, dstFile, temp);
            }

            
            findSeparator(type, dstFile, c);
        }
        
        /*  OPERATOR FUNCTIONS
            If statements checks for multi-char operators (i.e. ==, <=, etc.)
            If Token type still DEFAULT, checks for single char operators
        */
        if (srcFile.peek() == '=' && c == '=')
        {
            type = OPERATOR;
            logLexeme(type, dstFile, "==");
        }
         if (srcFile.peek() == '=' && c == '>')
        {
            type = OPERATOR;
            logLexeme(type, dstFile, ">=");
        }
        if (srcFile.peek() == '=' && c == '<')
        {
            type = OPERATOR;
            logLexeme(type, dstFile, "<=");
        }
        if (srcFile.peek() == '=' && c == '!')
        {
            type = OPERATOR;
            logLexeme(type, dstFile, "!=");
        }
        if (type == DEFAULT)
            findOperator(type, dstFile, c);
        

        /*  SEPARATOR FUNCTIONS
            If statements checks for multi-char separators
            If Token type still DEFAULT, checks for single char separators
        */
        if (srcFile.peek() == ':' && c == ':')
        {
            logLexeme(SEPARATOR, dstFile, "::");
        }
        if (srcFile.peek() == '/' && c == '/')
        {
            logLexeme(SEPARATOR, dstFile, "//");
        }
        if(type == DEFAULT)     
            findSeparator(type, dstFile, c);
        
        // Unknown Character Function
        if (type == DEFAULT)
            findUnknownChar(c, temp, type, dstFile);
        

        // If Token type still DEFAULT at end of loop, 
        // adds current character to string (for multi-char lexemes)
        if (type == DEFAULT)
            temp += c;          // appends current character to string

        // Sets Token type back to DEFAULT at end of loop  
        type = DEFAULT;
    }
    // End of While Loop
    
    // Print Formatting Purposes
    std::cout << "\n\n";  

    // Print Output File Contents
    dstFile.seekg(0);
    while(getline(dstFile, temp))
        std::cout << temp << std::endl;
        
    // Close Test File (src)
    srcFile.close();
    // Close Output File
    dstFile.close();
    // Close Temp File
    tempFile.close();

    return 0;
}

template <typename T> void logLexeme(Token t, std::fstream& dst, T& lexeme)
{
    std::string str{lexeme};
    if (t == OPERATOR)
        dst << "TOKEN: Operator  \tLEXEME: " << lexeme << std::endl;
    else if(t == SEPARATOR)
    {
        // 10.27.24 - Modified func. to ignore spaces and newline
        if(str == "\n" || str == " ")
            return;
        else
            dst << "TOKEN: Separator\tLEXEME: " << lexeme << std::endl;
    }
            
    return;
}

void logLexeme(Token t, std::fstream& dst, std::string& s)
{
    switch (t)
    {
    case 2:
        dst << "TOKEN: Integer  \tLEXEME: " << s << std::endl;
        break;
    case 3: 
        dst << "TOKEN: Real     \tLEXEME: " << s << std::endl;
        break;
    case 4:
        dst << "TOKEN: Keyword  \tLEXEME: " << s << std::endl;
        break;
    case 5:
        dst << "TOKEN: ID       \tLEXEME: " << s << std::endl;
        break;
    default:
        dst << "TOKEN: Unknown  \tLEXEME: " << s << std::endl;
        break;
    }
        s = "";     // Resets temp string
    
    return;
}

void findOperator(Token& t, std::fstream& file, std::string& word)
{
    if(operators.find(word) != operators.end())
        t = OPERATOR;
        logLexeme(t, file, word);
    return; 
}
void findOperator(Token& t, std::fstream& file, char& ch)
{
    std::string chword(1, ch);
    if(operators.find(chword) != operators.end())
        t = OPERATOR;
        logLexeme(t, file, ch);
    return; 
}

void findSeparator(Token& t, std::fstream& file, std::string& word)
{
    if(separators.find(word) != separators.end())
        t = SEPARATOR;
        logLexeme(t, file, word);
    return; 
}

void findSeparator(Token& t, std::fstream& file, char& ch)
{
    std::string chword(1, ch);
    if(separators.find(chword) != separators.end())
        t = SEPARATOR;
    logLexeme(t, file, ch);
    return; 
}

void findUnknownChar(char& c, std::string& s, Token& t, std::fstream& dst)
{
    for (size_t i = 0; i < ARRAY_SIZE; i++)
    {
        if (unknownCharArray[i] == c)
        {
            t = UNKNOWN;
            dst << "Unknown  \t" << c << std::endl;
        }
    }
    return;
}

bool IDs(const std::string& lexeme) {
    int state = 1; // starting state
    // int accepting_state = 1; // accepting state

    if(lexeme.empty()) {
        return false;
    }

    for (size_t i = 0; i < lexeme.size(); ++i) {
        char character = lexeme[i];
        switch (state) // switches
        {
            case 1:
                if (isalpha(character)) {
                    state = 2;
                } else {
                    state = 13; // rejected
                }
                break;
            case 2:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 4;
                } else {
                    state = 13;
                }
                break;
            case 3:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)){
                    state = 5;
                }
                break;
            case 4:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 6;
                } else {
                    state = 13;
                }
                break;
            case 5:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 7;
                } else {
                    state = 13;
                }
                break;
            case 6:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 8;
                } else {
                    state = 13;
                }
                break;
            case 7:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 9;
                } else {
                    state = 13;
                }
                break;
            case 8:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 10;
                } else {
                    state = 13;
                }
                break;
            case 9:
                if(isalpha(character)) {
                    state = 10;
                } else if (isdigit(character)) {
                    state = 11;
                } else {
                    state = 13;
                }
                break;
            case 10:
                if(isalpha(character)) {
                    state = 10; // stay in laste letter state check
                } else if (isdigit(character)) {
                    state = 11; // go to digits state
                }
                break;
            case 11:
                if(isalpha(character)) {
                    state = 10; // go to letter state
                } else if (isdigit(character)) {
                    state = 11; // stay in digits state
                } else {
                    state = 13; // reject
                }
                break;
            case 13:
                return false;

            // case 0: // starting
            //     if (isalpha(character)) {
            //         state = 1; // state 1 if first character is a letter
            //     } else {
            //         return false; // means first char is not a letter
            //     }
            //     break;
            // case 1:
            //     if (isalnum(character)) { // if letter & or digit
            //         state = 1; // stay in this state
            //     } else {
            //         return false;
            //     }
            //     break;

        }
        // if (!isalpha(lexeme[lexeme.size() - 1])) {
        //     return false;
        // }
    }
    // return true;
    return (state == 10);
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

void findKeyword(Token& t, std::fstream& file, std::string& word)
{
    if(keywords.find(word) != keywords.end())
    {
        t = KEYWORD;
        logLexeme(t, file, word);
    }
    return; 
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
    std::cout << "IDs TRANSITION TABLE\n";
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
    std::cout << "2" << std::setw(5) << "|" << std::setw(5) << "2" << std::setw(10) << "\n\n";
}



