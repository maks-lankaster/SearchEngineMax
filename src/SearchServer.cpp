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
            uniqueWordsOccurrencesSorted.push_back({uniqueWord, sumCount});
        }
    }
    if (!uniqueWordsOccurrencesSorted.empty()) {
        std::sort(uniqueWordsOccurrencesSorted.begin(), uniqueWordsOccurrencesSorted.end(), [](std::pair<std::string,
                std::size_t> &a, std::pair<std::string, std::size_t> &b) { return a.second < b.second; });
    }
    return uniqueWordsOccurrencesSorted;
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string> &requests) const {
    std::vector<std::vector<RelativeIndex>> answers;
    for (const auto& request : requests) {
        //Creating a list of unique words in one request
        std::unordered_set<std::string> uniqueWords = getUniqueWordsFromRequest(request);

        //Counting total occurrences of a unique word in all documents
        std::vector<std::pair<std::string, std::size_t>> uniqueWordsOccurrencesSorted = getUniqueWordsOccurrencesInDocs(uniqueWords, iIndex);

        //Sorting unique words by occurrences in all documents (from fewer to greater)
        if(!uniqueWordsOccurrencesSorted.empty()) {
            std::map<std::size_t, float> docsFound;
            for (const auto& currentWord: uniqueWordsOccurrencesSorted) { //Loop through sorted words of one request
                std::vector<Entry> entries = iIndex.getWordCount(currentWord.first); //Get all docs (Entry objects) with the current word
                for (auto currentEntry: entries) { //Loop through docs (Entry objects) with the current word
                    float relevanceAbs = 0;
                    for (const auto& otherWord: uniqueWordsOccurrencesSorted) { //Loop through sorted words of one request to compare with the current word
                        std::vector<Entry> otherEntries = iIndex.getWordCount(
                                otherWord.first); //Get all docs (Entry objects) with the 'other' word
                        for (int i = 0; i < otherEntries.size(); i++) { //Loop through 'other' docs
                            if (otherEntries[i].docId == currentEntry.docId) { //Find if 'other' word appears in the 'current' word docs
                                relevanceAbs += otherEntries[i].count; //Increase relevance of the 'current' doc
                                break;
                            }
                        }
                    }
                    docsFound.insert({currentEntry.docId, relevanceAbs});
                }
            }
            std::vector<RelativeIndex> relInd;
            for(auto it: docsFound) {
                RelativeIndex ri{};
                ri.docId = it.first;
                ri.rank = it.second;
                relInd.push_back(ri);
            }

            //SORTING BY RANK AND BY DOCID (IF RANK IS EQUAL)
            std::sort(relInd.begin(), relInd.end(), [](RelativeIndex &a, RelativeIndex &b) {
                if (a.rank == b.rank) {
                    return a.docId < b.docId;
                } else {
                    return a.rank > b.rank;
                }});

            float maxAbsRelevance = relInd[0].rank;

            //Define number of answers based on maxResponses from config.json file
            int responsesLimit = defineNumberOfResponses(relInd.size());

            relInd.resize(responsesLimit);

            for (int i = 0; i < relInd.size(); i++) {
                relInd[i].rank /= maxAbsRelevance;
            }
            answers.push_back(relInd);
        } else {
            std::vector<RelativeIndex> ri{};
            answers.push_back(ri);
        }
    }
    return answers;
}
