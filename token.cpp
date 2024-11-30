#include "token.h"
#include <cstdio>
#include <fstream>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>

Node::Node() {
    this->type = TokenType::NULL_TYPE;
    this->value = nullptr;
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

    read_data(&myFile);

    myFile.close();
}

void Tokenizer::read_data(std::istream* data)  {
    std::string line;
    while (std::getline(*data, line)) {
        this->data.append(line);
    }
}

void Tokenizer::push_token(Node* token) {
    if (this->head == nullptr) {
        this->head = token;
        return;
    }
    Node* current = this->head;
    while (current->next != nullptr) {
        current = current->next;
    }
    current->next = token;
}

Node Tokenizer::pop_first_token() {
    Node* first = this->head;
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

bool Tokenizer::is_space(char c) {
    if (c == '\n' || c == ' ' || c == '\t' || c == '\r') {
        return true;
    }
    return false;
}

void Tokenizer::load_to_token() {
    bool isString = false;
    bool isNumber = false;
    bool isBool = false;
    std::string* buffer = new std::string();
    Node* newToken = new Node();

    for (std::string::iterator it = this->data.begin(); it != this->data.end(); it++) {
        switch (*it) {
            case '{':
                newToken->type = TokenType::CURLY_START;
                push_token(newToken);
                newToken = new Node();
                break;
            case '[':
                newToken->type = TokenType::ARRAY_START;
                push_token(newToken);
                newToken = new Node();
                break;
            case '}':
                newToken->type = TokenType::CURLY_END;
                push_token(newToken);
                newToken = new Node();
                break;
            case ']':
                newToken->type = TokenType::ARRAY_END;
                push_token(newToken);
                newToken = new Node();
                break;
            case '\'':
            case'\"':
                if (isString) {
                    newToken->type = TokenType::STRING;
                    newToken->value = buffer;
                    push_token(newToken);
                    newToken = new Node();
                    buffer = new std::string();
                    isString = false;
                } else {
                    isString = true;
                }
                break;
            case ':':
                if (isString) {
                    buffer->push_back(*it);
                    continue;
                }
                newToken->type = TokenType::COLON;
                push_token(newToken);
                newToken = new Node();
                break;
            case ',':
                if (isNumber && isBool) {
                    throw std::runtime_error("JSON file syntax error: value cannot be number and boolean at the sametime");
                }
                if (!buffer->empty()) {
                    if (isString) {
                        buffer->push_back(*it);
                        continue;
                    }
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
                    newToken = new Node();
                    buffer = new std::string();
                }
                newToken->type = TokenType::COMMA;
                push_token(newToken);
                newToken = new Node();
                break;
            default:
                if (!isString && is_space(*it)) {
                    continue;
                }
                buffer->push_back(*it);
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

    if (isNumber || isString || isBool || !buffer->empty()) {
        throw std::runtime_error("JSON file syntax error");
    }
}
