#include "games_app.h"
#include <cassert>
#include <iostream>
#include <string>
int main(int argc,char** argv)
{
    LGFX_Sprite canvas;
    const games::Palette palette={0x0862,0xDEFB,0x7C51,0xFFE0};
    std::string output=argc>1?argv[1]:"/tmp";
    auto render=[&](const char* name) {
        games::draw(canvas,palette);
        for(const auto& text:canvas.overflow) std::cerr<<name<<": clipped text: "<<text<<"\n";
        assert(canvas.overflow.empty());canvas.save(output+"/"+name+".svg");
    };
    games::enter(42);render("games-menu");
    games::handleKey(games::Input::Ok,100);assert(games::portrait());
    canvas.resize(135,240);
    for(int i=0;i<4;++i) {
        for(int j=0;j<3;++j) games::handleKey(i%2?games::Input::Down:games::Input::Up,200+i*100);
        games::handleKey(games::Input::Ok,250+i*100);
    }
    render("games-tetris");
    games::handleKey(games::Input::Pause,800);render("games-tetris-paused");
    games::handleKey(games::Input::Home,900);assert(!games::portrait());canvas.resize(240,135);
    games::handleKey(games::Input::Down,1000);games::handleKey(games::Input::Ok,1100);render("games-klondike");
    // Navigate every slot and a deep scroll window without clipping captions.
    for(int i=0;i<100;++i) {
        games::handleKey(games::Input::Right,1200+i*10);
        games::handleKey(i%2?games::Input::Up:games::Input::Down,1201+i*10);
        games::handleKey(games::Input::Ok,1202+i*10);
        games::draw(canvas,palette);assert(canvas.overflow.empty());
    }
    games::handleKey(games::Input::Home,3000);games::handleKey(games::Input::Down,3100);
    games::handleKey(games::Input::Ok,3200);render("games-doom-lite");
    games::handleKey(games::Input::Home,3300);assert(games::handleKey(games::Input::Home,3400));
    const games::Palette light={0xFFFF,0,0x4208,0x07E0};games::draw(canvas,light);assert(canvas.overflow.empty());
    std::cout<<"games rendering: PASS (portrait, landscape, menu navigation, text bounds)\n";
}
