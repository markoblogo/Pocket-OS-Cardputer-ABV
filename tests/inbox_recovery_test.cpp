#include "persistence.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <unistd.h>
int main(){
    const std::string p="/tmp/abvx-recovery-"+std::to_string(getpid());
    std::vector<std::string> out;
    assert(abvx::mergeEventLog(p.c_str(),(p+".NEW").c_str(),{"first"},&out));
    assert(rename(p.c_str(),(p+".BAK").c_str())==0);
    // Simulates interrupted publication; uncommitted partial temp is ignored.
    FILE* f=fopen((p+".NEW").c_str(),"w"); fputs("partial",f); fclose(f);
    assert(abvx::loadEventLog(p.c_str(),&out)); assert(out.size()==1 && out[0]=="first");
    assert(abvx::mergeEventLog(p.c_str(),(p+".NEW").c_str(),{"second"},&out));
    assert(out.size()==2);
    remove(p.c_str());remove((p+".NEW").c_str());remove((p+".BAK").c_str());
}
