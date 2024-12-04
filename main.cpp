#include <string>

#include "json.h"

int main() {
    std::string filePath = "data.json";
    Parser parser = Parser();
    parser.load_file(filePath);
    parser.parse();
    parser.print();

    return 0;
}
