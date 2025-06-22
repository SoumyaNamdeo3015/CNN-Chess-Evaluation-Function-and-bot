#include <iostream>
#include <random>
#include <map>
#include <vector>
#include "opening_book.cpp"
#include "evalf.h"

using namespace std;
using namespace chess;

#define INF 1e9

enum NodeType {
    EXACT,     // PV-node with exact evaluation
    ALPHA,     // Failed low node (upper bound)
    BETA       // Failed high node (lower bound)
};

struct TTEntry {
    uint64_t zobristKey;   // Position's unique hash
    int depth;             // Search depth at which this was calculated
    int score;             // Position's evaluation
    NodeType type;         // Type of node
    Move bestMove;         // Best move from this position
    
    TTEntry() : zobristKey(0), depth(-1), score(0), type(EXACT) {}
};

// Size should be a power of 2 for efficient indexing
const int TT_SIZE = 1 << 20; // 1 million entries
vector<TTEntry> transpositionTable(TT_SIZE);

const int MAX_DEPTH = 20;       //maximum_depth
Move killerMoves[MAX_DEPTH][2]; //2 killer moves per depth
int historyTable[12][64];       //[piece][to_square]

int getPieceIndex(Board& board , Square& square) {
    Piece piece = board.at(square);
    
    int id;
    if(piece == Piece::WHITEPAWN) {
        id = 0;
    } else if(piece == Piece::WHITEKNIGHT) {
        id = 1;
    } else if(piece == Piece::WHITEBISHOP) {
        id = 2;
    } else if(piece == Piece::WHITEROOK) {
        id = 3;
    } else if(piece == Piece::WHITEQUEEN) {
        id = 4;
    } else if(piece == Piece::WHITEKING) {
        id = 5;
    } else if(piece == Piece::BLACKPAWN) {
        id = 6;
    } else if(piece == Piece::BLACKKNIGHT) {
        id = 7;
    } else if(piece == Piece::BLACKBISHOP) {
        id = 8;
    } else if(piece == Piece::BLACKROOK) {
        id = 9;
    } else if(piece == Piece::BLACKQUEEN) {
        id = 10;
    } else {
        id = 11;
    }

    return id;
}

int evaluate(Board& board) {
    double eval = Evalf::get_evaluation(board.getFen()) * 10000;
    return (int)eval;
}

int getValue(const PieceType& type) {
    int value = 0;

    if(type == PieceType::PAWN) {
        value = 1;
    } else if(type == PieceType::KNIGHT) {
        value = 3; 
    } else if(type == PieceType::BISHOP) {
        value = 3;
    } else if(type == PieceType::ROOK) {
        value = 5;
    } else if(type == PieceType::QUEEN) {
        value = 9;
    }

    return value;
}

void moveOrdering(Movelist& moves , Board& board , int ply , Move hashMove) {
    for(Move& move : moves) {
        int moveScore = 0;
        Square from_square = move.from() , to_square = move.to();

        if(move == hashMove) {
            //hashMove has highest score
            moveScore = 1e7;
        } else if(board.at(to_square) != PieceType::NONE) {
            //capture move
            int victim_value = getValue(board.at(to_square).type());
            int aggressor_value = getValue(board.at(from_square).type());

            moveScore = 1e6 + (victim_value * 10 - aggressor_value);
        } else if(move == killerMoves[ply][0]) {
            moveScore = 9 * 1e5;
        } else if(move == killerMoves[ply][1]) {
            moveScore = 8 * 1e5;
        } else {
            int pieceId = getPieceIndex(board , from_square);
            int toSquare = to_square.index();

            moveScore = historyTable[pieceId][toSquare];
        }

        move.setScore(moveScore);
    }

    sort(moves.begin() , moves.end() , [](const Move& a , const Move& b) {
        return a.score() > b.score();
    });
}

void updateHistoryHeuristic(Board& board , Move& move , int depth) {
    Square from_square = move.from() , to_square = move.to();

    //update only quiet moves
    if(board.at(to_square) == PieceType::NONE) {
        int pieceId = getPieceIndex(board , from_square);
        int toSquare = to_square.index();

        historyTable[pieceId][toSquare] += depth * depth;
    }
}

