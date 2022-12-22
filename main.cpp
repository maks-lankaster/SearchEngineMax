#include <iostream>
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include "Exceptions.h"

#define APP_VERSION "1.0"

void starting() {
    std::cout << "Starting..." << std::endl;
    nlohmann::json configData;
    std::ifstream configFile("config.json");
    if(!configFile.is_open()) {
        throw ConfigFileIsMissingException();
    }
    std::cout << "Reading config file..." << std::endl;
    configFile >> configData;
    if (configData.empty()) {
        throw ConfigFileIsEmptyException();
    }
    configFile.close();
    if (APP_VERSION != configData["config"]["version"]) {
        throw IncorrectVersionException();
    }
    std::ofstream oAnswers("answers.json"); //checking if answers.json is writable (if exists) or create a new answers.json
    if (oAnswers.is_open()) {
        std::cout << "Creating new or erasing contents of an existing 'answers.json' file..." << std::endl;
        oAnswers << "";
    } else {
        throw FileNotWritableException();
    }
    std::cout << std::endl << "WELCOME TO " << std::string(configData["config"]["name"]) << std::endl << std::endl;
}

std::vector<std::vector<std::pair<int, float>>> convertFromRelativeIndex(std::vector<std::vector<RelativeIndex>>& answersRelativeIndex) {
    std::vector<std::vector<std::pair<int, float>>> answersPair;

    for (const auto& answers : answersRelativeIndex) {
        std::vector<std::pair<int, float>> pairsIntFloat;
        for (auto answer : answers) {
            std::pair<std::size_t, float> pairIntFloat;
            pairIntFloat.first = answer.docId;
            pairIntFloat.second = answer.rank;
            pairsIntFloat.emplace_back(pairIntFloat);
        }
        answersPair.push_back(pairsIntFloat);
    }
    return answersPair;
}

int main() {
    try {
        starting();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cout << "Application can't start" << std::endl;
        return 0;
    }

    std::cout << "Search started..." << std::endl;

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
    std::vector<std::vector<RelativeIndex>> answersRelativeIndex;
    answersRelativeIndex = searchServer.search(requests);

    //Converting answers of type RelativeIndex to type std::pair<std::size_t, float> to use with putAnswers function
    auto answersPair = convertFromRelativeIndex(answersRelativeIndex);
    ConverterJSON::putAnswers(answersPair);
    std::cout << std::endl << "PROGRAM FINISHED CORRECTLY" << std::endl;
}