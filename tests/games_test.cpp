#include "games_core.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <set>

using namespace games;
int main()
{
    assert(portraitInput(Input::Left)==Input::Up);
    assert(portraitInput(Input::Up)==Input::Right);
    assert(portraitInput(Input::Ok)==Input::Ok);
    Tetris t;t.reset(42,0);
    std::set<int> bag={t.piece,t.next};
    for(int i=0;i<5;++i) bag.insert(t.takePiece());
    assert(bag.size()==7);
    t.reset(42,0);t.piece=0;t.rotation=0;t.x=3;t.y=18;
    for(int x=0;x<10;++x) t.board[19][x]=(x<3||x>6)?1:0;
    assert(t.fits(3,18,0));t.lock();assert(t.lines==1&&t.score==100);
    for(int x=0;x<10;++x) assert(t.board[19][x]==0);
    t.reset(8,0);int old_y=t.y;
    t.input(Input::Pause,1);assert(!t.tick(9000)&&t.y==old_y);
    t.input(Input::Pause,9000);assert(!t.tick(9001));assert(t.tick(9700));
    t.over=true;int old_x=t.x;t.input(Input::Left,9800);assert(t.x==old_x);
    t.reset(9,0xFFFFFF00U);assert(!t.tick(0xFFFFFF10U));assert(t.tick(500));
    // O rotates in place, unlike the original 4x4 translation.
    t.piece=1;for(int i=0;i<4;++i) assert(t.cell(1,0,i).x==t.cell(1,3,i).x);

    assert(!Klondike::red(0)&&Klondike::red(13)&&Klondike::red(26)&&!Klondike::red(39));
    Klondike k;k.reset(123);
    assert(k.piles[0].size()==24);
    for(int i=0;i<7;++i) assert(k.piles[i+6].size()==size_t(i+1));
    for(auto& p:k.piles) p.clear();
    k.piles[6]={{5,false},{12,true},{24,true},{10,true}}; // hidden, KC QD JC
    k.cursor=6;k.row=1;k.input(Input::Ok,1);
    assert(k.selected==6&&k.selected_row==1&&k.canMove(6,1,7));
    assert(!k.canMove(6,1,2)); // a run cannot move onto a foundation
    assert(k.move(6,1,7));assert(k.piles[7].size()==3);
    assert(k.piles[6].size()==1&&k.piles[6].back().face);
    k.piles[1]={{0,true}};assert(k.canMove(1,0,2));assert(!k.canMove(1,0,3));
    assert(k.move(1,0,2));
    k.reset(234);k.cursor=0;
    for(int i=0;i<24;++i) k.input(Input::Ok,i);
    int first=k.piles[1].front().id;k.input(Input::Ok,25);k.input(Input::Ok,26);
    assert(k.piles[1].back().id==first); // stock recycling preserves order

    Doom d;d.reset(0);assert(!Doom::wall(d.x,d.y));
    assert(std::abs(d.ray(0)-9.5f)<.001f);
    d.angle=3.14f;d.input(Input::Ok,1);assert(d.kills==0&&d.ammo==23);
    d.reset(0);d.input(Input::Ok,1);assert(d.enemies[0].hp==1&&d.kills==0);
    d.input(Input::Ok,100);assert(d.enemies[0].hp==1); // cooldown
    d.input(Input::Ok,400);assert(d.enemies[0].hp==0&&d.kills==1);
    d.reset(0);for(auto& e:d.enemies) e.hp=0;
    d.enemies[0]={2.5f,3.5f,2,1000};d.angle=std::atan2(2.f,1.f);
    d.input(Input::Ok,1);assert(d.enemies[0].hp==2); // no shooting through walls
    d.reset(0);for(auto& e:d.enemies) e.hp=0;
    assert(d.tick(10000)&&d.health==100); // no time-based health drain
    d.reset(0);for(auto& e:d.enemies) e.hp=0;
    d.enemies[0]={1.8f,1.5f,2,0};d.tick(100);assert(d.health==90);
    d.tick(200);assert(d.health==90);
    d.x=10.5f;d.y=7.5f;d.kills=3;d.input(Input::Up,300);assert(d.won);

    uint32_t random=12345;
    k.reset(random);t.reset(random,0);d.reset(0);
    for(uint32_t i=1;i<10000;++i) {
        random=random*1664525U+1013904223U;
        Input key=Input(1+(random>>16)%5);
        t.input(key,i*100);t.tick(i*100);
        if(!t.over) assert(t.fits(t.x,t.y,t.rotation));
        else t.input(Input::Ok,i*100);
        k.input(key,i*100);
        std::set<int> ids;int count=0;
        for(const auto& pile:k.piles) for(Card c:pile) { assert(c.id>=0&&c.id<52);ids.insert(c.id);++count; }
        assert(ids.size()==52&&count==52);
        d.input(key,i*100);d.tick(i*100);assert(!Doom::wall(d.x,d.y));
        assert(d.health>=0&&d.health<=100&&d.ammo>=0&&d.kills<=3);
    }
    std::cout<<"games rules: PASS (including 9,999 mixed input steps)\n";
}
