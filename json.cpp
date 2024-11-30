#include "json.h"
#include "token.h"
#include <cctype>
#include <iostream>
#include <map>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// JSON Value
Value::Value() {
    this->number = 0.0;
    this->boolean = nullptr;
    this->str = nullptr;
    this->list = nullptr;
    this->object = nullptr;
}

// JSON Node
JNode::JNode() {
    this->type = JType::NULL_TYPE;
    this->value = nullptr;
}

JNode::JNode(JType type, Value* value) {
    this->type = type;
    this->value = value;
}

void JNode::update_value(JType type, Value* value) {
    this->type = type;
    this->value = value;
}

void JNode::clear() {
    this->type = JType::NULL_TYPE;
    this->value = nullptr;
}

float JNode::get_number() {
    if (this->type != JType::NUMBER) {
        throw std::logic_error("Invalid type return");
    }
    return this->value->number;
}

bool JNode::get_bool() {
    if (this->type != JType::BOOLEAN) {
        throw std::logic_error("Invalid type return");
    }
    return this->value->boolean;
}

std::string JNode::get_string() {
    if (this->type != JType::STRING) {
        throw std::logic_error("Invalid type return");
    }
    return *this->value->str;
}

std::vector<JNode> JNode::get_list() {
    if (this->type != JType::LIST) {
        throw std::logic_error("Invalid type return");
    }
    return *this->value->list;
}

std::map<std::string, JNode> JNode::get_object() {
    if (this->type != JType::OBJECT) {
        throw std::logic_error("Invalid type return");
    }
    return *this->value->object;
}

void JNode::print_value() {
    switch (this->type) {
        case JType::NULL_TYPE:
                std::cout << "Null" << std::endl;
            break;
        case JType::OBJECT:
            std::cout << '{' << std::endl;
            for (auto obj : get_object()) {
                std::cout << obj.first << ": ";
                obj.second.print_value();
            }
            std::cout << "}," << std::endl;
            break;
        case JType::LIST:
            std::cout << '[' << std::endl;
            for (JNode val : get_list()) {
                val.print_value();
            }
            std::cout << ']' << std::endl;
            break;
        case JType::STRING:
            std::cout << get_string() << std::endl;
            break;
        case JType::NUMBER:
            std::cout << this->get_number() << std::endl;
            break;
        case JType::BOOLEAN:
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

// JSON Pareser
Parser::Parser() {
    this->tokenizer = Tokenizer();
    this->data = JNode();
}

void Parser::load_file(std::string filePath) {
    this->tokenizer.read_file(filePath);
    this->tokenizer.load_to_token();
}

void Parser::load_data(std::string input) {
    std::stringstream data(input);
    this->tokenizer.read_data(&data);
    this->tokenizer.load_to_token();
}

JNode* Parser::parse_object() {
    bool finish = false;
    JType type = JType::OBJECT;
    std::map<std::string, JNode>* object = new std::map<std::string, JNode>();
    bool isKey = true;

    std::string key;
    JNode* value = new JNode();

    while (!this->tokenizer.node_is_empty() && !finish) {
        Node tokenNode = this->tokenizer.pop_first_token();
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
    Value* nodeObject = new Value();
    nodeObject->object = object;
    return new JNode(type, nodeObject);
}

JNode* Parser::parse_list() {
    bool finish = false;
    JType type = JType::LIST;
    std::vector<JNode>* list = new std::vector<JNode>();
    JNode* value;

    while (!this->tokenizer.node_is_empty() && !finish) {
        Node token = this->tokenizer.pop_first_token();
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
    Value* nodeList = new Value();
    nodeList->list = list;
    return new JNode(type, nodeList);
}

JNode* Parser::parse_value(Node* node) {
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

JNode* Parser::parse_string(Node* node) {
    if (node->type != TokenType::STRING) {
        throw std::runtime_error("Node is not string");
    }
    Value* value = new Value();
    value->str = node->value;
    return new JNode(JType::STRING, value);
}

JNode* Parser::parse_number(Node* node) {
    if (node->type != TokenType::NUMBER) {
        throw std::runtime_error("Node is not number");
    }
    float number = std::stof(*node->value);
    Value* value = new Value();
    value->number = number;
    return new JNode(JType::NUMBER, value);
}

JNode* Parser::parse_boolean(Node* node) {
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
    Value* value = new Value();
    value->boolean = &result;
    return new JNode(JType::NUMBER, value);
}

JNode* Parser::parse_null(Node* node) {
    if (node->type != TokenType::NULL_TYPE) {
        throw std::runtime_error("Node is not null type");
    }
    Value* value = new Value();
    return new JNode(JType::NULL_TYPE, value);
}

void Parser::parse() {
    while (!this->tokenizer.node_is_empty()) {
        Node token = this->tokenizer.pop_first_token();
        this->data = *parse_value(&token);
    }
}

void Parser::print() {
    this->data.print_value();
}
