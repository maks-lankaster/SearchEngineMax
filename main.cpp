#include <iostream>
#include <filesystem>

#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"
#include "Exceptions.h"
#include "nlohmann/json.hpp"

#define APP_VERSION "1.0"

void writeConfigFile() {
    nlohmann::json configData;
    std::vector<std::string> filesVec;
    std::string filePath = "resources/";
    for (const auto &file : std::filesystem::directory_iterator(filePath)) {
        filesVec.push_back(file.path().string());
    }

    configData = {
            {"config", {
                               {"name", "MAX SEARCH ENGINE"},
                               {"version", "1.0"},
                               {"maxResponses", 5}

                       } },
            {"files", filesVec}
    };
    std::ofstream oConfigFile("config.json");
    oConfigFile << configData;
    oConfigFile.close();
}

void writeRequestsFile() {
    nlohmann::json requestJSON;
    std::vector<std::string> requests;
    requests = {{"the day morning"}, {"stop start end"}, {"privet poka"}, {"oh my god"}};
    requestJSON = {
            {"requests", requests}
    };
    std::ofstream oRequestsFile("requests.json");
    oRequestsFile << requestJSON;
    oRequestsFile.close();
}

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
    //Functions to create config.json and requests.json (for testing purposes)
    //writeConfigFile();
    //writeRequestsFile();
    try {
        starting();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cout << "Application can't start" << std::endl;
        return 0;
    }

    //Creating ConverterJSON object
    ConverterJSON converterJSON;

    //Creating InvertedIndex object
    InvertedIndex invertedIndex;

    //Updating inverted index document base
    invertedIndex.updateDocumentBase(converterJSON.getTextDocuments());

    //Creating SearchServer object
    SearchServer searchServer(invertedIndex);

    //Getting search requests from requests.json
    std::vector<std::string> requests;
    try {
        requests = converterJSON.getRequests();
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
    converterJSON.putAnswers(answersPair);
}