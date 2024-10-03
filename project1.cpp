// Project 1: Lexical Analyzer
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
// #include <regex>
#include <set>
#include <typeinfo>
#include <algorithm>

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

// std::regex id_pattern("[a-zA-Z][a-zA-Z0-9]*[a-zA-Z]");
// starts with any letter, any letter or digit that can occur zero times or more, ends with any letter

// std::regex int_pattern("[0-9]+");
// any int 0-9 that can occur once or more

// std::regex real_pattern("[0-9]+\\.[0-9]+");
// any int 0-9 that can occur once or more, \\. refers to a dot (need two slashes cuz one backslash represents smthg else), any int 0-9 that can occur once or more

const int ARRAY_SIZE = 11;
char unknownCharArray[ARRAY_SIZE] = {'!', '#', '$', '%', '&', '`', '?', '@', '^', '_', '~'};


std::set<std::string> keywords{"if", "switch", "else","double","fi","new","return","asm"
                            "get","auto","while","extern","put","typedef","break","integer"
                            "break","boolean","operator","real","template","goto","enum",
                            "class", "this","function","case","string","throw","const","float",
                            "try","char","void","for","signed","register","unsigned","friend",
                            "catch","union","public","continue","protected","virtual","private",
                            "default","inline","sizeof","delete","int","static","volatile","do",
                            "long","struct","char16_t","char32_t","explicit","nullptr","static_cast",
                            "overload","short"};

std::set<std::string> operators{"=", "==", ">", "<", "<=",">=","!=","+","-","*","/"};


std::set<std::string> separators{"{","}","(",")",R"(")", ":"," ",".",";","::",R"(\n)","//","[","]","|",","};


template <typename T> void findSeparator(Token&, std::fstream&, std::string&);
template <typename T> void findOperator(Token&, std::fstream&, std::string&);
void findKeyword(Token&, std::fstream&, std::string&);
void IDs(std::string&, Token&, std::fstream&);
void integers(std::string&, Token&, std::fstream&);
void reals(std::string&, Token&, std::fstream&);
void findUnknownChar(char&, std::string&, Token&, std::fstream&);
void logLexeme(Token, std::fstream&, std::string&, char);            // Logging Operators, Separators, and Unknown
void logLexeme(Token, std::fstream&, std::string&);                  // Logging Integer, Real, Keyword, ID

