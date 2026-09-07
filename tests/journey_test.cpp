#include "journey_service.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <unistd.h>
int main(){
    std::string root="/tmp/abvx-journey-"+std::to_string(getpid());
    JourneyService journey(root.c_str());
    assert(journey.start(100));
    GnssFix fix; fix.valid=true;fix.latitude=48;fix.longitude=2;fix.last_fix_ms=100;
    journey.appendIfDue(fix,100); assert(journey.pointCount()==1);
    journey.appendIfDue(fix,6000); assert(journey.pointCount()==1);
    assert(journey.status()==JourneyStatus::WaitingForFix);
    fix.last_fix_ms=7000;journey.appendIfDue(fix,7000); assert(journey.pointCount()==2);
    fix.latitude=49;fix.last_fix_ms=13000;journey.appendIfDue(fix,13000);
    assert(journey.pointCount()==2);
    journey.stop(15000);assert(journey.status()==JourneyStatus::Saved);
    assert(journey.elapsedSeconds(30000)==14);
    remove((root+"/J0001/TRACK.CSV").c_str());rmdir((root+"/J0001").c_str());rmdir(root.c_str());
}
