#include <iostream>
#include <bitset>
#include <fstream>
#include <vector>
#include <string>

#define NUM_HASH_FUNCS 4
#define BF_SIZE 57707801
std::bitset<BF_SIZE> bloomFilter;

std::vector<int> hashFunc(std::string hashValue) {
    std::vector<int> result;
    result.reserve(NUM_HASH_FUNCS);
    for (int i = 0; i < NUM_HASH_FUNCS; ++i) {
        hashValue += std::to_string(i);
        std::hash<std::string> hasher;
        auto hashed = hasher(hashValue);
        result[i] = (int) (hashed % BF_SIZE);
    }
    return result;
}

int main() {

    std::fstream inputFile;
    std::fstream checkFile;

    std::vector<std::string> inputHashValueVector;
    std::vector<std::string> checkHashValueVector;
    std::string hashValueString;
    inputFile.open("input.txt", std::ios::in);
    while (!inputFile.eof()) {
        std::getline(inputFile, hashValueString);
        if (hashValueString.empty()) break;
        hashValueString.pop_back();
        inputHashValueVector.push_back(hashValueString);
    }
    if (inputHashValueVector.empty()) {
        std::cout << "The input file is empty!" << std::endl;
        return 0;
    }
    inputFile.close();

    checkFile.open("check.txt", std::ios::in);
    while (!checkFile.eof()) {
        std::getline(checkFile, hashValueString);
        if (hashValueString.empty()) break;
        checkHashValueVector.push_back(hashValueString);
    }
    if (checkHashValueVector.empty()) {
        std::cout << "The check file is empty!" << std::endl;
        return 0;
    }
    checkFile.close();

    for (const std::string& hashValue : inputHashValueVector) {
        std::vector<int> hashIndex = hashFunc(hashValue);
        for (int i = 0; i < NUM_HASH_FUNCS; ++i) {
//            std::cout << hashValue << " hashed at " << hashIndex[i] << std::endl;
            bloomFilter[hashIndex[i]] = true;
        }
    }

    for (std::string hashValue : checkHashValueVector) {
        std::vector<int> hashIndex = hashFunc(hashValue);
        for (int i = 0; i < NUM_HASH_FUNCS; ++i) {
            if (bloomFilter[hashIndex[i]]) {
                std::cout << "Exist " << hashValue << std::endl;
                break;
            }
        }
    }

    return 0;
}
