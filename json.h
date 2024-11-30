#pragma once

#include "token.h"
#include <map>
#include <string>
#include <vector>

enum class JType {
    NULL_TYPE = 0,
    OBJECT,
    LIST,
    STRING,
    NUMBER,
    BOOLEAN,
};

class JNode;

union Value {
    public:
        float number;
        bool* boolean;
        std::string* str;
        std::vector<JNode>* list;
        std::map<std::string, JNode>* object;
        Value();
};

class JNode {
    private:
        JType type;
        Value* value;
    public:
        JNode();
        JNode(JType type, Value* value);
        void clear();
        void update_value(JType type, Value* value);
        float get_number();
        bool get_bool();
        std::string get_string();
        std::vector<JNode> get_list();
        std::map<std::string, JNode> get_object();
        void print_value();
};

class Parser {
    private:
        Tokenizer tokenizer;
        JNode data;

    public:
        Parser();
        void load_file(std::string filePath);
        void load_data(std::string input);
        JNode* parse_object();
        JNode* parse_list();
        JNode* parse_value(Node* node);
        JNode* parse_string(Node* node);
        JNode* parse_number(Node* node);
        JNode* parse_boolean(Node* node);
        JNode* parse_null(Node* node);
        void parse();
        void print();
};

