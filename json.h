#pragma once

#include "token.h"
#include <map>
#include <string>
#include <vector>

enum class JSONType {
    NULL_TYPE = 0,
    OBJECT,
    LIST,
    STRING,
    NUMBER,
    BOOLEAN,
};

class JSONNode;

class JSONValue {
    public:
        float number;
        bool* boolean;
        std::string* str;
        std::vector<JSONNode>* list;
        std::map<std::string, JSONNode>* object;
        JSONValue();
};

class JSONNode {
    private:
        JSONType type;
        JSONValue* value;
    public:
        JSONNode();
        JSONNode(JSONType type, JSONValue* value);
        void clear();
        void update_value(JSONType type, JSONValue* value);
        float get_number();
        bool get_bool();
        std::string get_string();
        std::vector<JSONNode> get_list();
        std::map<std::string, JSONNode> get_object();
        void print_value();
};

class JSONParser {
    private:
        std::string filePath;
        Tokenizer tokenizer;
        JSONNode data;

    public:
        JSONParser(std::string filePath);
        JSONNode* parse_object();
        JSONNode* parse_list();
        JSONNode* parse_value(TokenNode* node);
        JSONNode* parse_string(TokenNode* node);
        JSONNode* parse_number(TokenNode* node);
        JSONNode* parse_boolean(TokenNode* node);
        JSONNode* parse_null(TokenNode* node);
        void parse();
};

