//
// Created by Max on 30.10.2022.
//

#ifndef SEARCHENGINE_CONVERTERJSON_H
#define SEARCHENGINE_CONVERTERJSON_H
#include <vector>
#include <fstream>
#include "nlohmann/json.hpp"

class ConverterJSON {
public:
    ConverterJSON() = default;

    std::vector<std::string> getTextDocuments();

    int getResponsesLimit();

    std::vector<std::string> getRequests();

    void putAnswers(std::vector<std::vector<std::pair<int, float>>> answers);
};

#endif //SEARCHENGINE_CONVERTERJSON_H
