#include "games_app.h"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace games {
namespace {
enum class View { Menu, Tetris, Klondike, DoomLite };
View view=View::Menu;
Tetris blocks;
Klondike cards;
Doom arena;
int cursor=0;
uint32_t clock_ms=0;
constexpr uint16_t ink=0xDEFB, muted=0x7C51, black=0x0862, yellow=0xFFE0;
constexpr uint16_t pieces[]={0,0x07FF,0xFFE0,0xB81F,0x07E0,0xF800,0x051F,0xFD00};
uint16_t rgb(int r,int g,int b) { return uint16_t(((r>>3)<<11)|((g>>2)<<5)|(b>>3)); }
void text(LGFX_Sprite& c,int x,int y,const char* value,uint16_t color=ink,int size=1,uint16_t bg=black)
{
    c.setTextSize(size);c.setTextColor(color,bg);c.setCursor(x,y);c.print(value);
}
void drawMenu(LGFX_Sprite& c,const Palette& p)
{
    c.fillScreen(p.background);
    text(c,8,6,"GAMES",p.foreground,2,p.background);
    const char* names[]={"TETRIS","KLONDIKE","DOOM LITE"};
    const char* detail[]={"TURN CLOCKWISE / 10 x 20","DRAW ONE / MOVE RUNS","3 TARGETS / FIND THE EXIT"};
    for(int i=0;i<3;++i) {
        int y=32+i*28;
        if(i==cursor) c.fillRect(5,y,230,26,p.foreground);
        uint16_t fg=i==cursor?p.background:p.foreground,bg=i==cursor?p.foreground:p.background;
        text(c,10,y+2,names[i],fg,1,bg);text(c,10,y+14,detail[i],fg,1,bg);
    }
    text(c,8,122,"ARROWS SELECT  ENTER OPEN  GO BACK",p.dim,1,p.background);
}
void block(LGFX_Sprite& c,int x,int y,int color,bool ghost=false)
{
    if(ghost) c.drawRect(x,y,9,9,muted);
    else { c.fillRect(x,y,9,9,pieces[color]);c.drawFastHLine(x,y,9,ink); }
}
void drawTetris(LGFX_Sprite& c)
{
    c.fillScreen(black);
    text(c,5,3,"TETRIS",ink,2);
    c.setTextSize(1);c.setTextColor(ink,black);c.setCursor(5,22);
    c.printf("%05d L%02d",blocks.score,blocks.lines);
    // One 135x240 framebuffer, ten-pixel cells, full 10x20 playfield.
    c.drawRect(3,34,102,202,muted);
    for(int y=0;y<20;++y) for(int x=0;x<10;++x)
        if(blocks.board[y][x]) block(c,4+x*10,35+y*10,blocks.board[y][x]);
    if(!blocks.over) {
        int bottom=blocks.ghost();
        for(int i=0;i<4;++i) {
            Cell cell=blocks.cell(blocks.piece,blocks.rotation,i);
            if(bottom+cell.y>=0) block(c,4+(blocks.x+cell.x)*10,35+(bottom+cell.y)*10,0,true);
            if(blocks.y+cell.y>=0) block(c,4+(blocks.x+cell.x)*10,35+(blocks.y+cell.y)*10,blocks.piece+1);
        }
    }
    text(c,109,36,"NEXT",muted);
    for(int i=0;i<4;++i) {
        Cell cell=blocks.cell(blocks.next,0,i);
        c.fillRect(110+cell.x*5,49+cell.y*5,4,4,pieces[blocks.next+1]);
    }
    text(c,110,85,"P",yellow);text(c,110,96,"HOLD",muted);
    text(c,110,125,"R",yellow);text(c,110,136,"NEW",muted);
    text(c,110,165,"GO",yellow);text(c,110,176,"MENU",muted);
    if(blocks.over||blocks.paused) {
        c.fillRect(7,110,94,43,black);c.drawRect(7,110,94,43,yellow);
        text(c,13,118,blocks.over?"GAME OVER":"PAUSED",yellow);
        text(c,13,135,blocks.over?"ENTER NEW":"P RESUME",ink);
    }
}
void drawCard(LGFX_Sprite& c,int x,int y,const Card* card,bool focus,bool selected,int empty_suit=-1)
{
    constexpr uint16_t paper=0xEF9C;
    uint16_t bg=card&&card->face?paper:rgb(21,67,65);
    c.fillRect(x,y,31,19,bg);c.drawRect(x,y,31,19,focus?yellow:muted);
    if(selected) c.drawRect(x+1,y+1,29,17,yellow);
    if(card&&card->face) {
        char label[3]={"A23456789TJQK"[Klondike::rank(card->id)],"CDHS"[Klondike::suit(card->id)],0};
        text(c,x+4,y+2,label,Klondike::red(card->id)?rgb(180,20,30):black,1,bg);
    } else if(card) text(c,x+5,y+5,"///",muted,1,bg);
    else if(empty_suit>=0) { char label[2]={"CDHS"[empty_suit],0};text(c,x+12,y+5,label,muted,1,bg); }
}
void drawKlondike(LGFX_Sprite& c)
{
    c.fillScreen(black);text(c,4,3,"KLONDIKE",ink);
    c.setCursor(152,3);c.printf("MOVES %d",cards.moves);
    for(int slot=0;slot<6;++slot) {
        int x=slot<2?4+slot*34:104+(slot-2)*34;
        auto& pile=cards.piles[slot];
        drawCard(c,x,16,pile.empty()?nullptr:&pile.back(),cards.cursor==slot,cards.selected==slot,slot>=2?slot-2:-1);
        if(slot==0) { c.setTextColor(ink,rgb(21,67,65));c.setCursor(x+5,22);c.printf("%02d",int(pile.size())); }
    }
    for(int col=0;col<7;++col) {
        int slot=col+6,x=2+col*34;
        auto& pile=cards.piles[slot];int n=int(pile.size());
        int start=std::max(0,n-6);
        if(cards.cursor==slot) start=std::clamp(cards.row-4,0,std::max(0,n-6));
        c.setTextColor(muted,black);
        c.setCursor(x+2,37);c.printf("%d:%d",col+1,start);
        if(n==0) drawCard(c,x,47,nullptr,cards.cursor==slot,false);
        for(int r=start;r<std::min(n,start+6);++r) {
            drawCard(c,x,47+(r-start)*10,&pile[r],cards.cursor==slot&&cards.row==r,cards.selected==slot&&cards.selected_row==r);
        }
        // Cursor is always visible even when the card below overlaps it.
        if(cards.cursor==slot&&n>0) {
            int y=47+(cards.row-start)*10;
            c.drawRect(x,y,31,19,yellow);
            c.fillTriangle(x+24,y+2,x+28,y+2,x+26,y+5,yellow);
        }
    }
    text(c,3,118,cards.won?"YOU WIN! ENTER NEW":cards.notice,yellow);
    text(c,3,127,"ARROWS / ENTER   R NEW   GO MENU",muted);
}
void drawDoom(LGFX_Sprite& c)
{
    const int top=14,bottom=112,horizon=60;
    c.fillScreen(black);
    for(int y=top;y<bottom;++y) {
        int d=std::abs(y-horizon);
        c.drawFastHLine(0,y,240,y<horizon?rgb(19+d/3,26+d/3,39+d/2):rgb(35+d/2,29+d/3,24+d/4));
    }
    float depth[120];
    for(int ray=0;ray<120;++ray) {
        float offset=std::atan((float(ray)/119*2-1)*.66f),u=0;bool side=false;
        float distance=arena.ray(arena.angle+offset,&side,&u)*std::cos(offset);
        depth[ray]=distance;
        int height=std::min(180,int(85/std::max(.15f,distance))),y0=horizon-height/2;
        float light=1/(1+distance*.13f)*(side?.72f:1);
        int r=int(138*light),g=int(99*light),b=int(74*light);
        for(int y=std::max(top,y0);y<std::min(bottom,y0+height);++y) {
            int v=(y-y0)*64/height;
            bool mortar=v%16<2||int(u*64+(v/16%2)*16)%32<2;
            c.fillRect(ray*2,y,2,1,mortar?rgb(r/2,g/2,b/2):rgb(r,g,b));
        }
    }
    constexpr const char* monster[]={
        "...RRRRR...","..RrrrrrR..",".RrYrrrYrR.",".RrrrrrrrR.","..RrWWWrr..",
        "...RrrrR...","..BBBBBBB..",".BBbBBBbBB.","RBBbBBBbBBR","rr.BBBBB.rr",
        "...bbbbb...","..bbb.bbb..","..bbb.bbb..","..DDD.DDD.."};
    int order[]={0,1,2};
    std::sort(order,order+3,[](int a,int b) { return std::hypot(arena.enemies[a].x-arena.x,arena.enemies[a].y-arena.y)>std::hypot(arena.enemies[b].x-arena.x,arena.enemies[b].y-arena.y); });
    for(int i:order) {
        const auto& e=arena.enemies[i];if(e.hp<=0) continue;
        float angle=Doom::wrap(std::atan2(e.y-arena.y,e.x-arena.x)-arena.angle);
        if(std::abs(angle)>.9f) continue;
        float distance=std::hypot(e.x-arena.x,e.y-arena.y)*std::cos(angle);
        int scale=std::clamp(int(6/std::max(.2f,distance)),1,4);
        int ox=int(120+std::tan(angle)*120/.66f)-11*scale/2,oy=horizon-7*scale;
        for(int row=0;row<14;++row) for(int col=0;col<11;++col) {
            char pixel=monster[row][col];if(pixel=='.') continue;
            uint16_t color=pixel=='R'?rgb(202,63,39):pixel=='r'?rgb(135,38,27):pixel=='Y'?yellow:pixel=='W'?ink:pixel=='B'?rgb(103,114,65):pixel=='b'?rgb(62,76,41):rgb(38,29,24);
            for(int dx=0;dx<scale;++dx) {
                int x=ox+col*scale+dx,y=oy+row*scale;
                if(x>=0&&x<240&&distance<depth[x/2]&&y>=top&&y+scale<=bottom) c.drawFastVLine(x,y,scale,color);
            }
        }
    }
    c.drawFastHLine(116,60,9,ink);c.drawFastVLine(120,56,9,ink);
    c.fillRect(109,95,24,17,rgb(61,68,73));c.fillRect(116,82,10,21,muted);
    c.fillRect(119,81,4,12,ink);
    if(int32_t(arena.fire_until-clock_ms)>190) {
        c.fillTriangle(121,70,111,89,132,89,yellow);c.fillTriangle(121,73,116,88,127,88,0xFFFF);
    }
    c.fillRect(0,0,240,14,black);text(c,4,3,"DOOM LITE",ink);
    text(c,151,3,arena.kills==3?"EXIT OPEN":"EXIT LOCKED",arena.kills==3?0x07E0:muted);
    c.fillRect(199,17,38,29,black);
    for(int y=0;y<Doom::height;++y) for(int x=0;x<Doom::width;++x)
        if(Doom::wall(x+.5f,y+.5f)) c.fillRect(200+x*3,18+y*3,2,2,muted);
    c.fillRect(230,39,3,3,arena.kills==3?0x07E0:0x7800);
    for(const auto& e:arena.enemies) if(e.hp>0) c.fillRect(200+int(e.x)*3,18+int(e.y)*3,2,2,0xF800);
    c.fillRect(200+int(arena.x)*3,18+int(arena.y)*3,2,2,yellow);
    c.fillRect(0,112,240,23,black);c.setTextColor(ink,black);c.setTextSize(1);c.setCursor(4,114);
    c.printf("HP%03d AM%02d K%d/3",arena.health,arena.ammo,arena.kills);
    text(c,4,125,"ARROWS MOVE / TURN  ENTER FIRE  GO MENU",muted);
    if(arena.over||arena.won) {
        c.fillRect(30,46,180,39,black);c.drawRect(30,46,180,39,yellow);
        text(c,44,52,arena.won?"LEVEL CLEAR":"GAME OVER",yellow,2);
        text(c,48,73,"ENTER NEW  GO MENU",ink);
    }
}
}
void enter(uint32_t now) { clock_ms=now;cursor=0;view=View::Menu; }
bool portrait() { return view==View::Tetris; }
bool handleKey(Input key,uint32_t now)
{
    clock_ms=now;
    if(view==View::Menu) {
        if(key==Input::Home||key==Input::Back) return true;
        if(key==Input::Up) cursor=(cursor+2)%3;
        else if(key==Input::Down) cursor=(cursor+1)%3;
        else if(key==Input::Ok) {
            if(cursor==0) { blocks.reset(now,now);view=View::Tetris; }
            else if(cursor==1) { cards.reset(now);view=View::Klondike; }
            else { arena.reset(now);view=View::DoomLite; }
        }
    } else if(key==Input::Home||key==Input::Back) view=View::Menu;
    else if(view==View::Tetris) blocks.input(portraitInput(key),now);
    else if(view==View::Klondike) cards.input(key,now);
    else arena.input(key,now);
    return false;
}
bool update(uint32_t now)
{
    clock_ms=now;
    return view==View::Tetris?blocks.tick(now):view==View::DoomLite?arena.tick(now):false;
}
void draw(LGFX_Sprite& canvas,const Palette& palette)
{
    canvas.setTextDatum(0);
    if(view==View::Menu) drawMenu(canvas,palette);
    else if(view==View::Tetris) drawTetris(canvas);
    else if(view==View::Klondike) drawKlondike(canvas);
    else drawDoom(canvas);
    canvas.pushSprite(0,0);
}
}
