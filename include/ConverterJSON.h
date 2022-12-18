#ifndef SEARCHENGINE_CONVERTERJSON_H
#define SEARCHENGINE_CONVERTERJSON_H
#include <vector>
#include <fstream>
#include "nlohmann/json.hpp"

class ConverterJSON {
public:
    ConverterJSON() = default;

    static std::vector<std::string> getTextDocuments() ;

    static int getResponsesLimit();

    static std::vector<std::string> getRequests();

    static void putAnswers(std::vector<std::vector<std::pair<int, float>>> answers);
};

#endif //SEARCHENGINE_CONVERTERJSON_H
