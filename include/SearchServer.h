//
// Created by Max on 01.11.2022.
//

#ifndef SEARCHENGINE_SEARCHSERVER_H
#define SEARCHENGINE_SEARCHSERVER_H

#include "InvertedIndex.h"

struct RelativeIndex{
    std::size_t docId;
    float rank;
    bool operator ==(const RelativeIndex& other) const {
        return (docId == other.docId && rank == other.rank);
    }
};

class SearchServer {
public:

    SearchServer(InvertedIndex& idx) : iIndex(idx){ };

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& requests) const;
private:
    InvertedIndex iIndex;
};


#endif //SEARCHENGINE_SEARCHSERVER_H
