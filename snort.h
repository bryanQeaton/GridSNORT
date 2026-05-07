#ifndef GRIDSNORT_SNORT_H
#define GRIDSNORT_SNORT_H
#include <cstdint>
#include <iostream>
#include <ostream>
#include <random>

namespace Snort {
    inline std::mt19937 gen(std::random_device{}());
    static uint64_t adj[64]{};
    static uint64_t zobrist[129]{};
    constexpr uint64_t BOARD_MASK[73] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, // indices 0..8 unused
        0x1,                                   // 1x1
        0x3,                                   // 1x2
        0x7,                                   // 1x3
        0xf,                                   // 1x4
        0x1f,                                  // 1x5
        0x3f,                                  // 1x6
        0x7f,                                  // 1x7
        0xff,                                  // 1x8
        0x101,                                 // 2x1
        0x303,                                 // 2x2
        0x707,                                 // 2x3
        0xf0f,                                 // 2x4
        0x1f1f,                                // 2x5
        0x3f3f,                                // 2x6
        0x7f7f,                                // 2x7
        0xffff,                                // 2x8
        0x10101,                               // 3x1
        0x30303,                               // 3x2
        0x70707,                               // 3x3
        0xf0f0f,                               // 3x4
        0x1f1f1f,                              // 3x5
        0x3f3f3f,                              // 3x6
        0x7f7f7f,                              // 3x7
        0xffffff,                              // 3x8
        0x1010101,                             // 4x1
        0x3030303,                             // 4x2
        0x7070707,                             // 4x3
        0xf0f0f0f,                             // 4x4
        0x1f1f1f1f,                            // 4x5
        0x3f3f3f3f,                            // 4x6
        0x7f7f7f7f,                            // 4x7
        0xffffffff,                            // 4x8
        0x101010101,                           // 5x1
        0x303030303,                           // 5x2
        0x707070707,                           // 5x3
        0xf0f0f0f0f,                           // 5x4
        0x1f1f1f1f1f,                          // 5x5
        0x3f3f3f3f3f,                          // 5x6
        0x7f7f7f7f7f,                          // 5x7
        0xffffffffff,                          // 5x8
        0x10101010101,                         // 6x1
        0x30303030303,                         // 6x2
        0x70707070707,                         // 6x3
        0xf0f0f0f0f0f,                         // 6x4
        0x1f1f1f1f1f1f,                        // 6x5
        0x3f3f3f3f3f3f,                        // 6x6
        0x7f7f7f7f7f7f,                        // 6x7
        0xffffffffffff,                        // 6x8
        0x1010101010101,                       // 7x1
        0x3030303030303,                       // 7x2
        0x7070707070707,                       // 7x3
        0xf0f0f0f0f0f0f,                       // 7x4
        0x1f1f1f1f1f1f1f,                      // 7x5
        0x3f3f3f3f3f3f3f,                      // 7x6
        0x7f7f7f7f7f7f7f,                      // 7x7
        0xffffffffffffff,                      // 7x8
        0x101010101010101,                     // 8x1
        0x303030303030303,                     // 8x2
        0x707070707070707,                     // 8x3
        0xf0f0f0f0f0f0f0f,                     // 8x4
        0x1f1f1f1f1f1f1f1f,                    // 8x5
        0x3f3f3f3f3f3f3f3f,                    // 8x6
        0x7f7f7f7f7f7f7f7f,                    // 8x7
        0xffffffffffffffff                     // 8x8
    };
    constexpr uint64_t EAST_BOUND=0xfefefefefefefefe;
    constexpr uint64_t WEST_BOUND=0x7f7f7f7f7f7f7f7f;
    static uint64_t adjacent(const uint64_t x) {
        uint64_t o=x>>1&WEST_BOUND;
        o|=x<<1&EAST_BOUND;
        o|=x>>8;
        o|=x<<8;
        return o&~x;
    }
    inline void show(const uint64_t x,const int m,const int n) {
        for (int i=0;i<64;i++) {
            if (i%8==0){std::cout<<"\n";}
            if (1ull<<i&~BOARD_MASK[m*8+n]){continue;}

            if (1ull<<i&x){std::cout<<"1";}
            else{std::cout<<"0";}
        }
        std::cout<<"\n";
    }
    class Movelist {
    public:
        class Move {
            uint64_t move{};
            int16_t scor{};
        public:
            explicit Move(const uint64_t move):move(move){}
            Move()=default;
            int16_t score() const {return scor;}
            void set_score(const int16_t value){scor=value;}
            operator unsigned long long() const {return move;}
        };
    private:
        size_t siz=0;
        Move movelist[64]{};
    public:
        [[nodiscard]] size_t size() const {return siz;}
        [[nodiscard]] auto begin() const {return movelist;}
        [[nodiscard]] auto begin() {return movelist;}
        [[nodiscard]] auto end() const{return movelist+siz;}
        [[nodiscard]] auto end(){return movelist+siz;}
        auto &operator[](const int idx) const {return movelist[idx];}
        auto &operator[](const int idx) {return movelist[idx];}
        void add(const Move move) {
            movelist[siz]=move;
            siz++;
        }
        void pop() {
            siz--;
        }
        void clear(){siz=0;}
    };
    class Position {
        uint64_t board[2]{};
        bool stm{};
        uint64_t h=0;
        uint8_t m=8,n=8;
        uint8_t pl=0;
        static void _gen_adj() {
            for (int i=0;i<64;i++) {
                const uint64_t sqr=1ull<<i;
                adj[i]|=sqr>>8;
                adj[i]|=sqr<<8;
                adj[i]|=sqr<<1&EAST_BOUND;
                adj[i]|=sqr>>1&WEST_BOUND;
            }
        }
        static void _gen_zob() {
            std::uniform_int_distribution<uint64_t> dist(pow(2,60),pow(2,64)-1);
            for (int i=0;i<129;i++) {zobrist[i]=dist(gen);}
        }
    public:
        explicit Position(const uint8_t m=8,const uint8_t n=8):m(m),n(n) {
            if (!adj[0]){_gen_adj();}
            if (!zobrist[0]){_gen_zob();}
            h=zobrist[128];
        }
        void legal_moves(Movelist &movelist) const {
            const uint64_t occ=board[0]|board[1];
            uint64_t owned=board[!stm]>>1&WEST_BOUND;
            owned|=board[!stm]<<1&EAST_BOUND;
            owned|=board[!stm]>>8;
            owned|=board[!stm]<<8;
            uint64_t legals=~occ&~owned&BOARD_MASK[m*8+n];
            while (legals) {
                const uint64_t move=legals&-legals;
                legals&=(legals-1);
                movelist.add(Movelist::Move(move));
            }
        }
        void make_move(const Movelist::Move move) {
            board[stm]|=move;
            int idx=__builtin_ctzll(move);
            idx+=stm?64:0;
            h^=zobrist[idx];
            h^=zobrist[128];
            stm=!stm;
            pl++;
        }
        void undo_move(const Movelist::Move move) {
            stm=!stm;
            int idx=__builtin_ctzll(move);
            idx+=stm?64:0;
            h^=zobrist[idx];
            h^=zobrist[128];
            board[stm]^=move;
            pl--;
        }
        [[nodiscard]] uint64_t generate_hash() const {
            uint64_t ha=stm?0:zobrist[128];
            auto temp=board[0];
            while (temp) {
                const int idx=__builtin_ctzll(temp);
                temp&=(temp-1);
                ha^=zobrist[idx];
            }
            temp=board[1];
            while (temp) {
                const int idx=__builtin_ctzll(temp)+64;
                temp&=(temp-1);
                ha^=zobrist[idx];
            }
            return ha;
        }
        [[nodiscard]] uint64_t hash() const {return h;}
        [[nodiscard]] uint64_t all() const {return board[0]|board[1];}
        [[nodiscard]] uint64_t occ(const bool side) const {return board[side];}
        [[nodiscard]] uint64_t owned(const bool side) const {
            uint64_t o=board[side]>>1&WEST_BOUND;
            o|=board[side]<<1&EAST_BOUND;
            o|=board[side]>>8;
            o|=board[side]<<8;
            return o&~board[side];
        }
        [[nodiscard]] uint64_t empty() const{return ~all()&BOARD_MASK[m*8+n];}
        [[nodiscard]] bool turn() const{return stm;}
        [[nodiscard]] uint8_t ply() const {return pl;}
        uint64_t legals(const bool side) const {
            const uint64_t empty_sqrs=empty();
            uint64_t owned=board[!side]>>1&WEST_BOUND;
            owned|=board[!side]<<1&EAST_BOUND;
            owned|=board[!side]>>8;
            owned|=board[!side]<<8;
            return empty()&~owned;
        }
        [[nodiscard]] int solve() const {
            const uint64_t us_legals=legals(turn());
            const uint64_t them_legals=legals(!turn());
            const uint64_t us_adj=adjacent(us_legals);
            const uint64_t them_adj=adjacent(them_legals);
            const uint64_t intersection=(us_adj|us_legals)&(them_adj|them_legals);
            const int count=__builtin_popcountll(intersection);
            if (count>2){return 0;}
            const int us_count=__builtin_popcountll(us_legals);
            const int them_count=__builtin_popcountll(them_legals);
            int score=us_count-them_count;
            if (count==1){score+=1;}
            else if (count==2) {
                auto temp=us_legals;
                bool adj_intesect=false;
                while (temp) {
                    const int idx=__builtin_ctzll(temp);
                    temp&=(temp-1);
                    if ((adj[idx]&intersection)==intersection) {
                        //if they're adjacent you can't cover both in one move so the score stays the same
                        adj_intesect=true;
                        break;
                    }
                }
                if (!adj_intesect) {
                    //if theyre not adjacent you can cover both in one move so the score is supplimented
                    score+=1;
                }
            }
            if (score<=0){return -1;}
            return 1;
        }
        friend std::ostream &operator<<(std::ostream &os,const Position &pos) {
            for (int i=0;i<64;i++) {
                if (i%8==0){os<<"\n";}
                if (1ull<<i&~BOARD_MASK[pos.m*8+pos.n]){continue;}
                if (1ull<<i&pos.board[0]){os<<"X ";}
                else if (1ull<<i&pos.board[1]){os<<"O ";}
                else {os<<"_ ";}
            }
            return os;
        }
    };


}




#endif //GRIDSNORT_SNORT_H
