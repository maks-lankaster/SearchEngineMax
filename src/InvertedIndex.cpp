#include "InvertedIndex.h"
#include "ConverterJSON.h"
#include <iostream>
#include <thread>
#include <utility>
#include <vector>
#include <mutex>

std::mutex m;

void getDocUniqueWordsCount(std::size_t docId, const std::string& docText, std::map<std::string, std::vector<Entry>>& freqDictionary) {
    std::string word;
    std::map<std::string, std::size_t> uniqueWordsCount;
    for (auto ch : docText) {
        if (ch == ' ') {
            if (!word.empty()) {
                uniqueWordsCount[word]++;
                word = "";
            }
        } else {
            word += ch;
        }
    }
    if (!word.empty()) uniqueWordsCount[word]++;
    
    std::lock_guard<std::mutex> lock(m);
    for (const auto& uniqueWord : uniqueWordsCount) {
        Entry entry{};
        entry.docId = docId;
        entry.count = uniqueWord.second;
        auto it = freqDictionary.insert({uniqueWord.first, {}});
        it.first->second.push_back(entry);
    }
}

void InvertedIndex::updateDocumentBase(std::vector<std::string> inputDocs) {
    docs = std::move(inputDocs);
    std::vector<std::thread> docThreads;
    for(std::size_t i = 0; i < docs.size(); i++) {
        docThreads.emplace_back(getDocUniqueWordsCount, i, docs[i], std::ref(freqDictionary));
    }

    for (std::size_t i = 0; i < docThreads.size(); i++) {
        docThreads[i].join();
    }
}

std::vector<Entry> InvertedIndex::getWordCount(const std::string &word) const {
    auto uniqueWord = freqDictionary.find(word);
    if (uniqueWord != freqDictionary.end()) {
        return uniqueWord->second;
    } else {
        return {};
    }
}
