// Project 1: Lexical Analyzer
#include <iostream>
#include <fstream>
#include <string>

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

// NOTE: Replace .txt with file path
const std::string test1 = "testcase1.txt";
const std::string test2 = "testcase2.txt";
const std::string test3 = "testcase3.txt";
const std::string outputFile = "output.txt";

void logLexeme(Token, std::fstream&, std::string&, char);            // Logging Operators, Separators, and Unknown
void logLexeme(Token, std::fstream&, std::string&);                 // Logging Integer, Real, Keyword, ID

int main(int argc, char const *argv[])
{
    // Declare Local Variables
    std::ifstream srcFile;                                          // "Source" File
    std::fstream dstFile(outputFile, std::ios::in | std::ios::out); // "Destination" File
    char c;     
    Token type{DEFAULT};
    std::string temp{""};
        
    // Open Test File (src)
    srcFile.open(test1);        // test1 can be changed with any other test file

    // Formatting Output File
    dstFile << "TOKEN    \t" << "LEXEME\n";
    
    // Setting up Output to Terminal
    std::cout << "TOKEN    \t" << "LEXEME\n";

    // While Loop: Run Through Test File Character By Character (!eof)
    while (!srcFile.eof())
    {
        // TEMP: Testing reading character by character 
        srcFile >> c;
        std::cout << c << ' ';      // This line is just for testing purposes, can be removed later

        // Operator Function
        // Separator Function
        
        if(c == ' ')
        {
            // Integer Function
            // Real Function
            // Keyword Function
            // ID Function
            // Unknown Function
            if(type == DEFAULT)
                type = UNKNOWN;
        }
        
        // Log Lexeme into Output File
        if(type ==  OPERATOR || type == SEPARATOR || (type == UNKNOWN && temp.size() == 0))
            logLexeme(type, dstFile, temp, c);
        else    
            logLexeme(type, dstFile, temp);
                
        if (type == DEFAULT)
            temp += c;          // appends current character to string
                      
        type = DEFAULT;
    }
    // End of While Loop
    
    // Print Output File Contents
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
        dst << "Separator\t" << c << std::endl;
    } else
        dst << "Unknown  \t" << c << std::endl;

    if (t != DEFAULT)   // Resets temp string 
        s = "";
    
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
    if (t != DEFAULT)   // Resets temp string 
        s = "";
    
    return;
}
