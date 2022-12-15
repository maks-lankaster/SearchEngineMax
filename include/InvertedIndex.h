//
// Created by Max on 01.11.2022.
//

#ifndef SEARCHENGINE_INVERTEDINDEX_H
#define SEARCHENGINE_INVERTEDINDEX_H
#include <vector>
#include <string>
#include <map>

struct Entry {
    std::size_t docId, count;
// Данный оператор необходим для проведения тестовых сценариев
    bool operator ==(const Entry& other) const {
        return (docId == other.docId &&
                count == other.count);
    }
};

class InvertedIndex {
public:
    InvertedIndex() = default;
/**
* Обновить или заполнить базу документов, по которой будем совершать
поиск
* @param texts_input содержимое документов
*/
    void updateDocumentBase(std::vector<std::string> inputDocs);
/**
* Метод определяет количество вхождений слова word в загруженной базе
документов
* @param word слово, частоту вхождений которого необходимо определить
* @return возвращает подготовленный список с частотой слов
*/
    std::vector<Entry> getWordCount(const std::string& word);
private:
    std::vector<std::string> docs; // список содержимого документов
    std::map<std::string, std::vector<Entry>> freqDictionary; // частотный словарь
};


#endif //SEARCHENGINE_INVERTEDINDEX_H
