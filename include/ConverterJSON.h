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

/**
* Метод получения содержимого файлов
* @return Возвращает список с содержимым файлов перечисленных
* в config.json
*/
    std::vector<std::string> getTextDocuments();
/**
* Метод считывает поле max_responses для определения предельного
* количества ответов на один запрос
* @return
*/
    int getResponsesLimit();
/**
* Метод получения запросов из файла requests.json
* @return возвращает список запросов из файла requests.json
*/
    std::vector<std::string> getRequests();
/**
* Положить в файл answers.json результаты поисковых запросов
*/
    void putAnswers(std::vector<std::vector<std::pair<int, float>>>
                    answers);
};

#endif //SEARCHENGINE_CONVERTERJSON_H
