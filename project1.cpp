// Project 1: Lexical Analyzer
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <regex>
#include <set>
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

std::regex id_pattern("[a-zA-Z][a-zA-Z0-9]*[a-zA-Z]");
// starts with any letter, any letter or digit that can occur zero times or more, ends with any letter

std::regex int_pattern("[0-9]+");
// any int 0-9 that can occur once or more

std::regex real_pattern("[0-9]+\\.[0-9]+");
// any int 0-9 that can occur once or more, \\. refers to a dot (need two slashes cuz one backslash represents smthg else), any int 0-9 that can occur once or more

const std::set<char> unknownChar{'!', '#', '$', '%', '&', '`', '?', '@', '^', '_', '~'};

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

void findSeparator(Token&, std::fstream&, std::string&, char&);
void findSeparator(Token&, std::fstream&, std::string&);
void findOperator(Token&, std::fstream&, std::string&, char&);
void findOperator(Token&, std::fstream&, std::string&);
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
    std::smatch match;          // holds results of regex matches (not sure if it even functions in code :P)

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

        s = "";     // Resets temp string
    
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

void IDs(std::string& input, Token& type, std::fstream& dst) { 
    // sequence of letters/digits, first & last characters must be letters
    // upper/lowercase letters are different
    if (std::regex_search(input, /*match,*/ id_pattern)) {
        type = ID;
        logLexeme(type, dst, input);
        // std::cout << "identifier" << "         " << match.str() << "\n";
    }
    return;
}

void integers(std::string& input, Token& type, std::fstream& dst) {
    // integer division ignores any remainders
    // sequence of decimal digits
    if (std::regex_search(input, /*match,*/ int_pattern)) {
        type = INTEGER;
        logLexeme(type, dst, input);
        // std::cout << "integer" << "         " << match.str() << "\n";
    }
    return;
}

void reals(std::string& input, Token& type, std::fstream& dst) {
    // integer followed by "."
    if (std::regex_search(input, /*match,*/ real_pattern)) {
        type = REAL;
        logLexeme(type, dst, input);
        // std::cout << "real" << "         " << match.str() << "\n";
    }
    return;
}

void findUnknownChar(char& c, std::string& s, Token& t, std::fstream& dst)
{
    if(unknownChar.find(c) != unknownChar.end())
    {
        t = UNKNOWN;
        logLexeme(t, dst, s, c);
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

void findOperator(Token& t, std::fstream& file, std::string& word)
{
    if(operators.find(word) != operators.end())
        t = OPERATOR;
        logLexeme(t, file, word);
    return; 
}
void findOperator(Token& t, std::fstream& file, std::string& word, char& ch)
{
    std::string chword(1, ch);
    if(operators.find(chword) != operators.end())
        t = OPERATOR;
        logLexeme(t, file, word, ch);
    return; 
}

void findSeparator(Token& t, std::fstream& file, std::string& word)
{
    if(separators.find(word) != separators.end())
        t = SEPARATOR;
        logLexeme(t, file, word);
    return; 
}

void findSeparator(Token& t, std::fstream& file, std::string& word, char& ch)
{
    std::string chword(1, ch);
    if(separators.find(chword) != separators.end())
        t = SEPARATOR;
    logLexeme(t, file, word, ch);
    return; 
}


