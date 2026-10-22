#pragma once
#include <array>
#include <cstdint>
#include <vector>

namespace games {
enum class Input { Other, Up, Down, Left, Right, Ok, Back, Home, Restart, Pause };
Input portraitInput(Input key);

struct Cell { int x, y; };
struct Tetris {
    uint8_t board[20][10] = {};
    int piece=0, next=0, rotation=0, x=3, y=0, score=0, lines=0;
    bool over=false, paused=false;
    uint32_t deadline=0, random=1;
    std::array<int,7> bag{};
    int bag_pos=7;
    void reset(uint32_t seed, uint32_t now);
    Cell cell(int index, int turn, int n) const;
    bool fits(int nx, int ny, int turn) const;
    int ghost() const;
    int takePiece();
    void spawn();
    void lock();
    void input(Input key, uint32_t now);
    bool tick(uint32_t now);
};

struct Card { int id; bool face; };
struct Klondike {
    // stock, waste, C/D/H/S foundations, seven tableau columns.
    std::array<std::vector<Card>,13> piles;
    int cursor=6, row=0, selected=-1, selected_row=0, moves=0;
    bool won=false;
    const char* notice="ENTER SELECT";
    static int rank(int id) { return id%13; }
    static int suit(int id) { return id/13; }
    static bool red(int id) { return suit(id)==1 || suit(id)==2; }
    int firstFace(int slot) const;
    void reset(uint32_t seed);
    bool canMove(int source, int first, int destination) const;
    bool move(int source, int first, int destination);
    void input(Input key, uint32_t now);
};

struct Enemy { float x,y; int hp; uint32_t attack_at; };
struct Doom {
    static constexpr int width=12, height=9;
    float x=1.5f,y=1.5f,angle=0;
    int health=100,ammo=24,kills=0;
    bool over=false,won=false,medkit=true,ammo_box=true;
    uint32_t frame=0,fire_until=0;
    std::array<Enemy,3> enemies{};
    static bool wall(float x,float y);
    static float wrap(float angle);
    float ray(float angle, bool* side=nullptr, float* texture=nullptr) const;
    bool visible(float target_x,float target_y) const;
    void reset(uint32_t now);
    void input(Input key,uint32_t now);
    bool tick(uint32_t now);
};
}
