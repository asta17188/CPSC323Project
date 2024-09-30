// Project 1: Lexical Analyzer

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

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

void logLexeme(Token, std::fstream&, std::string&, char);            // Logging Operators, Separators, and Unknown
void logLexeme(Token, std::fstream&, std::string&);                 // Logging Integer, Real, Keyword, ID

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
        // TEMP: Testing reading character by character 
        c = srcFile.get();
        std::cout << c << ' ';      // TEMP: This line is just for testing purposes, can be removed later

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
        if(type != DEFAULT)
        {
            if(type ==  OPERATOR || type == SEPARATOR || (type == UNKNOWN && temp.size() == 0))
                logLexeme(type, dstFile, temp, c);
            else    
                logLexeme(type, dstFile, temp);
        }
                
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

       // Resets temp string 
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
       // Resets temp string 
        s = "";
    
    return;
}
