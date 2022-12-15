//
// Created by Max on 01.11.2022.
//

#include "InvertedIndex.h"
#include "ConverterJSON.h"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::mutex m;

void getDocUniqueWordsCount(std::size_t docId, std::string docText, std::map<std::string, std::vector<Entry>>& freqDictionary) {
    std::string word = "";
    std::map<std::string, std::size_t> uniqueWordsCount;
    for (auto ch : docText) {
        if (ch == ' ') {
            if (!word.empty()) {
                uniqueWordsCount[word]++;
                word = "";
            } else {
                word = "";
            };
        } else {
            word += ch;
        }
    }
    if (!word.empty()) uniqueWordsCount[word]++;
    
    std::lock_guard<std::mutex> lock(m);
    for (auto uniqueWord : uniqueWordsCount) {
        Entry entry;
        entry.docId = docId;
        entry.count = uniqueWord.second;
        auto it = freqDictionary.insert({uniqueWord.first, {}});
        it.first->second.push_back(entry);
    }
}

void InvertedIndex::updateDocumentBase(std::vector<std::string> inputDocs) {
    docs = inputDocs;
    std::vector<std::thread> docThreads;
    for(std::size_t i = 0; i < docs.size(); i++) {
        std::thread thr(getDocUniqueWordsCount, i, docs[i], std::ref(freqDictionary));
        docThreads.push_back(std::move(thr));
    }
    for (std::size_t i = 0; i < docThreads.size(); i++) {
        docThreads[i].join();
    }
}

std::vector<Entry> InvertedIndex::getWordCount(const std::string &word) {
    auto uniqueWord = freqDictionary.find(word);
    if (uniqueWord != freqDictionary.end()) {
        return uniqueWord->second;
    } else {
        return {};
    }
}