void updateKillerMoves(Board& board , Move& move , int ply) {
    Square from_square = move.from() , to_square = move.to();

    //update only quiet moves
    if(board.at(to_square) == PieceType::NONE && move != killerMoves[ply][0]) {
        killerMoves[ply][1] = killerMoves[ply][0];
        killerMoves[ply][0] = move;
    }
}


//add feature later to increase depth by 1 if inCheck()
// Fixed alpha-beta implementation
int alpha_beta(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, int ply) {
    // Check terminal nodes first (before transposition table lookup)
    if(depth == 0) return evaluate(board);

    Movelist moves;
    movegen::legalmoves(moves, board);

    // Checkmate detection
    if(moves.empty()) {
        return board.inCheck() ? 
            (maximizingPlayer ? -INF + ply : INF - ply) : 0;  // Add ply to prioritize faster mates
    }

    // Draw detection
    if(board.isInsufficientMaterial() || board.isRepetition()) return 0;

    // Transposition table lookup
    uint64_t posKey = board.zobrist();
    TTEntry& entry = transpositionTable[posKey % TT_SIZE];
    Move hashMove = Move::NULL_MOVE;

    if(entry.zobristKey == posKey && entry.depth >= depth) {
        if(entry.type == EXACT) return entry.score;
        if(entry.type == ALPHA && entry.score <= alpha) return alpha;
        if(entry.type == BETA && entry.score >= beta) return beta;
        hashMove = entry.bestMove;
    }

    int originalAlpha = alpha;
    int bestScore = maximizingPlayer ? -INF : INF;
    Move bestMove = Move::NULL_MOVE;

    // Improved move ordering with SEE for captures
    moveOrdering(moves, board, ply, hashMove);

    for(Move& move : moves) {
        board.makeMove(move);
        int eval = alpha_beta(board, depth-1, alpha, beta, !maximizingPlayer, ply+1);
        board.unmakeMove(move);

        if(maximizingPlayer) {
            if(eval > bestScore) {
                bestScore = eval;
                bestMove = move;
                if(eval > alpha) alpha = eval;
            }
        } else {
            if(eval < bestScore) {
                bestScore = eval;
                bestMove = move;
                if(eval < beta) beta = eval;
            }
        }

        // Beta cutoff with proper history updates
        if(beta <= alpha) {
            if(!board.isCapture(move)) {  // Only update for quiet moves
                updateKillerMoves(board , move , ply);
                updateHistoryHeuristic(board, move, depth);
            }
            break;
        }
    }

    // Store in transposition table
    entry.zobristKey = posKey;
    entry.depth = depth;
    entry.score = bestScore;
    entry.bestMove = bestMove;
    entry.type = (bestScore <= originalAlpha) ? ALPHA : 
                (bestScore >= beta) ? BETA : EXACT;

    return bestScore;
}

// Fixed root search function
Move findBestMove(Board& board, int depth) {
    static OpeningBook book("Perfect_2023/BIN/Perfect2023.bin"); // Initialize once
    std::vector<Move> bookMoves = book.getBookMoves(board);
    
    if(!bookMoves.empty()) {
        // Select random move from book options
        cout<<"Found in book!"<<endl;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, bookMoves.size()-1);
        return bookMoves[dist(gen)];
    }

    bool maximizing = board.sideToMove() == Color::WHITE;
    int bestEval = maximizing ? -INF : INF;
    Move bestMove = Move::NULL_MOVE;

    Movelist moves;
    movegen::legalmoves(moves, board);
    moveOrdering(moves, board, 0, Move::NULL_MOVE);

    for(Move& move : moves) {
        cout << move << ' ';

        board.makeMove(move);
        int eval = alpha_beta(board, depth-1, -INF, INF, !maximizing, 1);
        board.unmakeMove(move);

        if((maximizing && eval > bestEval) || (!maximizing && eval < bestEval)) {
            bestEval = eval;
            bestMove = move;
        }
    }

    cout<<endl;

    return bestMove;
}

//Perfect_2023/BIN/Perfect2023.bin


#include <cctype> // For tolower()

