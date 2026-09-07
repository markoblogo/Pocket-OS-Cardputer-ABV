#include "gnss_service.h"
#include <cassert>
#include <cstdio>
#include <string>

void sentence(GnssService& gnss, const std::string& body, uint32_t now)
{
    unsigned checksum=0; for(char c:body) checksum^=static_cast<unsigned char>(c);
    char data[256]; snprintf(data,sizeof(data),"$%s*%02X",body.c_str(),checksum);
    gnss.parseSentence(data,now);
}
int main()
{
    GnssService gnss; gnss.begin();
    sentence(gnss,"GPRMC,120000,A,4807.038,N,01131.000,E,1.0",100);
    assert(gnss.status(100)==GnssStatus::Fix);
    sentence(gnss,"GPGSV,1,1,08",4000);
    assert(gnss.status(4000)!=GnssStatus::Fix);
    gnss.poll(4000); assert(!gnss.fix().valid);
    sentence(gnss,"GPGGA,120000,4807.038,N,01131.000,E,1,08,1.0,12.0",5000);
    assert(gnss.fix().valid);
    sentence(gnss,"GPGGA,120000,,,,,0,00,1.0,12.0",5100);
    assert(!gnss.fix().valid);
    sentence(gnss,"GPRMC,120000,A,nan,N,01131.000,E,1.0",5200);
    assert(!gnss.fix().valid);
    sentence(gnss,"GPRMC,120000,A,9001.000,N,01131.000,E,1.0",5300);
    assert(!gnss.fix().valid);
    puts("GNSS freshness: OK");
}
