#include "json.h"
#include "token.h"
#include <cctype>
#include <iostream>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>

JSONValue::JSONValue() {
    this->number = 0.0;
    this->boolean = nullptr;
    this->str = nullptr;
    this->list = nullptr;
    this->object = nullptr;
}

JSONNode::JSONNode() {
    this->type = JSONType::NULL_TYPE;
    this->value = JSONValue();
}

JSONNode::JSONNode(JSONType type, JSONValue value) {
    this->type = type;
    this->value = value;
}

void JSONNode::update_value(JSONType type, JSONValue value) {
    this->type = type;
    this->value = value;
}

void JSONNode::clear() {
    this->type = JSONType::NULL_TYPE;
    this->value = JSONValue();
}

float JSONNode::get_number() {
    if (this->type != JSONType::NUMBER) {
        throw std::logic_error("Invalid type return");
    }
    return this->value.number;
}

bool JSONNode::get_bool() {

    if (this->type != JSONType::BOOLEAN) {
        throw std::logic_error("Invalid type return");
    }
    return this->value.boolean;
}

std::string JSONNode::get_string() {
    if (this->type != JSONType::STRING) {
        throw std::logic_error("Invalid type return");
    }
    return *this->value.str;
}

std::vector<JSONNode> JSONNode::get_list() {
    if (this->type != JSONType::LIST) {
        throw std::logic_error("Invalid type return");
    }
    return *this->value.list;
}

JSONNode JSONNode::get_object(std::string key) {
    if (this->type != JSONType::OBJECT) {
        throw std::logic_error("Invalid type return");
    }
    return this->value.object->at(key);
}

JSONParser::JSONParser(std::string filePath) {
    this->filePath = filePath;
    this->tokenizer = Tokenizer();
    this->tokenizer.read_file(filePath);
    std::cout << "Start" << std::endl;
    this->tokenizer.load_to_token();
    std::cout << "End" << std::endl;
}

JSONNode JSONParser::parse_object() {
    bool finish = false;
    JSONType type = JSONType::OBJECT;
    std::map<std::string, JSONNode> object;
    bool isKey = true;

    std::string key;
    JSONNode value;

    while (!this->tokenizer.node_is_empty() && !finish) {
        TokenNode tokenNode = this->tokenizer.pop_first_token();
        switch (tokenNode.type) {
            case TokenType::STRING:
                if (isKey) {
                    key = tokenNode.value;
                } else {
                    value = parse_string(tokenNode);
                }
                break;
            case TokenType::COMMA:
                object[key] = value;
                value.clear();
                isKey = true;
                break;
            case TokenType::COLON:
                isKey = false;
                break;
            case TokenType::NUMBER:
                value = parse_number(tokenNode);
                break;
            case TokenType::BOOLEAN:
                value = parse_boolean(tokenNode);
                break;
            case TokenType::CURLY_END:
                finish = true;
                break;
            default:
                break;
        }
    }
    JSONValue nodeObject;
    nodeObject.object = &object;
    return JSONNode(type, nodeObject);
}

JSONNode JSONParser::parse_list() {
    bool finish = false;
    JSONType type = JSONType::LIST;
    std::vector<JSONNode> list;
    JSONNode value;

    while (!this->tokenizer.node_is_empty() && !finish) {
        TokenNode token = this->tokenizer.pop_first_token();
        switch (token.type) {
            case TokenType::COMMA:
                list.push_back(value);
                break;
            case TokenType::ARRAY_END:
                finish = true;
                break;
            default:
                value = parse_value(token);
                break;
        }
    }
    JSONValue nodeList;
    nodeList.list = &list;
    return JSONNode(type, nodeList);
}

JSONNode JSONParser::parse_value(TokenNode &node) {
    switch (node.type) {
        case TokenType::ARRAY_START:
            return parse_list();
        case TokenType::CURLY_START:
            return parse_object();
        case TokenType::STRING:
            return parse_string(node);
        case TokenType::NUMBER:
            return parse_number(node);
        case TokenType::BOOLEAN:
            return parse_boolean(node);
        case TokenType::NULL_TYPE:
            return parse_number(node);
        default:
            throw std::runtime_error("This node is not value type");
    }
}

JSONNode JSONParser::parse_string(TokenNode &node) {
    if (node.type != TokenType::STRING) {
        throw std::runtime_error("Node is not string");
    }
    JSONValue value;
    value.str = &node.value;
    return JSONNode(JSONType::STRING, value);
}

JSONNode JSONParser::parse_number(TokenNode &node) {
    if (node.type != TokenType::NUMBER) {
        throw std::runtime_error("Node is not number");
    }
    float number = std::stof(node.value);
    JSONValue value;
    value.number = number;
    return JSONNode(JSONType::NUMBER, value);
}

JSONNode JSONParser::parse_boolean(TokenNode &node) {
    if (node.type != TokenType::BOOLEAN) {
        throw std::runtime_error("Node is not boolean");
    }
    std::string buffer;
    for (std::string::iterator it = node.value.begin(); it != node.value.end(); it++) {
        buffer.push_back(tolower(*it));
    }
    bool result = false;
    if (buffer == "true") {
        result = true;
    } else if (buffer == "false") {
        result = false;
    } else {
        throw std::runtime_error("Cannot convert node to bool");
    }
    JSONValue value;
    value.boolean = &result;
    return JSONNode(JSONType::NUMBER, value);
}
JSONNode JSONParser::parse_null(TokenNode &node) {
    if (node.type != TokenType::NULL_TYPE) {
        throw std::runtime_error("Node is not null type");
    }
    JSONValue value;
    return JSONNode(JSONType::NULL_TYPE, value);
}

void JSONParser::parse() {
    while (!this->tokenizer.node_is_empty()) {
        TokenNode token = this->tokenizer.pop_first_token();
        parse_value(token);
        std::cout << token.value << std::endl;
    }
}
