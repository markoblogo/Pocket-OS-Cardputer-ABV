#include "games_core.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace games {
namespace {
uint32_t roll(uint32_t& seed) { seed=seed*1664525U+1013904223U; return seed; }
bool due(uint32_t now,uint32_t at) { return static_cast<int32_t>(now-at)>=0; }
constexpr Cell shapes[7][4]={
    {{0,1},{1,1},{2,1},{3,1}},{{1,0},{2,0},{1,1},{2,1}},
    {{1,0},{0,1},{1,1},{2,1}},{{1,0},{2,0},{0,1},{1,1}},
    {{0,0},{1,0},{1,1},{2,1}},{{0,0},{0,1},{1,1},{2,1}},
    {{2,0},{0,1},{1,1},{2,1}}};
constexpr const char* map[Doom::height]={
    "############","#..........#","#.##.###...#","#....#.....#",
    "#....#..##.#","#........#.#","#..###.....#","#..........#",
    "############"};
}
Input portraitInput(Input key)
{
    switch(key) {
        case Input::Up:return Input::Right;
        case Input::Right:return Input::Down;
        case Input::Down:return Input::Left;
        case Input::Left:return Input::Up;
        default:return key;
    }
}
Cell Tetris::cell(int index,int turn,int n) const
{
    Cell c=shapes[index][n];
    if(index==1) return c;
    const int edge=index==0?3:2;
    for(int i=0;i<(turn&3);++i) { int nx=edge-c.y;c.y=c.x;c.x=nx; }
    return c;
}
bool Tetris::fits(int nx,int ny,int turn) const
{
    for(int i=0;i<4;++i) {
        Cell c=cell(piece,turn,i);c.x+=nx;c.y+=ny;
        if(c.x<0||c.x>=10||c.y>=20||(c.y>=0&&board[c.y][c.x])) return false;
    }
    return true;
}
int Tetris::ghost() const { int gy=y;while(fits(x,gy+1,rotation)) ++gy;return gy; }
int Tetris::takePiece()
{
    if(bag_pos==7) {
        for(int i=0;i<7;++i) bag[i]=i;
        for(int i=6;i>0;--i) std::swap(bag[i],bag[roll(random)%unsigned(i+1)]);
        bag_pos=0;
    }
    return bag[bag_pos++];
}
void Tetris::spawn()
{
    piece=next;next=takePiece();rotation=0;x=3;y=0;
    if(!fits(x,y,rotation)) over=true;
}
void Tetris::reset(uint32_t seed,uint32_t now)
{
    *this=Tetris{};random=seed;next=takePiece();spawn();deadline=now+650;
}
void Tetris::lock()
{
    for(int i=0;i<4;++i) {
        Cell c=cell(piece,rotation,i);c.x+=x;c.y+=y;
        if(c.y<0) { over=true;return; }
        board[c.y][c.x]=piece+1;
    }
    int cleared=0;
    for(int r=19;r>=0;--r) {
        bool full=true;for(int c=0;c<10;++c) if(!board[r][c]) full=false;
        if(!full) continue;
        for(int k=r;k>0;--k) std::copy(board[k-1],board[k-1]+10,board[k]);
        std::fill(board[0],board[0]+10,0);++cleared;++r;
    }
    constexpr int points[]={0,100,300,500,800};
    score+=points[cleared]*(1+lines/10);lines+=cleared;spawn();
}
void Tetris::input(Input key,uint32_t now)
{
    if(key==Input::Restart||(over&&key==Input::Ok)) { reset(now,now);return; }
    if(key==Input::Pause&&!over) { paused=!paused;deadline=now+650;return; }
    if(over||paused) return;
    if(key==Input::Left&&fits(x-1,y,rotation)) --x;
    else if(key==Input::Right&&fits(x+1,y,rotation)) ++x;
    else if(key==Input::Down&&fits(x,y+1,rotation)) { ++y;++score; }
    else if(key==Input::Up) {
        for(int kick : {0,-1,1,-2,2}) if(fits(x+kick,y,rotation+1)) { x+=kick;rotation=(rotation+1)&3;break; }
    } else if(key==Input::Ok) { int bottom=ghost();score+=2*(bottom-y);y=bottom;lock();deadline=now+650; }
}
bool Tetris::tick(uint32_t now)
{
    if(over||paused||!due(now,deadline)) return false;
    deadline=now+std::max(130,650-lines*20);
    if(fits(x,y+1,rotation)) ++y;else lock();
    return true;
}
int Klondike::firstFace(int slot) const
{
    if(slot<0||slot>=13) return 0;
    const auto& pile=piles[slot];
    for(size_t i=0;i<pile.size();++i) if(pile[i].face) return int(i);
    return int(pile.size());
}
void Klondike::reset(uint32_t seed)
{
    *this=Klondike{};
    std::array<int,52> deck{};for(int i=0;i<52;++i) deck[i]=i;
    for(int i=51;i>0;--i) std::swap(deck[i],deck[roll(seed)%unsigned(i+1)]);
    int n=0;
    for(int col=0;col<7;++col) for(int r=0;r<=col;++r) piles[col+6].push_back({deck[n++],r==col});
    while(n<52) piles[0].push_back({deck[n++],false});
}
bool Klondike::canMove(int source,int first,int destination) const
{
    if(source<1||source>=13||destination<2||destination>=13||source==destination) return false;
    const auto& from=piles[source];const auto& to=piles[destination];
    if(first<0||first>=int(from.size())||!from[first].face) return false;
    if(source<6&&first!=int(from.size())-1) return false;
    for(size_t i=first+1;i<from.size();++i)
        if(!from[i].face||red(from[i].id)==red(from[i-1].id)||rank(from[i].id)+1!=rank(from[i-1].id)) return false;
    const int id=from[first].id;
    if(destination<6) return first==int(from.size())-1&&suit(id)==destination-2&&rank(id)==int(to.size());
    if(to.empty()) return rank(id)==12;
    return to.back().face&&red(id)!=red(to.back().id)&&rank(id)+1==rank(to.back().id);
}
bool Klondike::move(int source,int first,int destination)
{
    if(!canMove(source,first,destination)) { notice="NO MOVE";return false; }
    auto& from=piles[source];auto& to=piles[destination];
    to.insert(to.end(),from.begin()+first,from.end());from.erase(from.begin()+first,from.end());
    if(source>=6&&!from.empty()) from.back().face=true;
    ++moves;selected=-1;notice="ENTER SELECT";won=true;
    for(int i=2;i<6;++i) if(piles[i].size()!=13) won=false;
    row=std::max(0,int(piles[cursor].size())-1);return true;
}
void Klondike::input(Input key,uint32_t now)
{
    if(key==Input::Restart||(won&&key==Input::Ok)) { reset(now);return; }
    if(won) return;
    if(key==Input::Left||key==Input::Right) {
        int first=cursor>=6?6:0,count=cursor>=6?7:6;
        cursor=first+(cursor-first+(key==Input::Right?1:count-1))%count;
        row=std::max(0,int(piles[cursor].size())-1);
    } else if(key==Input::Up) {
        if(cursor>=6&&row>firstFace(cursor)) --row;
        else if(cursor>=6) { cursor=std::min(5,cursor-6);row=0; }
    } else if(key==Input::Down) {
        if(cursor<6) { constexpr int cols[]={6,7,9,10,11,12};cursor=cols[cursor];row=std::max(0,int(piles[cursor].size())-1); }
        else row=std::min(std::max(0,int(piles[cursor].size())-1),row+1);
    } else if(key==Input::Ok) {
        if(cursor==0) {
            selected=-1;
            if(!piles[0].empty()) { Card c=piles[0].back();piles[0].pop_back();c.face=true;piles[1].push_back(c); }
            else while(!piles[1].empty()) { Card c=piles[1].back();piles[1].pop_back();c.face=false;piles[0].push_back(c); }
            ++moves;notice="ENTER SELECT";
        } else if(selected==cursor) { selected=-1;notice="CANCELLED"; }
        else if(selected>=0) move(selected,selected_row,cursor);
        else if(!piles[cursor].empty()) {
            row=std::clamp(row,firstFace(cursor),int(piles[cursor].size())-1);
            if(piles[cursor][row].face) { selected=cursor;selected_row=cursor>=6?row:int(piles[cursor].size())-1;notice="CHOOSE DESTINATION"; }
        }
    }
}
bool Doom::wall(float px,float py)
{
    return px<0||py<0||px>=width||py>=height||map[int(py)][int(px)]=='#';
}
float Doom::wrap(float a) { while(a>3.141593f) a-=6.283186f;while(a<-3.141593f) a+=6.283186f;return a; }
float Doom::ray(float a,bool* side,float* texture) const
{
    const float dx=std::cos(a),dy=std::sin(a);
    const float ddx=std::abs(dx)>0.0001f?std::abs(1/dx):10000;
    const float ddy=std::abs(dy)>0.0001f?std::abs(1/dy):10000;
    int mx=int(x),my=int(y),sx=dx<0?-1:1,sy=dy<0?-1:1;
    float tx=(dx<0?x-mx:mx+1-x)*ddx,ty=(dy<0?y-my:my+1-y)*ddy,distance=20;
    bool hit_side=false;
    for(int i=0;i<32;++i) {
        if(tx<ty) { distance=tx;tx+=ddx;mx+=sx;hit_side=false; }
        else { distance=ty;ty+=ddy;my+=sy;hit_side=true; }
        if(wall(mx+0.5f,my+0.5f)) break;
    }
    if(side) *side=hit_side;
    if(texture) { float hit=hit_side?x+distance*dx:y+distance*dy;*texture=hit-std::floor(hit); }
    return distance;
}
bool Doom::visible(float px,float py) const { return ray(std::atan2(py-y,px-x))+0.05f>=std::hypot(px-x,py-y); }
void Doom::reset(uint32_t now)
{
    *this=Doom{};frame=now;fire_until=now;
    enemies={{{4.5f,1.5f,2,now+1500},{9.5f,3.5f,2,now+1500},{7.5f,7.5f,2,now+1500}}};
}
void Doom::input(Input key,uint32_t now)
{
    if(key==Input::Restart||((over||won)&&key==Input::Ok)) { reset(now);return; }
    if(over||won) return;
    if(key==Input::Left||key==Input::Right) angle=wrap(angle+(key==Input::Left?-0.14f:0.14f));
    else if(key==Input::Up||key==Input::Down) {
        float speed=key==Input::Up?0.16f:-0.12f;
        float nx=x+std::cos(angle)*speed,ny=y+std::sin(angle)*speed;
        auto clear=[](float px,float py) { return !wall(px-.18f,py-.18f)&&!wall(px+.18f,py-.18f)&&!wall(px-.18f,py+.18f)&&!wall(px+.18f,py+.18f); };
        if(clear(nx,y)) x=nx;
        if(clear(x,ny)) y=ny;
    } else if(key==Input::Ok&&ammo>0&&due(now,fire_until)) {
        --ammo;fire_until=now+280;
        Enemy* target=nullptr;float nearest=20;
        for(auto& enemy:enemies) {
            float distance=std::hypot(enemy.x-x,enemy.y-y);
            if(enemy.hp>0&&distance<nearest&&std::abs(wrap(std::atan2(enemy.y-y,enemy.x-x)-angle))<std::atan2(.30f,distance)&&visible(enemy.x,enemy.y)) { target=&enemy;nearest=distance; }
        }
        if(target&&--target->hp==0) ++kills;
    }
    if(ammo_box&&std::hypot(x-6.5f,y-5.5f)<.6f) { ammo+=12;ammo_box=false; }
    if(medkit&&health<100&&std::hypot(x-3.5f,y-5.5f)<.6f) { health=std::min(100,health+40);medkit=false; }
    if(kills==3&&std::hypot(x-10.5f,y-7.5f)<.6f) won=true;
}
bool Doom::tick(uint32_t now)
{
    if(over||won||now-frame<100) return false;
    frame=now;
    for(auto& e:enemies) {
        if(e.hp<=0) continue;
        float distance=std::hypot(e.x-x,e.y-y);
        if(distance>6||!visible(e.x,e.y)) continue;
        if(distance>1) {
            float nx=e.x+(x-e.x)*.035f/distance,ny=e.y+(y-e.y)*.035f/distance;
            if(!wall(nx,e.y)) e.x=nx;
            if(!wall(e.x,ny)) e.y=ny;
        } else if(due(now,e.attack_at)) { health=std::max(0,health-10);e.attack_at=now+1000; }
    }
    if(health==0) over=true;
    return true;
}
}
