// Project 1: Syntax Analyzer
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <iomanip>

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
const int ARRAY_SIZE = 11;
char unknownCharArray[ARRAY_SIZE] = {'!', '#', '$', '%', '&', '`', '?', '^', '_', '~'};

// 10.23.24 - Missing commas added after "asm" and "integer"
std::set<std::string> keywords{"if", "switch", "else","double","fi","new","return","asm",
                            "get","auto","while","extern","put","typedef","break","integer",
                            "break","boolean","operator","real","template","goto","enum",
                            "class", "this","function","case","string","throw","const","float",
                            "try","char","void","for","signed","register","unsigned","friend",
                            "catch","union","public","continue","protected","virtual","private",
                            "default","inline","sizeof","delete","int","static","volatile","do",
                            "long","struct","char16_t","char32_t","explicit","nullptr","static_cast",
                            "overload","short"};

std::set<std::string> operators{"=", "==", ">", "<", "<=",">=","!=","+","-","*","/"};

// 10.23.24 - Moved "@" symbol from unknowns to separators 
std::set<std::string> separators{"{","}","(",")",R"(")", ":"," ",";","::","\n","//","[","]","|",",", "@"};

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

int main(int argc, char const *argv[])
{
    // Declare Local Variables
    std::ifstream srcFile;      // "Source" File
    char c;     
    Token type{DEFAULT};
    std::string temp{""};
    std::string fileName;

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

    // Print Transition Tables to Terminal
    real_output();
    ids_output();
    int_output();

    // Formatting Output File
    dstFile << "TOKEN    \t" << "LEXEME\n";

    // While Loop: Run Through Test File Character By Character (!eof)
    while (!srcFile.eof())
    {
        c = srcFile.get();
        //std::cout << c << ' ';      // TEMP: This line is just for testing purposes, can be removed later
        
        if((c == ' ' || c == ';' || c == '}' || c == ')' || c == ']' || c == ',' || c == '*' || c == '(') && temp != "")
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
        }
        
        // Operator Function
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
        

        // Separator Function
        if (srcFile.peek() == ':' && c == ':')
        {
            logLexeme(SEPARATOR, dstFile, "::");
        }
        if (srcFile.peek() == '/' && c == '/')
        {
            logLexeme(SEPARATOR, dstFile, "//");
        }
        findSeparator(type, dstFile, c);
        
        if (type == DEFAULT)
            findUnknownChar(c, temp, type, dstFile);
        
        if (type == DEFAULT)
            temp += c;          // appends current character to string
                      
        type = DEFAULT;
    }
    // End of While Loop
    
    std::cout << "\n\n";  // TEMP

    // Print Output File Contents
    dstFile.seekg(0);
    while(getline(dstFile, temp))
        std::cout << temp << std::endl;
        
    // Close Test File (src)
    srcFile.close();
    // Close Output File
    dstFile.close();

    return 0;
}

template <typename T> void logLexeme(Token t, std::fstream& dst, T& lexeme)
{
    std::string str{lexeme};
    if (t == OPERATOR)
        dst << "Operator  \t" << lexeme << std::endl;
    else if(t == SEPARATOR)
    {
        if(str == "\n")
            dst << "Separator\t" << "NEWLINE" << std::endl;
        else if(str == " ")
            dst << "Separator\t" << "SPACE" << std::endl;
        else
            dst << "Separator\t" << lexeme << std::endl;
    }
            
    return;
}

void logLexeme(Token t, std::fstream& dst, std::string& s)
{
    switch (t)
    {
    case 2:
        dst << "Integer  \t" << s << std::endl;
        break;
    case 3: 
        dst << "Real     \t" << s << std::endl;
        break;
    case 4:
        dst << "Keyword  \t" << s << std::endl;
        break;
    case 5:
        dst << "ID       \t" << s << std::endl;
        break;
    default:
        dst << "Unknown  \t" << s << std::endl;
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


