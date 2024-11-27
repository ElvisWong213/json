#include "json.h"
#include "token.h"
#include <cctype>
#include <iostream>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

JSONValue::JSONValue() {
    this->number = 0.0;
    this->boolean = nullptr;
    this->str = nullptr;
    this->list = nullptr;
    this->object = nullptr;
}

JSONNode::JSONNode() {
    this->type = JSONType::NULL_TYPE;
    this->value = nullptr;
}

JSONNode::JSONNode(JSONType type, JSONValue* value) {
    this->type = type;
    this->value = value;
}

void JSONNode::update_value(JSONType type, JSONValue* value) {
    this->type = type;
    this->value = value;
}

void JSONNode::clear() {
    this->type = JSONType::NULL_TYPE;
    this->value = nullptr;
}

float JSONNode::get_number() {
    if (this->type != JSONType::NUMBER) {
        throw std::logic_error("Invalid type return");
    }
    return this->value->number;
}

bool JSONNode::get_bool() {
    if (this->type != JSONType::BOOLEAN) {
        throw std::logic_error("Invalid type return");
    }
    return this->value->boolean;
}

std::string JSONNode::get_string() {
    if (this->type != JSONType::STRING) {
        throw std::logic_error("Invalid type return");
    }
    return *this->value->str;
}

std::vector<JSONNode> JSONNode::get_list() {
    if (this->type != JSONType::LIST) {
        throw std::logic_error("Invalid type return");
    }
    return *this->value->list;
}

std::map<std::string, JSONNode> JSONNode::get_object() {
    if (this->type != JSONType::OBJECT) {
        throw std::logic_error("Invalid type return");
    }
    return *this->value->object;
}

void JSONNode::print_value() {
    switch (this->type) {
        case JSONType::NULL_TYPE:
                std::cout << "Null" << std::endl;
            break;
        case JSONType::OBJECT:
            std::cout << '{' << std::endl;
            for (auto obj : get_object()) {
                std::cout << obj.first << ": ";
                obj.second.print_value();
            }
            std::cout << "}," << std::endl;
            break;
        case JSONType::LIST:
            std::cout << '[' << std::endl;
            for (JSONNode val : get_list()) {
                val.print_value();
            }
            std::cout << ']' << std::endl;
            break;
        case JSONType::STRING:
            std::cout << get_string() << std::endl;
            break;
        case JSONType::NUMBER:
            std::cout << this->get_number() << std::endl;
            break;
        case JSONType::BOOLEAN:
            if (this->get_bool()) {
                std::cout << "True" << std::endl;
            } else {
                std::cout << "False" << std::endl;
            }
            break;
        default:
            break;
    }
}

JSONParser::JSONParser(std::string filePath) {
    this->filePath = filePath;
    this->tokenizer = Tokenizer();
    this->tokenizer.read_file(filePath);
    this->tokenizer.load_to_token();
}

JSONNode* JSONParser::parse_object() {
    bool finish = false;
    JSONType type = JSONType::OBJECT;
    std::map<std::string, JSONNode>* object = new std::map<std::string, JSONNode>();
    bool isKey = true;

    std::string key;
    JSONNode* value = new JSONNode();

    while (!this->tokenizer.node_is_empty() && !finish) {
        TokenNode tokenNode = this->tokenizer.pop_first_token();
        switch (tokenNode.type) {
            case TokenType::STRING:
                if (isKey) {
                    key = *tokenNode.value;
                } else {
                    value = parse_string(&tokenNode);
                }
                break;
            case TokenType::COMMA:
                object->insert({key, *value});
                isKey = true;
                break;
            case TokenType::COLON:
                isKey = false;
                break;
            case TokenType::NUMBER:
                value = parse_number(&tokenNode);
                break;
            case TokenType::BOOLEAN:
                value = parse_boolean(&tokenNode);
                break;
            case TokenType::ARRAY_START:
                value = parse_list();
                break;
            case TokenType::CURLY_END:
                object->insert({key, *value});
                finish = true;
                break;
            default:
                break;
        }
    }
    JSONValue* nodeObject = new JSONValue();
    nodeObject->object = object;
    return new JSONNode(type, nodeObject);
}

JSONNode* JSONParser::parse_list() {
    bool finish = false;
    JSONType type = JSONType::LIST;
    std::vector<JSONNode>* list = new std::vector<JSONNode>();
    JSONNode* value;

    while (!this->tokenizer.node_is_empty() && !finish) {
        TokenNode token = this->tokenizer.pop_first_token();
        switch (token.type) {
            case TokenType::COMMA:
                list->push_back(*value);
                value->clear();
                break;
            case TokenType::ARRAY_END:
                list->push_back(*value);
                value->clear();
                finish = true;
                break;
            default:
                value = parse_value(&token);
                break;
        }
    }
    JSONValue* nodeList = new JSONValue();
    nodeList->list = list;
    return new JSONNode(type, nodeList);
}

JSONNode* JSONParser::parse_value(TokenNode* node) {
    switch (node->type) {
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
            return parse_null(node);
        default:
            throw std::runtime_error("This node is not value type");
    }
}

JSONNode* JSONParser::parse_string(TokenNode* node) {
    if (node->type != TokenType::STRING) {
        throw std::runtime_error("Node is not string");
    }
    JSONValue* value = new JSONValue();
    value->str = node->value;
    return new JSONNode(JSONType::STRING, value);
}

JSONNode* JSONParser::parse_number(TokenNode* node) {
    if (node->type != TokenType::NUMBER) {
        throw std::runtime_error("Node is not number");
    }
    float number = std::stof(*node->value);
    JSONValue* value = new JSONValue();
    value->number = number;
    return new JSONNode(JSONType::NUMBER, value);
}

JSONNode* JSONParser::parse_boolean(TokenNode* node) {
    if (node->type != TokenType::BOOLEAN) {
        throw std::runtime_error("Node is not boolean");
    }
    std::string buffer;
    for (std::string::iterator it = node->value->begin(); it != node->value->end(); it++) {
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
    JSONValue* value = new JSONValue();
    value->boolean = &result;
    return new JSONNode(JSONType::NUMBER, value);
}

JSONNode* JSONParser::parse_null(TokenNode* node) {
    if (node->type != TokenType::NULL_TYPE) {
        throw std::runtime_error("Node is not null type");
    }
    JSONValue* value = new JSONValue();
    return new JSONNode(JSONType::NULL_TYPE, value);
}

void JSONParser::parse() {
    while (!this->tokenizer.node_is_empty()) {
        TokenNode token = this->tokenizer.pop_first_token();
        this->data = *parse_value(&token);
    }
    this->data.print_value();
}
