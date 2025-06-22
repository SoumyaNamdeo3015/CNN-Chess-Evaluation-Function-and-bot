#include "evalf.h"
#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<cctype>

using namespace std;



Material_non_psqt Evalf:: get_material_non_psqt(chess::Board & board)
{
    Material_non_psqt material_feature;
    constexpr int piece_values[6] = {100, 300, 330, 500,900, 10000};
    for(int sq = 0; sq < 64; sq++)
    {
        chess::Piece piece = board.at(chess::Square(sq));
        if(piece == chess::Piece::NONE) continue;
        chess :: PieceType piece_type = piece.type();
        if(piece_type == chess :: PieceType :: KING) continue;
        chess :: Color color = piece.color();
        if (color == chess::Color::WHITE) {
            material_feature.white += piece_values[static_cast<int>(piece_type)];
        } 
        else 
        {
            material_feature.black -= piece_values[static_cast<int>(piece_type)];
        }
    }
    return material_feature;
}

Material_psqt Evalf :: get_material_psqt(chess :: Board & board, Material_non_psqt material_non_psqt)
{
    int white = material_non_psqt.white;
    int black = material_non_psqt.black;

    for(int sq = 0; sq < 64; sq++)
    {
        chess::Piece piece = board.at(chess::Square(sq));
        if(piece == chess::Piece::NONE) continue;
        chess :: PieceType piece_type = piece.type();
        if(piece_type == chess :: PieceType :: KING) continue;
        chess :: Color color = piece.color();


        if(piece_type == chess :: PieceType :: PAWN)
        {   
            if(color == chess::Color::WHITE)
            {
                white += pawn_psqt[sq];
            }
            else
            {
                int rank = sq/8;
                int file = sq%8;
                black -= pawn_psqt[(7 - rank)*8 + file];
            }
        }

        else if(piece_type == chess :: PieceType :: KNIGHT)
        {

            if(color == chess :: Color :: WHITE)
            {
                white += knight_psqt[sq];
            }
            else
            {
                int rank = sq/8;
                int file = sq%8;
                black -= knight_psqt[(7 - rank)*8 + file];
            }

        }

        else if(piece_type == chess :: PieceType :: BISHOP)
        {

            if(color == chess :: Color :: WHITE)
            {
                white += bishop_psqt[sq];
            }
            else
            {
                int rank = sq/8;
                int file = sq%8;
                black -= bishop_psqt[(7 - rank)*8 + file];
            }

        }

        else if(piece_type == chess :: PieceType :: ROOK)
        {

            if(color == chess :: Color :: WHITE)
            {
                white += rook_psqt[sq];
            }
            else
            {
                int rank = sq/8;
                int file = sq%8;
                black -= rook_psqt[(7 - rank)*8 + file];
            }

        }


        else if(piece_type == chess :: PieceType :: QUEEN)
        {

            if(color == chess :: Color :: WHITE)
            {
                white += queen_psqt[sq];
            }
            else
            {
                int rank = sq/8;
                int file = sq%8;
                black -= queen_psqt[(7 - rank)*8 + file];
            }

        }
        
    }
    
    Material_psqt material_psqt;
    material_psqt.white = white;
    material_psqt.black = black;
    return material_psqt;

}


Mobility Evalf :: get_mobility(chess :: Board & board)
{
    Mobility mobility_feature;
    chess :: Movelist white_moves;
    chess :: movegen :: legalmoves(white_moves, board);
    mobility_feature.white += white_moves.size();

    board.makeNullMove();
    chess :: Movelist black_moves;
    chess :: movegen :: legalmoves(black_moves, board);
    mobility_feature.black += black_moves.size();
    board.unmakeNullMove();

    return mobility_feature;
}

MaterialImbalances Evalf :: get_material_imbalances(const chess::Board& board) {
    MaterialImbalances imbalances;

    // White pieces
    imbalances.white_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE).count();
    imbalances.white_knights = board.pieces(chess::PieceType::KNIGHT, chess::Color::WHITE).count();
    imbalances.white_bishops = board.pieces(chess::PieceType::BISHOP, chess::Color::WHITE).count();
    imbalances.white_rooks = board.pieces(chess::PieceType::ROOK, chess::Color::WHITE).count();
    imbalances.white_queens = board.pieces(chess::PieceType::QUEEN, chess::Color::WHITE).count();
    // Black pieces

    imbalances.black_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK).count();
    imbalances.black_knights = board.pieces(chess::PieceType::KNIGHT, chess::Color::BLACK).count();
    imbalances.black_bishops = board.pieces(chess::PieceType::BISHOP, chess::Color::BLACK).count();
    imbalances.black_rooks = board.pieces(chess::PieceType::ROOK, chess::Color::BLACK).count();
    imbalances.black_queens = board.pieces(chess::PieceType::QUEEN, chess::Color::BLACK).count();

    return imbalances;
}


