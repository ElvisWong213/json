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

class Node {
    public:
        std::string* value;
        TokenType type;
        Node* next;
        Node();
        std::string to_string();
        void print();
};

class Tokenizer {
    private:
        Node* head;
        std::string data;
        bool is_space(char c);

    public:
        Tokenizer();
        void read_file(std::string filePath);
        void read_data(std::istream* data);
        void push_token(Node* token);
        Node* pop_first_token();
        bool node_is_empty();
        void load_to_token();
};
