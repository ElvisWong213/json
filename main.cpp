#include <string>

#include "json.h"

int main() {
    std::string filePath = "../data.json";
    JSONParser parser = JSONParser(filePath);

    parser.parse();

    return 0;
}