UnDevelopmentFeatures Evalf :: get_undevelopment_features(const chess::Board& board) {
    UnDevelopmentFeatures features;

    constexpr chess::Square white_knight1_start = chess::Square::SQ_B1;
    constexpr chess::Square white_knight2_start = chess::Square::SQ_G1;
    constexpr chess::Square white_bishop1_start = chess::Square::SQ_C1;
    constexpr chess::Square white_bishop2_start = chess::Square::SQ_F1;

    constexpr chess::Square black_knight1_start = chess::Square::SQ_B8;
    constexpr chess::Square black_knight2_start = chess::Square::SQ_G8;
    constexpr chess::Square black_bishop1_start = chess::Square::SQ_C8;
    constexpr chess::Square black_bishop2_start = chess::Square::SQ_F8;

    
    if (board.at(white_knight1_start) == chess::Piece(chess::PieceType::KNIGHT, chess::Color::WHITE))
        features.white_knights_undeveloped++;
    if (board.at(white_knight2_start) == chess::Piece(chess::PieceType::KNIGHT, chess::Color::WHITE))
        features.white_knights_undeveloped++;

    if (board.at(black_knight1_start) == chess::Piece(chess::PieceType::KNIGHT, chess::Color::BLACK))
        features.black_knights_undeveloped++;
    if (board.at(black_knight2_start) == chess::Piece(chess::PieceType::KNIGHT, chess::Color::BLACK))
        features.black_knights_undeveloped++;

    
    if (board.at(white_bishop1_start) == chess::Piece(chess::PieceType::BISHOP, chess::Color::WHITE))
        features.white_bishops_undeveloped++;
    if (board.at(white_bishop2_start) == chess::Piece(chess::PieceType::BISHOP, chess::Color::WHITE))
        features.white_bishops_undeveloped++;

    if (board.at(black_bishop1_start) == chess::Piece(chess::PieceType::BISHOP, chess::Color::BLACK))
        features.black_bishops_undeveloped++;
    if (board.at(black_bishop2_start) == chess::Piece(chess::PieceType::BISHOP, chess::Color::BLACK))
        features.black_bishops_undeveloped++;

    return features;
}



