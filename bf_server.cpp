//
// Created by cuongbv on 28/04/2020.
//

#include <bitset>
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 6595
#define NUM_HASH_FUNCS 4
#define BF_SIZE 57707801

std::bitset<BF_SIZE> bloomFilter;
bool startState = true;

/** Split the input string with delim (default is space ' '). This is void function (pure procedure).
 * @tparam Container: the C++ STL container (std::vector, std::list, std::set, ...).
 * @param str: the input string will be split.
 * @param cont: the container object will contain the result after split.
 * @param delim: the separator will been used for split procedure.
 */
template <class Container>
void splitString(const std::string& str, Container& cont, char delim = ' ') {
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

/** Hash the input value with number of hash function.
 * @param inputValue: the input value will be hashed.
 * @return The collection of result values (correspond to hash function)
 */
std::vector<int> hashFunc(std::string inputValue) {
    std::vector<int> result;
    result.reserve(NUM_HASH_FUNCS);
    for (int i = 0; i < NUM_HASH_FUNCS; ++i) {
        inputValue += std::to_string(i);
        std::hash<std::string> hasher;
        auto hashed = hasher(inputValue);
        result[i] = (int) (hashed % BF_SIZE);
    }
    return result;
}

/** The guide for this Bloom filter socket client/server architecture.
 *
 * @return String, which is a guide panel for user at client side.
 */
std::string showUsage() {
    std::stringstream stringstream;
    stringstream << std::endl;
    stringstream << "|*****************************************************|" << std::endl;
    stringstream << "|**************** Bloom Filter Usage *****************|" << std::endl;
    stringstream << "|*****************************************************|" << std::endl;
    stringstream << "| Show Bloom filter info: $ ./bf_client show_info     |" << std::endl;
    stringstream << "| Load new input file: $ ./bf_client load_file <path> |" << std::endl;
    stringstream << "| Add new value: $ ./bf_client add_value <value>      |" << std::endl;
    stringstream << "| Load check file: $ ./bf_client load_check <path>    |" << std::endl;
    stringstream << "| Check new value: $ ./bf_client check_value <value>  |" << std::endl;
    stringstream << "| Reset Bloom filter: $ ./bf_client reset             |" << std::endl;
    stringstream << "|*****************************************************|" << std::endl;
    stringstream << std::endl;
    startState = false;
    return stringstream.str();
}

/** Show the information of Bloom filter.
 *
 * @return String, which is a collection of information about the Bloom filter structure at server side.
 */
std::string showBFInfo() {
    std::stringstream stringstream;
    stringstream << std::endl;
    stringstream << "Bloom filter information:" << std::endl;
    stringstream << "Number of hash functions: " << NUM_HASH_FUNCS << std::endl;
    stringstream << "Size of the Bloom filter: " << BF_SIZE << std::endl;
    stringstream << "Maximum storage capacity value: " << (int) (BF_SIZE / (NUM_HASH_FUNCS /  log(2))) << std::endl;
    stringstream << "False positive probability: " << (int)(pow(2, -NUM_HASH_FUNCS) * 100) << "%" << std::endl;
    stringstream << std::endl;
    startState = false;
    return stringstream.str();
}

/** Load the input file (at server side) into the Bloom filter (at server side).
 *
 * @param inputFilePath: path of input file (at server side).
 * @return String, which is a notification about load input file procedure.
 */
std::string loadInputFile(const std::string& inputFilePath) {
    std::stringstream stringstream;
    std::ifstream inputFile;
    std::vector<std::string> inputHashValueVector;
    std::string hashValueString;
    inputFile.open(inputFilePath);
    if (inputFile.fail()) {
        stringstream << "The input file is not exist!" << std::endl;
        return stringstream.str();
    }
    while (!inputFile.eof()) {
        std::getline(inputFile, hashValueString);
        if (hashValueString.empty()) break;
        hashValueString.pop_back();
        inputHashValueVector.push_back(hashValueString);
    }
    if (inputHashValueVector.empty()) {
        stringstream << "The input file is empty!" << std::endl;
        return stringstream.str();
    }
    inputFile.close();

    for (const std::string& hashValue : inputHashValueVector) {
        std::vector<int> hashIndex = hashFunc(hashValue);
        for (int i = 0; i < NUM_HASH_FUNCS; ++i) {
            bloomFilter[hashIndex[i]] = true;
        }
    }
    stringstream << "Complete load the new input file \"" << inputFilePath << "\" into the Bloom filter" << std::endl;
    startState = false;
    return stringstream.str();
}

/** Add 1 value into the Bloom filter (at server side).
 *
 * @param inputValue: the value will be add into the Bloom filter.
 * @return A string, which is a notification about add input value procedure into the Bloom filter (at server side).
 */
std::string addValue(const std::string& inputValue) {
    std::stringstream stringstream;
    std::vector<int> hashIndex = hashFunc(inputValue);
    for (int i = 0; i < NUM_HASH_FUNCS; ++i) bloomFilter[hashIndex[i]] = true;
    stringstream << "Complete add \"" << inputValue << "\" to the Bloom filter" << std::endl;
    startState = false;
    return stringstream.str();
}

/** Load the checking file for the Bloom filter (at server side).
 *
 * @param checkFilePath: the path of the checking file, which will be matched with the Bloom filter (at server side).
 * @return A string, which is a notification about checking file procedure with the Bloom filter (at server side).
 */
std::string loadCheckFile(const std::string& checkFilePath) {
    std::stringstream stringstream;
    std::ifstream checkFile;
    std::vector<std::string> checkHashValueVector;
    std::string hashValueString;
    // Load all of values in checking file into memory
    checkFile.open(checkFilePath, std::ios::in);
    while (!checkFile.eof()) {
        std::getline(checkFile, hashValueString);
        if (hashValueString.empty()) break;
        checkHashValueVector.push_back(hashValueString);
    }
    if (checkHashValueVector.empty()) {
        stringstream << "The check file is empty!" << std::endl;
        return stringstream.str();
    }
    checkFile.close();

    // Check each value with the Bloom filter
    for (const std::string& hashValue : checkHashValueVector) {
        std::vector<int> hashIndex = hashFunc(hashValue);
        bool isExist = true;
        for (int i = 0; i < NUM_HASH_FUNCS; ++i) {
            if (!bloomFilter[hashIndex[i]]) {
                isExist = false;
                break;
            }
        }
        if (isExist) stringstream << "Exist " << hashValue << std::endl;
    }
    stringstream << "Complete check values in \"" << checkFilePath << "\" with the Bloom filter" << std::endl;
    startState = false;
    return stringstream.str();
}

/** Check a value with the Bloom filter (at server side).
 *
 * @param checkValue: the value will be check with the Bloom filter (at server side).
 * @return A string, which is a notification about checking value with the Bloom filter (at server side).
 */
std::string checkValue(const std::string& checkValue) {
    std::stringstream stringstream;
    std::vector<int> hashIndex = hashFunc(checkValue);
    bool isExist = true;
    for (int i = 0; i < NUM_HASH_FUNCS; ++i) {
        if (!bloomFilter[hashIndex[i]]) {
            isExist = false;
            break;
        }
    }
    startState = false;
    if (isExist) stringstream << "Exist " << checkValue << std::endl;
    else stringstream << "Not exist" << std::endl;
    return stringstream.str();
}

/** Reset the Bloom filter at server side.
 *
 * @return A string, which is a notification about reset procedure the Bloom filter (at server side).
 */
std::string restartBF() {
    std::stringstream stringstream;
    startState = true;
    bloomFilter.reset();
    stringstream << "The Bloom filter has been reset!" << std::endl;
    return stringstream.str();
}

/** The main program run as a Socket Server (C) with Bloom filter core (C++). Run forever.
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char const *argv[]) {
    int socketServer;       // The socket file description of the socket server side
    int incomingClient;     // The socket file description of a socket client side
    int readStatus;         // status when read request from a Socket Client
    struct sockaddr_in sockServerAddr{};        // The structure of a Socket Server address
    int optionName = 1;     // OPTNAME param of setsockopt() call
    int addrLen = sizeof(sockServerAddr);

    // Creating the Socket Server
    if ((socketServer = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Fail to create a Socket Server\n");
        exit(EXIT_FAILURE);
    }

    // Bind the Socket Server address with necessary parameter
    if (setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optionName, sizeof(optionName))) {
        perror("ERROR at Set Socket Server Option name\n");
        exit(EXIT_FAILURE);
    }
    sockServerAddr.sin_family = AF_INET;        // IPv4
    sockServerAddr.sin_addr.s_addr = INADDR_ANY;        // the loopback address
    sockServerAddr.sin_port = htons(PORT);      // Encode port value
    // Forcefully attaching socket to the port value on Device
    if (bind(socketServer, (struct sockaddr *)&sockServerAddr, sizeof(sockServerAddr)) < 0) {
        perror("Fail to Bind socket to the TCP port!\n");
        exit(EXIT_FAILURE);
    }

    // The infinity loop. The Socket Server will listen connect forever
    while (true) {
        char receiveBuffer[1024] = {0};     // The buffer which will contain a request command from a Socket Client

        // Listen the incoming request on assigned port
        if (listen(socketServer, 3) < 0) {
            std::cerr << "Could not listen on assign port " << PORT << std::endl;
            exit(EXIT_FAILURE);
        }

        // Intecept the incoming request from Socket Client side
        if ((incomingClient = accept(socketServer, (struct sockaddr *) &sockServerAddr, (socklen_t *) &addrLen)) < 0) {
            std::cerr << "Could not accept request from a socket client side" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Read and store the incoming request command from the Socket Client side
        readStatus = read(incomingClient, receiveBuffer, 1024);
        if (readStatus < 0) {
            std::cerr << "Fail to read request command from Socket client!" << std::endl;
            return 1;
        }

        printf("From client: %s\n", receiveBuffer);

        // Handle input command from Socket Client
        std::vector<std::string> requestCommandVector;
        std::string passingCommand (receiveBuffer);
        splitString(passingCommand, requestCommandVector);

        if (requestCommandVector[0] == "show_info") {
            std::string passingString = showBFInfo();
            send(incomingClient, passingString.c_str(), strlen(passingString.c_str()), 0);
            continue;
        }

        if (requestCommandVector[0] == "load_file") {
            if (requestCommandVector.size() > 1) {
                std::string response = loadInputFile(requestCommandVector[1]);
                send(incomingClient, response.c_str(), strlen(response.c_str()), 0);
                continue;
            }
            send(incomingClient, showUsage().c_str(), strlen(showUsage().c_str()), 0);
            continue;
        }

        if (requestCommandVector[0] == "add_value") {
            if (requestCommandVector.size() > 1) {
                std::string response = addValue(requestCommandVector[1]);
                send(incomingClient, response.c_str(), strlen(response.c_str()), 0);
                continue;
            }
            send(incomingClient, showUsage().c_str(), strlen(showUsage().c_str()), 0);
            continue;
        }

        if (requestCommandVector[0] == "load_check") {
            if (requestCommandVector.size() > 1) {
                std::string response = loadCheckFile(requestCommandVector[1]);
                send(incomingClient, response.c_str(), strlen(response.c_str()), 0);
                continue;
            }
            send(incomingClient, showUsage().c_str(), strlen(showUsage().c_str()), 0);
            continue;
        }

        if (requestCommandVector[0] == "check_value") {
            if (requestCommandVector.size() > 1) {
                std::string response = checkValue(requestCommandVector[1]);
                send(incomingClient, response.c_str(), strlen(response.c_str()), 0);
                continue;
            }
            send(incomingClient, showUsage().c_str(), strlen(showUsage().c_str()), 0);
            continue;
        }

        if (requestCommandVector[0] == "reset") {
            std::string response = restartBF();
            send(incomingClient, response.c_str(), strlen(response.c_str()), 0);
            continue;
        }

        // Default, show guide panel
        send(incomingClient, showUsage().c_str(), strlen(showUsage().c_str()), 0);
    }
    return 0;
}
