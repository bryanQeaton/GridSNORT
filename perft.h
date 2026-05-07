#ifndef GRIDSNORT_PERFT_H
#define GRIDSNORT_PERFT_H
#include <cstdint>

#include "snort.h"

inline uint64_t perft(Snort::Position &pos,const int depth) {
    if (depth<=0){return 1;}
    Snort::Movelist moves;
    pos.legal_moves(moves);
    if (depth==1){return moves.size();}
    if (moves.size()==0){return 1;}
    uint64_t nodes=0;
    for (const auto move:moves) {
        pos.make_move(move);
        nodes+=perft(pos,depth-1);
        pos.undo_move(move);
    }
    return nodes;
}


#endif //GRIDSNORT_PERFT_H