PawnFeatures Evalf:: get_pawn_features(const chess::Board& board) {
    PawnFeatures features;

    chess::Bitboard white_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
    chess::Bitboard black_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

    std::array<int, 8> white_pawns_per_file = {0};
    std::array<int, 8> black_pawns_per_file = {0};

    //file count vector
    for (int file = 0; file < 8; file++) {
        chess::Bitboard file_mask = chess::Bitboard(chess::File(file));
        white_pawns_per_file[file] = (white_pawns & file_mask).count();
        black_pawns_per_file[file] = (black_pawns & file_mask).count();
    }

    // double pawns
    for (int file = 0; file < 8; file++) {
        if (white_pawns_per_file[file] > 1) features.white_doubled_pawns += white_pawns_per_file[file] - 1;
        if (black_pawns_per_file[file] > 1) features.black_doubled_pawns += black_pawns_per_file[file] - 1;
    }

    //isolated
    for (int file = 0; file < 8; file++) {
        bool white_isolated = true;
        bool black_isolated = true;

        if (file > 0) {
            white_isolated &= (white_pawns_per_file[file - 1] == 0);
            black_isolated &= (black_pawns_per_file[file - 1] == 0);
        }
        if (file < 7) {
            white_isolated &= (white_pawns_per_file[file + 1] == 0);
            black_isolated &= (black_pawns_per_file[file + 1] == 0);
        }

        if (white_isolated && white_pawns_per_file[file] > 0) features.white_isolated_pawns++;
        if (black_isolated && black_pawns_per_file[file] > 0) features.black_isolated_pawns++;
    }

    // Passed pawn feature

    //white
    for (int file = 0; file < 8; file++) {
        chess::Bitboard file_mask = chess::Bitboard(chess::File(file));

        chess::Bitboard white_pawns_in_file = white_pawns & file_mask;
        while (white_pawns_in_file) {
            chess::Square pawn_sq = white_pawns_in_file.pop();
            chess::Rank pawn_rank = pawn_sq.rank();

            bool is_passed = true;

            if ((black_pawns & file_mask).count() > 0) {
                is_passed = false;
            }
            if (file > 0) {
                chess::Bitboard left_file_mask = chess::Bitboard(chess::File(file - 1));
                chess::Bitboard left_file_pawns = black_pawns & left_file_mask;
                if (left_file_pawns.count() > 0) {
                    chess::Square left_pawn_sq = left_file_pawns.lsb();
                    if (left_pawn_sq.rank() > pawn_rank) {
                        is_passed = false;
                    }
                }
            }
            if (file < 7) {
                chess::Bitboard right_file_mask = chess::Bitboard(chess::File(file + 1));
                chess::Bitboard right_file_pawns = black_pawns & right_file_mask;
                if (right_file_pawns.count() > 0) {
                    chess::Square right_pawn_sq = right_file_pawns.lsb();
                    if (right_pawn_sq.rank() > pawn_rank) {
                        is_passed = false;
                    }
                }
            }

            if (is_passed) features.white_passed_pawns++;
        }

        
        //black
        chess::Bitboard black_pawns_in_file = black_pawns & file_mask;
        while (black_pawns_in_file) {
            chess::Square pawn_sq = black_pawns_in_file.pop();
            chess::Rank pawn_rank = pawn_sq.rank();

            bool is_passed = true;

    
            if ((white_pawns & file_mask).count() > 0) {
                is_passed = false;
            }

            if (file > 0) {
                chess::Bitboard left_file_mask = chess::Bitboard(chess::File(file - 1));
                chess::Bitboard left_file_pawns = white_pawns & left_file_mask;
                if (left_file_pawns.count() > 0) {
                    chess::Square left_pawn_sq = left_file_pawns.lsb();
                    if (left_pawn_sq.rank() < pawn_rank) {
                        is_passed = false;
                    }
                }
            }
            if (file < 7) {
                chess::Bitboard right_file_mask = chess::Bitboard(chess::File(file + 1));
                chess::Bitboard right_file_pawns = white_pawns & right_file_mask;
                if (right_file_pawns.count() > 0) {
                    chess::Square right_pawn_sq = right_file_pawns.lsb();
                    if (right_pawn_sq.rank() < pawn_rank) {
                        is_passed = false;
                    }
                }
            }

            if (is_passed) features.black_passed_pawns++;
        }
    }
    return features;
}



RookFeatures Evalf:: get_rook_features(const chess::Board& board) {
    RookFeatures features;
    chess::Bitboard white_rooks = board.pieces(chess::PieceType::ROOK, chess::Color::WHITE);
    chess::Bitboard black_rooks = board.pieces(chess::PieceType::ROOK, chess::Color::BLACK);

    chess::Bitboard white_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
    chess::Bitboard black_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

    ///7th rank
    chess::Bitboard white_7th_rank = chess::Bitboard(chess::Rank::RANK_7); 
    chess::Bitboard black_7th_rank = chess::Bitboard(chess::Rank::RANK_2); 

    features.white_rook_on_7th_rank = (white_rooks & white_7th_rank).count();
    features.black_rook_on_7th_rank = (black_rooks & black_7th_rank).count();

    //open and semi-open files
    for (int file = 0; file < 8; file++) {
        chess::Bitboard file_mask = chess::Bitboard(chess::File(file));

        bool white_pawns_on_file = (white_pawns & file_mask).count() > 0;
        bool black_pawns_on_file = (black_pawns & file_mask).count() > 0;

        if (!white_pawns_on_file && !black_pawns_on_file) {
            features.white_rook_open_file += (white_rooks & file_mask).count();
            features.black_rook_open_file += (black_rooks & file_mask).count();
        }
        else if (!white_pawns_on_file && black_pawns_on_file) {
            features.white_rook_semi_open_file += (white_rooks & file_mask).count();
        }
        else if (!black_pawns_on_file && white_pawns_on_file) {
            features.black_rook_semi_open_file += (black_rooks & file_mask).count();
        }
    }

    return features;
}


