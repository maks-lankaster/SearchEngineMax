#include <iostream>
#include <filesystem>

#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include "Exceptions.h"
#include "nlohmann/json.hpp"

#define APP_VERSION "1.0"

void starting() {
    std::cout << "Starting..." << std::endl;
    nlohmann::json configData;
    std::ifstream configFile("config.json");
    if(!configFile.is_open()) {
        throw ConfigFileIsMissingException();
    }
    configFile >> configData;
    if (configData.empty()) {
        throw ConfigFileIsEmptyException();
    }
    configFile.close();
    if (APP_VERSION != configData["config"]["version"]) {
        throw IncorrectVersionException();
    }
    std::cout << "WELCOME TO " << std::string(configData["config"]["name"]) << std::endl;
}

int main() {
    try {
        starting();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cout << "Application can't start" << std::endl;
        return 0;
    }

    //Creating InvertedIndex object
    InvertedIndex invertedIndex;

    //Updating inverted index document base
    invertedIndex.updateDocumentBase(ConverterJSON::getTextDocuments());

    //Creating SearchServer object
    SearchServer searchServer(invertedIndex);

    //Getting search requests from requests.json
    std::vector<std::string> requests;
    try {
        requests = ConverterJSON::getRequests();
    } catch (FileNotFoundException &e) {
        std::cerr << e.what() << std::endl;
    }

    //Getting answers to requests
    std::vector<std::vector<RelativeIndex>> answersRelInd;
    answersRelInd = searchServer.search(requests);

    //Converting answers of type RelativeIndex to type std::pair<std::size_t, float> to use with putAnswers function
    std::vector<std::vector<std::pair<int, float>>> answersPair;
    for (int i = 0; i < answersRelInd.size(); i++) {
        std::vector<std::pair<int, float>> entries;
        for (int j = 0; j < answersRelInd[i].size(); j++) {
            std::pair<std::size_t, float> entry;
            entry.first = answersRelInd[i][j].docId;
            entry.second = answersRelInd[i][j].rank;
            entries.push_back(entry);
        }
        answersPair.push_back(entries);
    }
    ConverterJSON::putAnswers(answersPair);
}