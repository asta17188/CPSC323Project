#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include "project1.cpp"
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

void findKeyword(std::string word)
{
    if(keywords.find(word) != keywords.end())
        type = KEYWORD;
    return; 
}

void findOperator(std::string word)
{
    if(operators.find(word) != operators.end())
        type = OPERATOR;
    return; 
}
void findOperator(char ch)
{
    std::string word(1, ch);
    if(operators.find(word) != operators.end())
        type = OPERATOR;
    return; 
}

void findSeparator(std::string word)
{
    if(separators.find(word) != separators.end())
        type = SEPARATOR;
    return; 
}
void findSeparator(char ch)
{
    std::string word(1, ch);
    if(separators.find(word) != separators.end())
        type = SEPARATOR;
    return; 
}