int main(int argc, char const *argv[])
{
    // Declare Local Variables
    std::ifstream srcFile;      // "Source" File
    char c;     
    Token type{DEFAULT};
    std::string temp{""};
    std::string fileName;
    // std::smatch match;          // holds results of regex matches (not sure if it even functions in code :P)

    std::cout << "Enter Test File Name: ";
    std::cin >> fileName;
        
    // Open Test File (src)
    srcFile.open(fileName);        
    if (!srcFile)
        throw std::invalid_argument("ERROR: FILE CANNOT BE FOUND, ENSURE YOU INPUT THE CORRECT FILE NAME");

    std::cout << "Enter Output File Name: ";
    std:: cin >> fileName;

    // Declare "Destination" File
    std::fstream dstFile(fileName, std::ios::in | std::ios::out);     
    if (!dstFile)
        throw std::invalid_argument("ERROR: FILE CANNOT BE FOUND, ENSURE YOU INPUT THE CORRECT FILE NAME");


    // TEMP: Check file size for testing purposes
    srcFile.seekg(0, std::ios::end);
    int fileSize{srcFile.tellg()};
    srcFile.seekg(0);
    std::cout << "File Size: " << fileSize << std::endl;
    // END OF TEMP

    // Formatting Output File
    dstFile << "TOKEN    \t" << "LEXEME\n";
    
    // Setting up Output to Terminal
    std::cout << "TOKEN    \t" << "LEXEME\n";

    // While Loop: Run Through Test File Character By Character (!eof)
    while (!srcFile.eof())
    {
        // Reading character by character 
        srcFile.get(c);
        std::cout << c << ' ';      // TEMP: This line is just for testing purposes, can be removed later

        // Operator Function
        findOperator(type, dstFile, temp, c);
        
        // Separator Function
        findSeparator(type, dstFile, temp, c);
        
        // Unknown Char Function
        findUnknownChar(c, temp, type, dstFile);
       
        if(c == ' ' || c == ')' || c == ']' || c == '}')    // If a delimiter is hit, it indicates end of possible string
        {               
            // Integer Function
            integers(temp, type, dstFile);

            // Real Function
            reals(temp, type, dstFile);

            // Keyword Function
            findKeyword(type, dstFile, temp);

            // ID Function
            IDs(temp, type, dstFile);

            // Unknown Function for string
            if(type == DEFAULT)
            {
                type = UNKNOWN;
                logLexeme(type, dstFile, temp);
            }
        }

        // Unknown Function for chars      
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

void logLexeme(Token t, std::fstream& dst, std::string& s, char c)
{
    if (t == OPERATOR)
        dst << "Operator  \t" << c << std::endl;
    else if (t == SEPARATOR)
    {
        if (c == '\n')
            dst << "Separator\t" << "newline" << std::endl;
        else if(c == ' ') 
            dst << "Separator\t" << "space" << std::endl;
        else
            dst << "Separator\t" << c << std::endl;

    } else
        dst << "Unknown  \t" << c << std::endl;
    
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

void IDs(std::string input, Token& type, std::fstream&) {

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

    if (isalpha(input.size()-1)) {
        type = ID;
    }
}

void integers(std::string input, Token& type, std::fstream&) {

    int i = 0;

    if (input.size() == 0) {
        return; // not valid string size of 0
    }

    for (i = 0; i < input.size() - 1; i++) {
        if(!isdigit(input[i])) {
            return; // if there is not an integer, invalid integer
        }
    }

    type = INTEGER;
}

void reals(std::string input, Token& type, std::fstream&) {

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
}

void findUnknownChar(char& c, std::string& s, Token& t, std::fstream& dst)
{
    for (size_t i = 0; i < ARRAY_SIZE; i++)
    {
        if (unknownCharArray[i] == c)
        {
            t = UNKNOWN;
            logLexeme(t, dst, s, c);
        }
    }
    return;
}

void findKeyword(Token& t, std::fstream& file, std::string& word)
{
    if(keywords.find(word) != keywords.end())
        t = KEYWORD;
        logLexeme(t, file, word);
    return; 
}


template <typename T> void findOperator(Token& t, std::fstream& file, T word)
{
    if(typeid(word) == typeid(char))
    {
        std::string chword(1, word);
        word = chword;
    }
    if(operators.find(word) != operators.end())
        t = OPERATOR;
        logLexeme(t, file, word);
    return; 
}

template <typename T> void findSeparator(Token& t, std::fstream& file, T word)
{
    if(typeid(word) == typeid(char))
    {
        std::string chword(1, word);
        word = chword;
    }
    if(operators.find(word) != operators.end())
        t = SEPARATOR;
        logLexeme(t, file, word);
    return; 
}

// void findOperator(Token& t, std::fstream& file, std::string& word)
// {
//     if(operators.find(word) != operators.end())
//         t = OPERATOR;
//         logLexeme(t, file, word);
//     return; 
// }
// void findOperator(Token& t, std::fstream& file, std::string& word, char& ch)
// {
//     std::string chword(1, ch);
//     if(operators.find(chword) != operators.end())
//         t = OPERATOR;
//         logLexeme(t, file, word, ch);
//     return; 
// }

// void findSeparator(Token& t, std::fstream& file, std::string& word)
// {
//     if(separators.find(word) != separators.end())
//         t = SEPARATOR;
//         logLexeme(t, file, word);
//     return; 
// }

// void findSeparator(Token& t, std::fstream& file, std::string& word, char& ch)
// {
//     std::string chword(1, ch);
//     if(separators.find(chword) != separators.end())
//         t = SEPARATOR;
//     logLexeme(t, file, word, ch);
//     return; 
// }