KingFeatures Evalf:: get_king_features(const chess::Board& board, chess::Color color) {
    KingFeatures features;

    int total_material = board.pieces(chess::PieceType::QUEEN).count() * 900 +
                         board.pieces(chess::PieceType::ROOK).count() * 500 +
                         board.pieces(chess::PieceType::BISHOP).count() * 300 +
                         board.pieces(chess::PieceType::KNIGHT).count() * 300;
    features.game_phase = (total_material <= 1150) ? 1 : 0;

    features.castled = !board.castlingRights().has(color);
    features.can_castle = board.castlingRights().has(color);

    chess::Square king_sq = board.kingSq(color);

    chess::Bitboard king_zone = chess::attacks::king(king_sq);


    for (int sq = 0; sq < 64; ++sq) {
        
        if (king_zone.check(sq)) {  
        
            if (board.isAttacked(sq, ~color)) {
               features.opponent_attacks_near_king++;

            }
    
            if (board.isAttacked(sq, color)) {
                features.defence_near_king++;
            }
        }
    }
    

    chess::Bitboard pawns_in_king_zone = board.pieces(chess::PieceType::PAWN, color) & king_zone;
    features.pawns_in_king_zone = pawns_in_king_zone.count();
    
    chess::File king_file = king_sq.file();
    for (int file_offset = -1; file_offset <= 1; file_offset++) {
        int file = king_file + file_offset;
        if (file >= 0 && file < 8) {
            chess::Bitboard file_mask = chess::Bitboard(chess::File(file));
            bool is_open = (board.pieces(chess::PieceType::PAWN, color) & file_mask).count() == 0;
            if (is_open) features.open_files_near_king++;
        }
    }

    return features;
}


KingFeaturesPair Evalf :: get_king_features_pair(const chess::Board& board) {
    KingFeaturesPair features_pair;
    features_pair.white = Evalf :: get_king_features(board, chess::Color::WHITE);
    features_pair.black = Evalf :: get_king_features(board, chess::Color::BLACK);
    return features_pair;
}


std :: vector<int> Evalf:: get_feature_vector(chess::Board board)
{
    std :: vector<int> v;
    
    Material_non_psqt material_non_psqt = Evalf::get_material_non_psqt(board);
    v.push_back(material_non_psqt.white);
    v.push_back(-material_non_psqt.black);
    
    Material_psqt material_psqt = Evalf::get_material_psqt(board, material_non_psqt);
    v.push_back(material_psqt.white);
    v.push_back(-material_psqt.black);

    Mobility mobility = Evalf:: get_mobility(board);
    v.push_back(mobility.white);
    v.push_back(mobility.black);
    
    UnDevelopmentFeatures ud = Evalf:: get_undevelopment_features(board);
    v.push_back(ud.white_bishops_undeveloped);
    v.push_back(ud.white_knights_undeveloped);
    v.push_back(ud.black_bishops_undeveloped);
    v.push_back(ud.black_knights_undeveloped);


    MaterialImbalances material_imbalances = Evalf:: get_material_imbalances(board);
    v.push_back(material_imbalances.white_pawns);
    v.push_back(material_imbalances.white_knights);
    v.push_back(material_imbalances.white_bishops);
    v.push_back(material_imbalances.white_rooks);
    v.push_back(material_imbalances.white_queens);

    v.push_back(material_imbalances.black_pawns);
    v.push_back(material_imbalances.black_knights);
    v.push_back(material_imbalances.black_bishops);
    v.push_back(material_imbalances.black_rooks);
    v.push_back(material_imbalances.black_queens);




    PawnFeatures pawn_features = Evalf:: get_pawn_features(board);
    v.push_back(pawn_features.white_doubled_pawns);
    v.push_back(pawn_features.white_isolated_pawns);
    v.push_back(pawn_features.white_passed_pawns);

    v.push_back(pawn_features.black_doubled_pawns);
    v.push_back(pawn_features.black_isolated_pawns);
    v.push_back(pawn_features.black_passed_pawns);


    RookFeatures rook_features = Evalf:: get_rook_features(board);
    v.push_back(rook_features.white_rook_on_7th_rank);
    v.push_back(rook_features.white_rook_open_file);
    v.push_back(rook_features.white_rook_semi_open_file);

    v.push_back(rook_features.black_rook_on_7th_rank);
    v.push_back(rook_features.black_rook_open_file);
    v.push_back(rook_features.black_rook_semi_open_file);



    KingFeatures king_features = Evalf:: get_king_features(board, chess::Color::WHITE);
    v.push_back(king_features.game_phase);
    v.push_back(king_features.castled ? 1 : 0);
    v.push_back(king_features.can_castle ? 1 : 0);
    v.push_back(king_features.opponent_attacks_near_king);
    v.push_back(king_features.defence_near_king);
    v.push_back(king_features.pawns_in_king_zone);
    v.push_back(king_features.open_files_near_king);

    KingFeatures king_features1 = Evalf::get_king_features(board, chess::Color::BLACK);

    v.push_back(king_features1.game_phase);
    v.push_back(king_features1.castled ? 1 : 0);
    v.push_back(king_features1.can_castle ? 1 : 0);
    v.push_back(king_features1.opponent_attacks_near_king);
    v.push_back(king_features1.defence_near_king);
    v.push_back(king_features1.pawns_in_king_zone);
    v.push_back(king_features1.open_files_near_king);


    return v;
}

