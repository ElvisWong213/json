#pragma once

#include <string>

enum class TokenType {
    ARRAY_START,
    ARRAY_END,
    CURLY_START,
    CURLY_END,
    STRING,
    NUMBER,
    BOOLEAN,
    COLON,
    COMMA,
    NULL_TYPE
};

class TokenNode {
    public:
        std::string value;
        TokenType type;
        TokenNode* next;
        TokenNode();
};

class Tokenizer {
    private:
        TokenNode* head;
        std::string data;
        bool is_space(char c);
    public:
        Tokenizer();
        void read_file(std::string filePath);
        void push_token(TokenNode* token);
        TokenNode pop_first_token();
        bool node_is_empty();
        void load_to_token();
};
