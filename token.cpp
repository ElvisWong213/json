#include "token.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

TokenNode::TokenNode() {
    this->type = TokenType::NULL_TYPE;
    this->value = "";
    this->next = nullptr;
}

Tokenizer::Tokenizer() {
    this->head = nullptr;
    this->data = "";
}

void Tokenizer::read_file(std::string filePath) {
    std::ifstream myFile;

    myFile.open(filePath);
    if (!myFile.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

    std::string line;
    while (std::getline(myFile, line)) {
        this->data.append(line);
    }

    myFile.close();
}

void Tokenizer::push_token(TokenNode* token) {
    if (this->head == nullptr) {
        this->head = token;
        return;
    }
    TokenNode* current = this->head;
    while (current->next != nullptr) {
        current = current->next;
    }
    current->next = token;
}

TokenNode Tokenizer::pop_first_token() {
    TokenNode* first = this->head;
    if (first == nullptr) {
        throw std::runtime_error("pointer is null");
    }
    this->head = this->head->next;
    return *first;
};

bool Tokenizer::node_is_empty() {
    if (this->head == nullptr) {
        return true;
    }
    return false;
};

void Tokenizer::load_to_token() {
    bool isString = false;
    bool isNumber = false;
    bool isBool = false;
    std::string buffer;
    TokenNode* newToken = new TokenNode();

    for (std::string::iterator it = this->data.begin(); it != this->data.end(); it++) {
        std::cout << buffer << std::endl;
        switch (*it) {
            case '{':
                newToken->type = TokenType::CURLY_START;
                newToken->value = '{';
                push_token(newToken);
                newToken = new TokenNode();
                break;
            case '[':
                newToken->type = TokenType::ARRAY_START;
                newToken->value = '[';
                push_token(newToken);
                newToken = new TokenNode();
                break;
            case '}':
                newToken->type = TokenType::CURLY_END;
                newToken->value = '}';
                push_token(newToken);
                newToken = new TokenNode();
                break;
            case ']':
                newToken->type = TokenType::ARRAY_END;
                newToken->value = ']';
                push_token(newToken);
                newToken = new TokenNode();
                break;
            case '\'':
            case'\"':
                if (isString) {
                    newToken->type = TokenType::STRING;
                    newToken->value = buffer;
                    push_token(newToken);
                    newToken = new TokenNode();
                    buffer.clear();
                    isString = false;
                } else {
                    isString = true;
                }
                break;
            case ':':
                newToken->type = TokenType::COLON;
                newToken->value = ':';
                push_token(newToken);
                newToken = new TokenNode();
                break;
            case ',':
                if (isNumber && isBool) {
                    std::cout << buffer << std::endl;
                    throw std::runtime_error("JSON file syntax error: value cannot be number and boolean at the sametime");
                }
                if (!buffer.empty()) {
                    if (isNumber) {
                        isNumber = false;
                        newToken->type = TokenType::NUMBER;
                    } else if (isBool) {
                        isBool = false;
                        newToken->type = TokenType::BOOLEAN;
                    } else {
                        newToken->type = TokenType::NULL_TYPE;
                    }
                    newToken->value = buffer;
                    push_token(newToken);
                    newToken = new TokenNode();
                    buffer.clear();
                }
                newToken->type = TokenType::COMMA;
                newToken->value = ',';
                push_token(newToken);
                newToken = new TokenNode();
                break;
            default:
                buffer.push_back(*it);
                if (isString) {
                    continue;
                }
                if (*it == '-' || *it == '.' || *it >= '0' && *it <= '9') {
                    isNumber = true;
                    continue;
                }
                if (*it == 'T' || *it == 't' || *it == 'F' || *it == 'f' || isBool) {
                    isBool = true;
                    continue;
                }
                break;
        }
    }

    if (!isNumber || !isString || !isBool || !buffer.empty()) {
        throw std::runtime_error("JSON file syntax error");
    }
}