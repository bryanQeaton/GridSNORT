#ifndef GRIDSNORT_SOLVER_H
#define GRIDSNORT_SOLVER_H
#include <algorithm>

#include "snort.h"
#include "tt.h"

inline uint64_t nodes=0;

inline void score_moves(const Snort::Position &pos,Snort::Movelist &moves,const uint64_t &best_move) {
    for (auto move:moves) {
        if (move==best_move){move.set_score(32767);}
        const uint64_t adj=Snort::adj[__builtin_ctzll(move)];
        const uint64_t them_owned_adj=adj&pos.owned(!pos.turn());
        move.set_score(move.score()+__builtin_popcountll(adj)-__builtin_popcountll(them_owned_adj));
    }
}


inline int solver(Snort::Position &pos,int alph=-1,const int beta=1) {
    nodes++;
    const int solve=pos.solve();
    if (solve!=0) {
        return solve;
    }
    const uint64_t hash=pos.hash();
    Entry &entry=tt[hash];
    const int alph_orig=alph;
    uint64_t best_move=0;
    if (entry.hash==hash) {
        if (entry.flag==0){return entry.value;}
        if (entry.flag==-1&&entry.value>=beta){return entry.value;}
        if (entry.flag==1&&entry.value<=alph){return entry.value;}
        best_move=entry.best_move;
    }
    Snort::Movelist moves;
    pos.legal_moves(moves);
    if (moves.size()==0) {
        return -1;
    }
    score_moves(pos,moves,best_move);
    std::ranges::sort(moves,[](Snort::Movelist::Move &a,Snort::Movelist::Move &b){return a.score()>b.score();});
    //don't move in your own territory unless thats the only choice
    const uint64_t legals=pos.legals(pos.turn());
    const uint64_t owned=pos.owned(pos.turn())&legals;
    const uint64_t neutral=legals&~owned;
    int value=-1;
    for (int m=0;m<moves.size();m++) {
        if (neutral&&(moves[m]&owned)){continue;}

        pos.make_move(moves[m]);
        value=std::max(value,-solver(pos,-beta,-alph));
        pos.undo_move(moves[m]);
        if (value>alph) {
            best_move=moves[m];
            if (value>=beta) {
                break;
            }
            alph=value;
        }
    }
    entry.value=value;
    entry.hash=hash;
    entry.best_move=best_move;
    if (value<alph_orig){entry.flag=1;}
    else if (value>=beta){entry.flag=-1;}
    else{entry.flag=0;}
    return value;
}



#endif //GRIDSNORT_SOLVER_H
