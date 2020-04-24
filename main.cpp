#include <iostream>
#include <bitset>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

#define NUM_HASH_FUNCS 4
#define BF_SIZE 57707801
std::bitset<BF_SIZE> bloomFilter;
bool startState = true;

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

void showUsage() {
    std::cin.clear();
    std::cout << std::endl;
    std::cout << "|*********************************|" << std::endl;
    std::cout << "|****** Bloom Filter Usage *******|" << std::endl;
    std::cout << "|*********************************|" << std::endl;
    std::cout << "| [0] Show Bloom filter info      |" << std::endl;
    std::cout << "| [1] Load new input file         |" << std::endl;
    std::cout << "| [2] Add new value               |" << std::endl;
    std::cout << "| [3] Load check file             |" << std::endl;
    std::cout << "| [4] Check new value             |" << std::endl;
    std::cout << "| [5] Restart Bloom filter        |" << std::endl;
    std::cout << "| [6] Stop program                |" << std::endl;
    std::cout << "|*********************************|" << std::endl;
    std::cout << std::endl;
    startState = false;
}

void showBFInfo() {
    std::cin.clear();
    std::cout << std::endl;
    std::cout << "Bloom filter information:" << std::endl;
    std::cout << "Number of hash functions: " << NUM_HASH_FUNCS << std::endl;
    std::cout << "Size of the Bloom filter: " << BF_SIZE << std::endl;
    std::cout << "Maximum storage capacity value: " << (int) (BF_SIZE / (NUM_HASH_FUNCS /  log(2))) << std::endl;
    std::cout << "False positive probability: " << (int)(pow(2, -NUM_HASH_FUNCS) * 100) << "%" << std::endl;
    std::cout << std::endl;
    startState = false;
}

void loadInputFile() {
    std::cout << "Type the file name or path of input file: ";
    std::string inputFilePath;
    std::ifstream inputFile;
    std::vector<std::string> inputHashValueVector;
    std::string hashValueString;
    std::cin >> inputFilePath;
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    inputFile.open(inputFilePath);
    if (inputFile.fail()) {
        std::cout << "The input file is not exist!" << std::endl;
        return;
    }
    while (!inputFile.eof()) {
        std::getline(inputFile, hashValueString);
        if (hashValueString.empty()) break;
        hashValueString.pop_back();
        inputHashValueVector.push_back(hashValueString);
    }
    if (inputHashValueVector.empty()) {
        std::cout << "The input file is empty!" << std::endl;
        return;
    }
    inputFile.close();

    for (const std::string& hashValue : inputHashValueVector) {
        std::vector<int> hashIndex = hashFunc(hashValue);
        for (int i = 0; i < NUM_HASH_FUNCS; ++i) {
            bloomFilter[hashIndex[i]] = true;
        }
    }
    std::cout << "Complete load the new input file \"" << inputFilePath << "\" to the Bloom filter" << std::endl;
    startState = false;
}

void addValue() {
    std::cout << "Type the new value for Bloom filter: ";
    std::string inputValue;
    std::cin >> inputValue;
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    std::vector<int> hashIndex = hashFunc(inputValue);
    for (int i = 0; i < NUM_HASH_FUNCS; ++i) bloomFilter[hashIndex[i]] = true;
    std::cout << "Complete add \"" << inputValue << "\" to the Bloom filter" << std::endl;
    startState = false;
}

void loadCheckFile() {
    std::cout << "Type the name or path of the check file: ";
    std::string checkFilePath;
    std::ifstream checkFile;
    std::vector<std::string> checkHashValueVector;
    std::string hashValueString;
    std::cin >> checkFilePath;
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    checkFile.open(checkFilePath, std::ios::in);
    while (!checkFile.eof()) {
        std::getline(checkFile, hashValueString);
        if (hashValueString.empty()) break;
        checkHashValueVector.push_back(hashValueString);
    }
    if (checkHashValueVector.empty()) {
        std::cout << "The check file is empty!" << std::endl;
        return;
    }
    checkFile.close();

    for (const std::string& hashValue : checkHashValueVector) {
        std::vector<int> hashIndex = hashFunc(hashValue);
        for (int i = 0; i < NUM_HASH_FUNCS; ++i) {
            if (bloomFilter[hashIndex[i]]) {
                std::cout << "Exist " << hashValue << std::endl;
                break;
            }
        }
    }
    std::cout << "Complete check values in \"" << checkFilePath << "\" with the Bloom filter" << std::endl;
    startState = false;
}

void checkValue() {
    std::cin.clear();
    std::cout << "Type the value which need to check on Bloom filter: ";
    std::string inputValue;
    std::cin >> inputValue;
    std::cin.clear();
    std::cin.ignore(10000,'\n');
    std::vector<int> hashIndex = hashFunc(inputValue);
    for (int i = 0; i < NUM_HASH_FUNCS; ++i) {
        if (bloomFilter[hashIndex[i]]) {
            std::cout << "Exist " << inputValue << std::endl;
            break;
        }
    }
    startState = false;
}

void restartBF() {
    std::cin.clear();
    startState = true;
    bloomFilter.reset();
    std::cout << "The Bloom filter has been reset!" << std::endl;
}


int main() {
    int option;
    while (true) {
        showUsage();
        if (startState) {
            std::cout << "Select some option in panel: ";
        } else {
            std::cout << "Select new option in panel: ";
        }
        std::cin >> option;
        std::cin.clear();
        std::cin.ignore(10000,'\n');
        if (std::cin.fail()) {
            std::cout << "Invalid the option value!" << std::endl;
            continue;
        }
        if (option == 0) {
            showBFInfo();
            continue;
        }
        if (option == 1) {
            loadInputFile();
            continue;
        }
        if (option == 2) {
            addValue();
            continue;
        }
        if (option == 3) {
            loadCheckFile();
            continue;
        }
        if (option == 4) {
            checkValue();
            continue;
        }
        if (option == 5) {
            restartBF();
            continue;
        }
        if (option == 6) {
            break;
        }
        std::cout << "Invalid command value!" << std::endl;
    }
    return 0;
}