uint16_t encodeMove(const std::string& moveStr) {
    // Helper function to convert file/rank to square index
    auto parseSquare = [](char file, char rank) -> int {
        return (rank - '1') * 8 + (file - 'a');
    };

    // Handle castling (king side and queen side)
    if (moveStr == "e1g1" || moveStr == "e1c1" || 
        moveStr == "e8g8" || moveStr == "e8c8") {
        
        const bool isKingSide = (moveStr[2] == 'g');
        const bool isWhite = (moveStr[1] == '1');

        Square from = static_cast<Square>(parseSquare(moveStr[0], moveStr[1]));
        Square to = static_cast<Square>(isKingSide ? 
            (isWhite ? parseSquare('h', '1') : parseSquare('h', '8')) :
            (isWhite ? parseSquare('a', '1') : parseSquare('a', '8')));

        return Move::make<Move::CASTLING>(from, to).move();
    }

    // Handle promotions (e.g., "a7a8q")
    if (moveStr.length() == 5) {
        Square from = static_cast<Square>(parseSquare(moveStr[0], moveStr[1]));
        Square to = static_cast<Square>(parseSquare(moveStr[2], moveStr[3]));
        
        const char promoChar = tolower(moveStr[4]);
        PieceType promo = PieceType::KNIGHT;
        switch(promoChar) {
            case 'q': promo = PieceType::QUEEN; break;
            case 'r': promo = PieceType::ROOK; break;
            case 'b': promo = PieceType::BISHOP; break;
            case 'n': promo = PieceType::KNIGHT; break;
            default: throw std::invalid_argument("Invalid promotion piece");
        }
        
        return Move::make<Move::PROMOTION>(from, to, promo).move();
    }

    // Handle normal moves and en passant
    if (moveStr.length() == 4) {
        Square from = static_cast<Square>(parseSquare(moveStr[0], moveStr[1]));
        Square to = static_cast<Square>(parseSquare(moveStr[2], moveStr[3]));
        
        // En passant would need board context to detect automatically
        return Move::make<Move::NORMAL>(from, to).move();
    }

    throw std::invalid_argument("Invalid move string format");
}



//need to handle castling

int main() {
    // Initialize the board to the starting position
    Board board;

    // Set the engine's search depth
    int aiDepth = 6;

    cout << "Welcome to Simple Chess Engine!" << endl;
    cout << "You are playing as White. Enter your moves in algebraic notation (e.g., e2e4)." << endl;
    cout << "Type 'quit' to exit the game." << endl;

    while (true) {
        // Display the current board
        cout << board << endl;

        // Check for game-ending conditions
        Movelist moves;
        movegen::legalmoves(moves, board);

        if (moves.empty()) {
            if (board.inCheck()) {
                cout << (board.sideToMove() == Color::WHITE ? "Black" : "White") << " wins by checkmate!" << endl;
            } else {
                cout << "Game ends in stalemate!" << endl;
            }
            break;
        }

        if (board.isInsufficientMaterial()) {
            cout << "Draw by insufficient material!" << endl;
            break;
        }

        if (board.isRepetition(3)) {
            cout << "Draw by threefold repetition!" << endl;
            break;
        }

        if (board.halfMoveClock() >= 100) {
            cout << "Draw by fifty-move rule!" << endl;
            break;
        }

        // If it's White's turn (player)
        if (board.sideToMove() == Color::WHITE) {
            string inputMove;
            Move playerMove = Move::NULL_MOVE;

            cout << "Your move: ";
            cin >> inputMove;

            if (inputMove == "quit") {
                cout << "Thanks for playing! Goodbye!" << endl;
                return 0;
            }

            playerMove = Move(encodeMove(inputMove));

            // Make the player's move
            board.makeMove(playerMove);
        } else { 
            // If it's Black's turn (engine)
            cout << "Engine is thinking..." << endl;

            Move engineMove = findBestMove(board, aiDepth);

            if (engineMove == Move::NULL_MOVE) {
                cout << "Engine could not find a valid move. Something went wrong!" << endl;
                break;
            }

            // Make the engine's move
            board.makeMove(engineMove);
            cout << "Engine played: " 
                 << char('a' + engineMove.from().index() % 8) 
                 << char('1' + engineMove.from().index() / 8)
                 << char('a' + engineMove.to().index() % 8)
                 << char('1' + engineMove.to().index() / 8)
                 << endl;
        }

        cout<<"Evaluation: "<<evaluate(board)<<endl;
    }

    return 0;
}

//g++ -std=c++17 alpha_beta_minimax.cpp opening_book.cpp evalf.cpp -o program