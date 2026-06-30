#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace fs = std::filesystem;

enum class Screen { Launcher, MusicList, MusicPlaying, RecorderList, Message };
enum class Volume { Mute, Mid, Loud };

Screen screen = Screen::Launcher;
int launcher = 0;
std::vector<std::string> music;
std::vector<std::string> recordings;
int music_sel = 0;
int rec_sel = 0;
bool playing = false;
bool shuffle_on = false;
Volume volume = Volume::Mid;
std::string title, body;

std::string lowerExt(const fs::path& p) {
    std::string e = p.extension().string();
    std::transform(e.begin(), e.end(), e.begin(), [](unsigned char c){ return std::tolower(c); });
    return e;
}

void scan(const char* folder, std::vector<std::string>& out, const std::vector<std::string>& exts) {
    out.clear();
    fs::path dir = fs::path("sim_sd") / folder;
    if (!fs::exists(dir)) return;
    for (auto& e : fs::directory_iterator(dir)) {
        if (!e.is_regular_file()) continue;
        std::string name = e.path().filename().string();
        if (name.empty() || name[0] == '.') continue;
        std::string ext = lowerExt(e.path());
        if (std::find(exts.begin(), exts.end(), ext) != exts.end()) out.push_back(name);
    }
    std::sort(out.begin(), out.end());
}

const char* volName() {
    if (volume == Volume::Mute) return "MUTE";
    if (volume == Volume::Loud) return "LOUD";
    return "MID";
}

void clear() { std::cout << "\033[2J\033[H"; }

void draw() {
    clear();
    if (screen == Screen::Launcher) {
        const char* labels[] = {"[#] MUSIC", "[=] READER", "[+] NOTES", "[o] RECORD", "[~] TIME", "[*] TOOLS"};
        std::cout << "ABVx SIM\n\n";
        for (int i = 0; i < 6; ++i) std::cout << (i == launcher ? "> " : "  ") << labels[i] << "\n";
        std::cout << "\nw/s move  e open  g music  q quit\n";
    } else if (screen == Screen::MusicList) {
        std::cout << "MUSIC " << (music.empty()?0:music_sel+1) << "/" << music.size() << "\n";
        if (music.empty()) std::cout << "No MP3 files in sim/sim_sd/music\n";
        for (int i = 0; i < (int)music.size(); ++i) std::cout << (i == music_sel ? "> " : "  ") << music[i] << "\n";
        std::cout << "\ne/g play  1 shuffle:" << (shuffle_on?"ON":"OFF") << "  q back\n";
    } else if (screen == Screen::MusicPlaying) {
        std::cout << "PLAYING\n" << (music.empty()?"":music[music_sel]) << "\n\n";
        std::cout << "VOL:" << volName() << " SHUF:" << (shuffle_on?"ON":"OFF") << "\n";
        std::cout << "~~~~__/-\\__/\\___/--\\_~~~~\n";
        std::cout << "\ne/g stop  a/d track  w/s vol  1 shuffle\n";
    } else if (screen == Screen::RecorderList) {
        std::cout << "RECORDER " << (recordings.empty()?0:rec_sel+1) << "/" << recordings.size() << "\n/sdcard/recordings\n";
        if (recordings.empty()) std::cout << "No WAV/PCM files; record/play in v0.2\n";
        for (int i = 0; i < (int)recordings.size(); ++i) std::cout << (i == rec_sel ? "> " : "  ") << recordings[i] << "\n";
        std::cout << "\nq/g back\n";
    } else {
        std::cout << title << "\n" << body << "\n\ne/q/g back\n";
    }
}

void nextTrack(int delta) {
    if (music.empty()) return;
    if (shuffle_on && music.size() > 1) music_sel = std::random_device{}() % music.size();
    else music_sel = (music_sel + delta + music.size()) % music.size();
}

bool handle(char c) {
    if (c == 'x') return false;
    if (screen == Screen::Launcher) {
        if (c == 'w') launcher = std::max(0, launcher - 1);
        else if (c == 's') launcher = std::min(5, launcher + 1);
        else if (c == 'g') { launcher = 0; scan("music", music, {".mp3"}); screen = Screen::MusicList; }
        else if (c == 'e') {
            if (launcher == 0) { scan("music", music, {".mp3"}); screen = Screen::MusicList; }
            else if (launcher == 3) { scan("recordings", recordings, {".wav", ".pcm"}); screen = Screen::RecorderList; }
            else { title = "Coming soon"; body = "Music now; Recorder v0.2"; screen = Screen::Message; }
        }
    } else if (screen == Screen::MusicList) {
        if (c == 'w' && !music.empty()) music_sel = std::max(0, music_sel - 1);
        else if (c == 's' && !music.empty()) music_sel = std::min((int)music.size() - 1, music_sel + 1);
        else if (c == '1') shuffle_on = !shuffle_on;
        else if ((c == 'e' || c == 'g') && !music.empty()) { playing = true; screen = Screen::MusicPlaying; }
        else if (c == 'q') screen = Screen::Launcher;
    } else if (screen == Screen::MusicPlaying) {
        if (c == 'e' || c == 'g' || c == 'q') { playing = false; screen = Screen::MusicList; }
        else if (c == 'a') nextTrack(-1);
        else if (c == 'd') nextTrack(1);
        else if (c == 'w') volume = volume == Volume::Loud ? Volume::Loud : (Volume)((int)volume + 1);
        else if (c == 's') volume = volume == Volume::Mute ? Volume::Mute : (Volume)((int)volume - 1);
        else if (c == '1') shuffle_on = !shuffle_on;
    } else if (screen == Screen::RecorderList) {
        if (c == 'w' && !recordings.empty()) rec_sel = std::max(0, rec_sel - 1);
        else if (c == 's' && !recordings.empty()) rec_sel = std::min((int)recordings.size() - 1, rec_sel + 1);
        else if (c == 'e') { title = "Recorder"; body = "Record/play in v0.2"; screen = Screen::Message; }
        else if (c == 'q' || c == 'g') screen = Screen::Launcher;
    } else if (c == 'e' || c == 'q' || c == 'g') screen = Screen::Launcher;
    return true;
}

int main() {
    while (true) {
        draw();
        std::cout << "key> " << std::flush;
        char c = 0;
        if (!(std::cin >> c)) break;
        if (!handle(c)) break;
    }
}