string Evalf :: convert_fen(string fen)
{
    vector<string> ls;
    stringstream ss(fen);
    string segment;

    // Split FEN into parts
    while (getline(ss, segment, ' ')) {
        ls.push_back(segment);
    }

    // If active color is white, return the original FEN
    if (ls[1] == "w") {
        return fen;
    }

    // Reverse the board part of the FEN
    string fen_str = ls[0];
    reverse(fen_str.begin(), fen_str.end());
    string new_fen_str = "";

    // Convert piece letters and keep digits and slashes
    for (char ch : fen_str) {
        if (ch == '/' || isdigit(ch)) {
            new_fen_str += ch;
        } else if (islower(ch)) {
            new_fen_str += toupper(ch);
        } else {
            new_fen_str += tolower(ch);
        }
    }

    // Reverse each rank back
    vector<string> new_fen_ls;
    stringstream ss2(new_fen_str);
    while (getline(ss2, segment, '/')) {
        reverse(segment.begin(), segment.end());
        new_fen_ls.push_back(segment);
    }

    new_fen_str = "";
    for (size_t i = 0; i < new_fen_ls.size(); ++i) {
        new_fen_str += new_fen_ls[i];
        if (i < new_fen_ls.size() - 1) {
            new_fen_str += '/';
        }
    }

    // Handle castling rights
    string cas_str = ls[2];
    string new_cas_str = "";
    for (char ch : cas_str) {
        if (ch == '-') {
            new_cas_str += ch;
        } else if (islower(ch)) {
            new_cas_str += toupper(ch);
        } else {
            new_cas_str += tolower(ch);
        }
    }

    string cas_str1 = "";
    if (new_cas_str == "-") {
        cas_str1 = "-";
    } else {
        if (new_cas_str.find('K') != string::npos) cas_str1 += 'K';
        if (new_cas_str.find('Q') != string::npos) cas_str1 += 'Q';
        if (new_cas_str.find('k') != string::npos) cas_str1 += 'k';
        if (new_cas_str.find('q') != string::npos) cas_str1 += 'q';
    }

    // Handle en passant square
    string ep = ls[3];
    if (ep != "-") {
        int c2 = ep[1] - '0' - 1;
        c2 = 8 - c2;
        ep = ep.substr(0, 1) + to_string(c2);
    }

    // Reconstruct FEN
    vector<string> new_ls = {new_fen_str, "w", cas_str1, ep, ls[4], ls[5]};
    string new_fen = "";
    for (size_t i = 0; i < new_ls.size(); ++i) {
        new_fen += new_ls[i];
        if (i < new_ls.size() - 1) {
            new_fen += " ";
        }
    }

    return new_fen;
}

//relative evaluation main fuction 
double Evalf :: get_evaluation(string fen)
{   
    double factor = 1;
    string new_fen = Evalf :: convert_fen(fen);
    
    if(new_fen != fen)
    {
        factor = -1;
    }

    chess :: Board board(new_fen);
    vector<int> features = get_feature_vector(board);
    double eval = BIAS;
    for(int i = 0; i < 46; i++)
    {
        double feature_value = static_cast<double>(features[i]);
        double weight = FEATURE_WEIGHT[i];
        eval += feature_value * weight;
    }   
    return eval*factor;

}


// int main()
// {
//     while(true)
//     {
//         string fen; cout << "enter fen :" << endl;
//         getline(cin, fen);
//         cout << "eval (relative) : " << Evalf :: get_evaluation(fen) << endl;

//     }
//     return 0;
// }