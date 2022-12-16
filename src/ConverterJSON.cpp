//
// Created by Max on 30.10.2022.
//
#include <iostream>
#include <io.h>
#include <sys/stat.h>
#include "ConverterJSON.h"
#include "Exceptions.h"
#include "nlohmann/json.hpp"

std::string getTextDocument(const std::string& filePath) {
    std::ifstream iFile(filePath);
    if (!iFile.is_open()) {
        throw FileNotFoundException();
    }
    std::string str;
    std::string resultStr;
    while (iFile >> str) {
        resultStr += (" " + str);
    }
    return resultStr;
}

std::vector<std::string> ConverterJSON::getTextDocuments() {
    nlohmann::json configData;
    std::ifstream configFile("config.json");
    configFile >> configData;
    configFile.close();
    std::vector<std::string> textDocuments;
    for (const auto& filePath : configData["files"]) {
        try {
            textDocuments.push_back(getTextDocument(filePath));
        } catch (FileNotFoundException &e) {
            std::cerr << e.what() << std::endl;
        }
    }
    return textDocuments;
}

int ConverterJSON::getResponsesLimit() {
    nlohmann::json configData;
    std::ifstream configFile("config.json");
    configFile >> configData;
    configFile.close();
    if (configData["config"]["maxResponses"].empty()) {
        return 5;
    } else {
        return configData["config"]["maxResponses"];
    }
}

std::vector<std::string> ConverterJSON::getRequests() {
    std::ifstream iFile("requests.json");
    if(!iFile.is_open()) {
        throw FileNotFoundException();
    }
    nlohmann::json requestsJSON;
    iFile >> requestsJSON;
    iFile.close();
    return requestsJSON["requests"];
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers) {
    nlohmann::json answersJSON;
    for (int i = 0; i < answers.size(); i++) {
        std::ostringstream requestNum;
        requestNum << "request" << std::setfill('0') << std::setw(3) << (i + 1);
        if (!answers[i].empty()) {
            answersJSON["answers"][requestNum.str()]["result"] = true;
            if(answers[i].size() > 1) {
                for (auto answer : answers[i]) {
                    answersJSON["answers"][requestNum.str()]["relevance"].push_back({{"docid", answer.first},
                                                                                     {"rank", answer.second}});
                }
            } else {
                answersJSON["answers"][requestNum.str()]["docid"] = answers[i][0].first;
                answersJSON["answers"][requestNum.str()]["rank"] = answers[i][0].second;
            }
        } else {
            answersJSON["answers"][requestNum.str()]["result"] = false;
        }
    }
    std::ofstream oAnswersFile("answers.json");
    oAnswersFile << answersJSON.dump(4, ' ');
    oAnswersFile.close();
    //setting the 'answers.json' as read-only
    _chmod("answers.json", _S_IREAD);
}

