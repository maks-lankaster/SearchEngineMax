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
    bool operator ==(const Entry& other) const {
        return (docId == other.docId &&
                count == other.count);
    }
};

class InvertedIndex {
public:
    InvertedIndex() = default;

    void updateDocumentBase(std::vector<std::string> inputDocs);

    std::vector<Entry> getWordCount(const std::string& word) const;
private:
    std::vector<std::string> docs; // список содержимого документов
    std::map<std::string, std::vector<Entry>> freqDictionary; // частотный словарь
};


#endif //SEARCHENGINE_INVERTEDINDEX_H
