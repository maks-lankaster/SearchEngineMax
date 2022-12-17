//
// Created by Max on 01.11.2022.
//

#include <map>
#include <unordered_set>
#include <algorithm>
#include "ConverterJSON.h"
#include "SearchServer.h"

int defineNumberOfResponses(std::size_t currentNumber) {
    int maxResponses = ConverterJSON::getResponsesLimit();
    return (currentNumber > maxResponses ? maxResponses : currentNumber);
}

std::unordered_set<std::string> getUniqueWordsFromRequest(const std::string& request) {
    std::unordered_set<std::string> uniqueWords;
    std::string word;
    //Dividing request into separate words and inserting them into the list of unique words
    for (auto ch : request) {
        if (ch == ' ') {
            if (!word.empty()) {
                uniqueWords.insert(word);
                word = "";
            }
        } else {
            word += ch;
        }
    }
    if (!word.empty()) uniqueWords.insert(word);
    return uniqueWords;
}

std::vector<std::pair<std::string, std::size_t>> getUniqueWordsOccurrencesInDocs(const std::unordered_set<std::string>&
        uniqueWords, const InvertedIndex& iIndex) {
    std::vector<std::pair<std::string, std::size_t>> uniqueWordsOccurrencesSorted;
    for (const auto& uniqueWord : uniqueWords) {
        std::vector<Entry> entries = iIndex.getWordCount(uniqueWord);
        if(!entries.empty()) {
            std::size_t sumCount = 0;
            for (auto entry: entries) {
                sumCount += entry.count;
            }
            uniqueWordsOccurrencesSorted.emplace_back(uniqueWord, sumCount);
        }
    }
    if (!uniqueWordsOccurrencesSorted.empty()) {
        std::sort(uniqueWordsOccurrencesSorted.begin(), uniqueWordsOccurrencesSorted.end(), [](std::pair<std::string,
                std::size_t> &a, std::pair<std::string, std::size_t> &b) { return a.second < b.second; });
    }
    return uniqueWordsOccurrencesSorted;
}

std::map<std::size_t, float> getDocsAbsoluteRelevance(const std::vector<std::pair<std::string, size_t>>& uniqueWords,
                                                      const InvertedIndex& iIndex) {
    std::map<std::size_t, float> docsAbsRelevance;
    for (const auto& currentWord: uniqueWords) { //Loop through sorted words of one request
        std::vector<Entry> entries = iIndex.getWordCount(currentWord.first); //Get all docs (Entry objects) with the current word
        for (auto currentEntry: entries) { //Loop through docs (Entry objects) with the current word
            float relevanceAbs = 0;
            for (const auto& otherWord: uniqueWords) { //Loop through sorted words of one request to compare with the current word
                std::vector<Entry> otherEntries = iIndex.getWordCount(
                        otherWord.first); //Get all docs (Entry objects) with the 'other' word
                for (auto otherEntry : otherEntries) { //Loop through 'other' docs
                    if (otherEntry.docId == currentEntry.docId) { //Find if 'other' word appears in the 'current' word docs
                        relevanceAbs += otherEntry.count; //Increase relevance of the 'current' doc
                        break;
                    }
                }
            }
            docsAbsRelevance.insert({currentEntry.docId, relevanceAbs});
        }
    }
    return docsAbsRelevance;
}

std::vector<RelativeIndex> makeRelativeIndex(const std::map<std::size_t, float>& docsAbsRelevance) {
    std::vector<RelativeIndex> relativeIndexes;
    for(auto doc: docsAbsRelevance) {
        RelativeIndex ri{};
        ri.docId = doc.first;
        ri.rank = doc.second;
        relativeIndexes.push_back(ri);
    }
    //SORTING BY RANK AND BY DOCID (IF RANK IS EQUAL)
    std::sort(relativeIndexes.begin(), relativeIndexes.end(), [](RelativeIndex &a, RelativeIndex &b) {
        if (a.rank == b.rank) {
            return a.docId < b.docId;
        } else {
            return a.rank > b.rank;
        }});
    float maxAbsRelevance = relativeIndexes[0].rank;
    //Define number of answers based on maxResponses from config.json file
    int responsesLimit = defineNumberOfResponses(relativeIndexes.size());
    relativeIndexes.resize(responsesLimit);
    for(std::size_t i = 0; i < relativeIndexes.size(); i++) {
        relativeIndexes[i].rank /= maxAbsRelevance;
    }
    return relativeIndexes;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string> &requests) const {
    std::vector<std::vector<RelativeIndex>> answers;
    for (const auto& request : requests) {
        auto uniqueWords = getUniqueWordsFromRequest(request); //Creating a list of unique words in one request
        auto uniqueWordsOccurrencesSorted =
                getUniqueWordsOccurrencesInDocs(uniqueWords, iIndex); //Counting total occurrences of a unique word in all documents sorted (from smaller to larger)
        if(!uniqueWordsOccurrencesSorted.empty()) {
            std::map<std::size_t, float> docsAbsRelevance =
                    getDocsAbsoluteRelevance(uniqueWordsOccurrencesSorted, iIndex);
            std::vector<RelativeIndex> relativeIndexes = makeRelativeIndex(docsAbsRelevance);
            answers.push_back(relativeIndexes);
        } else {
            std::vector<RelativeIndex> ri{};
            answers.push_back(ri);
        }
    }
    return answers;
}
