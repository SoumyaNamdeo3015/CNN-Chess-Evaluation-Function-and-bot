#include "chess-library-master/include/chess.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace chess;

class OpeningBook {
public:
    OpeningBook(const std::string& path) {
        // Open book file and load into memory
        std::ifstream file(path, std::ios::binary);

        if(!file) {
            std::cerr << "Failed to open book file: " << path << std::endl;
            return;
        }

        cout << "Book file Opened, Searching..."<<endl;

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        entries.resize(size / sizeof(PolyglotEntry));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(entries.data()), size);
    }

    std::vector<Move> getBookMoves(const Board& board) {
        std::vector<Move> result;
        uint64_t key = board.zobrist(); // Implement Zobrist hashing
        
        for(const auto& entry : entries) {
            if(entry.key == key) {
                result.push_back(entry.move);
            }
        }
        return result;
    }

private:
    #pragma pack(push, 1)
    struct PolyglotEntry {
        uint64_t key;
        uint16_t move;
        uint16_t weight;
        uint32_t learn;
    };
    #pragma pack(pop)
    
    std::vector<PolyglotEntry> entries;
};
