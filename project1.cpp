// Project 1: Lexical Analyzer
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
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

/*
    NOTE: When testing program, make sure to delete the output.txt file
          and create a new one. Otherwise, old (and possibly failed) attempts
          will still be on the output.txt when running the program again
          and create erroneous results.
*/
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

void findOperator(Token& , std::fstream& , std::string& );
void findOperator(Token& , std::fstream& , char& );
void findSeparator(Token& , std::fstream& , std::string& );
void findSeparator(Token& , std::fstream& , char& );
void findKeyword(Token&, std::fstream&, std::string&);
void IDs(std::string&, Token&, std::fstream&);
void integers(std::string&, Token&, std::fstream&);
void reals(std::string&, Token&, std::fstream&);
void findUnknownChar(char&, std::string&, Token&, std::fstream&); 
template <typename T> void logLexeme(Token, std::fstream&, T&);          
void logLexeme(Token, std::fstream&, std::string&);                 

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
        c = srcFile.get();
        std::cout << c << ' ';      // TEMP: This line is just for testing purposes, can be removed later

        // Operator Function
        findOperator(type, dstFile, c);

        // Separator Function
        findSeparator(type, dstFile, c);
        
        if (srcFile.peek() == '=' && c == '=')
        {
            logLexeme(OPERATOR, dstFile, "==");
        }

        if (type != DEFAULT)
            findUnknownChar(c, temp, type, dstFile);
        
        if((c == ' ' || c == ';' || c == '}' || c == ')' || c == ']' || c == ',' || c == '*') && temp != "")
        {
            // Integer Function
            integers(temp, type, dstFile);

            // Real Function
            reals(temp, type, dstFile);

            // Keyword Function
            findKeyword(type, dstFile, temp);
            
            // ID Function
            IDs(temp, type, dstFile);
            // Unknown Function
            if(type == DEFAULT)
            {
                type = UNKNOWN;
                logLexeme(type, dstFile, temp);
            }
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

void integers(std::string& input, Token& type, std::fstream& dst) {

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
    logLexeme(type, dst, input);

    return;
    // integer division ignores any remainders
    // sequence of decimal digits
    // if (std::regex_search(input, /*match,*/ int_pattern)) {
        // type = INTEGER;
        // std::cout << "integer" << "         " << match.str() << "\n";
    // }
}

void reals(std::string& input, Token& type, std::fstream& dst) {

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
        logLexeme(type, dst, input);
    }

    return;
    // integer followed by "."
    // if (std::regex_search(input, /*match,*/ real_pattern)) {
        // type = REAL;
        // std::cout << "real" << "         " << match.str() << "\n";
    // }
}

void IDs(std::string& input, Token& type, std::fstream& dst) {

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
            logLexeme(type, dst, input);
    }

    
    return;
    // sequence of letters/digits, first & last characters must be letters
    // upper/lowercase letters are different
    // if (std::regex_search(input, /*match,*/ id_pattern)) {
        // type = ID;
        // std::cout << "identifier" << "         " << match.str() << "\n";
    // }
}

void findKeyword(Token& t, std::fstream& file, std::string& word)
{
    if(keywords.find(word) != keywords.end())
        t = KEYWORD;
        logLexeme(t, file, word);
    return; 
}






