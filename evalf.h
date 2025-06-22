#ifndef EVALF_H
#define EVALF_H
#include<vector>
#include<string>
#include "chess-library-master/include/chess.hpp"
using namespace std;

// extern const vector<double> FEATURE_WEIGHT;
// extern const double BIAS = 2

const vector<double> FEATURE_WEIGHT{
    -40.343589311255158, 40.3811407084486616, 50.49857910192702, -50.762379259869275,
    2.5889107325902274, -2.305535870320396, -8.208679242560931, -9.838366565312626,
    6.22056719745971, 17.11429346545257, 20.443913709789335, -36.05289867993929,
    -22.920567066503345, -63.9885562924157, -138.5660647579206, -19.468501211302772,
    34.47859678128173, 21.75637674077167, 64.30254714446693, 143.56200775393935,
    -12.712298544226343, -12.101292032446521, 21.791521121985625, 13.17168021251228,
    12.999380293447555, -21.31898316213979, 0, 0,
    0, 0, 0, 0,
    0, 0,0,0,
    0, 0, 0, 0,
    0, 0, 0,0,
    -0,0
};
// const vector<double> FEATURE_WEIGHT{
//     -4.343589311255158, 4.3811407084486616, 5.49857910192702, -0.5762379259869275,
//     2.5889107325902274, -2.305535870320396, -8.208679242560931, -9.838366565312626,
//     6.22056719745971, 17.11429346545257, 20.443913709789335, -36.05289867993929,
//     -22.920567066503345, -63.9885562924157, -138.5660647579206, -19.468501211302772,
//     34.47859678128173, 21.75637674077167, 64.30254714446693, 143.56200775393935,
//     -12.712298544226343, -12.101292032446521, 21.791521121985625, 13.17168021251228,
//     12.999380293447555, -21.31898316213979, 20.021294411217085, 27.60688664281168,
//     17.180146213340517, -32.37101063002427, -20.766244311967103, -15.247242455157197,
//     -7.883634047139672, 6.601941497514384, -6.6019521000348815, -3.8614315821281013,
//     0.9881404388043697, 9.515176500878427, -4.666352252701163, -7.883634047139672,
//     -13.033937591823191, 13.033927000608706, 8.107431008323847, -7.300867581071819,
//     -9.35143088399921, 8.034594217109216
// };

const double BIAS = 54.82869884;

//PSQT MAPS
constexpr int pawn_psqt[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10,-20,-20, 10, 10,  5,  
    5, -5,-10,  0,  0,-10, -5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5,  5, 10, 25, 25, 10,  5,  5,
   10, 10, 20, 30, 30, 20, 10, 10,
   50, 50, 50, 50, 50, 50, 50, 50,
    0,  0,  0,  0,  0,  0,  0,  0
};

constexpr int knight_psqt[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

constexpr int bishop_psqt[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};


constexpr int rook_psqt[64] = {
    0,  0,  0,  5,  5,  0,  0,  0,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
   -5,  0,  0,  0,  0,  0,  0, -5,
    5, 10, 10, 10, 10, 10, 10,  5,
    0,  0,  0,  5,  5,  0,  0,  0
};

constexpr int queen_psqt[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};




// STRUCTURES DEFINATIONS 

struct Material_non_psqt
{
    int white = 0;
    int black = 0;
};


struct Material_psqt
{
    int white = 0;
    int black = 0;
};


struct MaterialImbalances {
    
    int white_pawns = 0;
    int white_knights = 0;
    int white_bishops = 0;
    int white_rooks = 0;
    int white_queens = 0;

    int black_pawns = 0;
    int black_knights = 0;
    int black_bishops = 0;
    int black_rooks = 0;
    int black_queens = 0;
};


struct Mobility
{
    int white = 0;
    int black = 0;
};


struct UnDevelopmentFeatures {
    
    int white_knights_undeveloped = 0;
    int white_bishops_undeveloped = 0;

    int black_knights_undeveloped = 0;
    int black_bishops_undeveloped = 0;
};

struct PawnFeatures {
   
    int white_doubled_pawns = 0;
    int white_isolated_pawns = 0;
    int white_passed_pawns = 0;

    int black_doubled_pawns = 0;
    int black_isolated_pawns = 0;
    int black_passed_pawns = 0;
};

struct RookFeatures {
    int white_rook_on_7th_rank = 0; 
    int white_rook_open_file = 0;  
    int white_rook_semi_open_file = 0;
    
    int black_rook_open_file = 0;   
    int black_rook_on_7th_rank = 0;   
    int black_rook_semi_open_file = 0;
};

struct KingFeatures {
    int game_phase = 0;
    bool castled = false;
    bool can_castle = false;
    int opponent_attacks_near_king = 0;
    int defence_near_king = 0;
    int pawns_in_king_zone = 0;
    int open_files_near_king = 0;
};

struct KingFeaturesPair {
    KingFeatures white;
    KingFeatures black;
};

class Evalf
{
    public:

    static Material_non_psqt get_material_non_psqt(chess::Board &board);
    
    static Material_psqt get_material_psqt(chess :: Board & board, Material_non_psqt material_non_psqt);

    static Mobility get_mobility(chess :: Board & board);

    static MaterialImbalances get_material_imbalances(const chess::Board& board);

    static UnDevelopmentFeatures get_undevelopment_features(const chess::Board& board);

    static PawnFeatures get_pawn_features(const chess::Board& board);

    static RookFeatures get_rook_features(const chess::Board& board);

    static KingFeatures get_king_features(const chess::Board& board, chess::Color color);

    static KingFeaturesPair get_king_features_pair(const chess::Board& board);

    static std::vector<int> get_feature_vector(chess::Board board);

    static double get_evaluation(string fen);

    static string convert_fen(string fen);

};





#endif