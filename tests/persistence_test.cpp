#include "persistence.h"

#include <cassert>
#include <cstdio>
#include <string>
#include <unistd.h>
#include <vector>

int main()
{
    const std::string base = "/tmp/abvx-inbox-" + std::to_string(getpid());
    const std::string log = base + ".log";
    const std::string tmp = base + ".tmp";
    std::remove(log.c_str());
    std::remove(tmp.c_str());

    std::vector<std::string> events;
    std::string err;
    assert(abvx::loadEventLog(log.c_str(), &events, &err));
    assert(events.empty());

    const std::vector<std::string> first = {
        "S1|READ|BOOK1.TXT",
        "S2|VOICE|REC00001.WAV",
    };
    assert(abvx::mergeEventLog(log.c_str(), tmp.c_str(), first, &events, &err));
    assert(events == first);

    events.clear();
    assert(abvx::loadEventLog(log.c_str(), &events, &err));
    assert(events == first);

    std::vector<std::string> overflow;
    for (size_t i = 0; i < abvx::kEventLogMaxEntries + 8; ++i) {
        overflow.push_back("S" + std::to_string(i + 3) + "|TEST|event");
    }
    assert(abvx::mergeEventLog(log.c_str(), tmp.c_str(), overflow, &events, &err));
    assert(events.size() == abvx::kEventLogMaxEntries);
    assert(events.back() == overflow.back());

    std::remove(log.c_str());
    std::remove(tmp.c_str());
    return 0;
}
