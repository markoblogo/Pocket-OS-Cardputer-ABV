#pragma once
// Host drawing adapter: same renderer calls, approximate fixed 6x8 font metrics.
#include <algorithm>
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
class LGFX_Sprite {
    int w=240,h=135,x=0,y=0,size=1;
    uint16_t foreground=0xffff,background=0;
    std::ostringstream out;
    static std::string color(uint16_t value) {
        char s[8];snprintf(s,sizeof(s),"#%02x%02x%02x",((value>>11)&31)*255/31,((value>>5)&63)*255/63,(value&31)*255/31);return s;
    }
    static std::string escape(const std::string& value) {
        std::string s;for(char c:value) { if(c=='&')s+="&amp;";else if(c=='<')s+="&lt;";else if(c=='>')s+="&gt;";else s+=c; }return s;
    }
public:
    std::vector<std::string> overflow;
    void resize(int width,int height) {w=width;h=height;}
    void fillScreen(uint16_t c) {out.str("");out.clear();overflow.clear();fillRect(0,0,w,h,c);}
    void fillRect(int px,int py,int width,int height,uint16_t c) {out<<"<rect x='"<<px<<"' y='"<<py<<"' width='"<<width<<"' height='"<<height<<"' fill='"<<color(c)<<"'/>\n";}
    void drawRect(int px,int py,int width,int height,uint16_t c) {out<<"<rect x='"<<px+.5<<"' y='"<<py+.5<<"' width='"<<width-1<<"' height='"<<height-1<<"' fill='none' stroke='"<<color(c)<<"'/>\n";}
    void drawFastHLine(int px,int py,int width,uint16_t c) {fillRect(px,py,width,1,c);}
    void drawFastVLine(int px,int py,int height,uint16_t c) {fillRect(px,py,1,height,c);}
    void fillTriangle(int ax,int ay,int bx,int by,int cx,int cy,uint16_t c) {out<<"<polygon points='"<<ax<<","<<ay<<" "<<bx<<","<<by<<" "<<cx<<","<<cy<<"' fill='"<<color(c)<<"'/>\n";}
    void setTextSize(int s) {size=s;}
    void setTextColor(uint16_t fg,uint16_t bg) {foreground=fg;background=bg;}
    void setTextDatum(int) {}
    void setCursor(int px,int py) {x=px;y=py;}
    void print(const char* value) {
        const std::string s(value);int width=int(s.size())*6*size;
        if(x<0||y<0||x+width>w||y+8*size>h) overflow.push_back(s);
        fillRect(x,y,width,8*size,background);
        out<<"<text x='"<<x<<"' y='"<<y+7*size<<"' font-family='monospace' font-size='"<<8*size<<"' textLength='"<<width<<"' lengthAdjust='spacingAndGlyphs' fill='"<<color(foreground)<<"'>"<<escape(s)<<"</text>\n";
        x+=width;
    }
    void printf(const char* format,...) {char s[256];va_list a;va_start(a,format);vsnprintf(s,sizeof(s),format,a);va_end(a);print(s);}
    void pushSprite(int,int) {}
    void save(const std::string& path) {std::ofstream f(path);f<<"<svg xmlns='http://www.w3.org/2000/svg' width='"<<w<<"' height='"<<h<<"' viewBox='0 0 "<<w<<" "<<h<<"'><title>Pocket OS software render; not a device photograph</title>"<<out.str()<<"</svg>\n";}
};
