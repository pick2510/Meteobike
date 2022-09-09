#include "writer.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <sstream>
using namespace std;

writer::~writer()
{
    ofile.close();
}

writer::writer(const string &path, const string &hostname, const string &ip) : path(path), hostname(hostname), ip(ip), diritems(0), record("")
{
    counter = 0;
    if (!filesystem::exists(path))
    {
        exit(EXIT_FAILURE);
        cout << "Path " << path << " does not exist." << endl;
    }
    for (auto const &dir_entry : filesystem::directory_iterator{path})
    {
        diritems++;
    }
    cout << diritems << " directory items in " << path << endl;
    stringstream path_s;
    path_s << path << "/" << std::setfill('0') << std::setw(4) << diritems << "_meteobike.csv";
    cout << path_s.str() << endl;
    ofile.open(path_s.str());
    if (!ofile.is_open())
    {
        cout << "File " << path_s.str() << " could not be opened." << endl;
        exit(EXIT_FAILURE);
    }
    writeHeader();
}

void writer::writeHeader()
{
    ofile << "Hostname,count,GPS_Time,Altitude,Latitude,Longitude,Temperature,RelHumidity,VapourPressure,WetBulbTemp,Heatindex\n";
    ofile.flush();
}

void writer::createRecord(const results &mymeas)
{
    record.str("");
    record << hostname << ","
           << counter << ","
           << mymeas.gps.time << ","
           << mymeas.gps.altitude << ","
           << mymeas.gps.latitude << ","
           << mymeas.gps.longitude << ","
           << mymeas.dht.temperature << ","
           << mymeas.dht.humdidity << ","
           << mymeas.pv << ","
           << mymeas.wbtemp << ","
           << mymeas.heatindex;
    counter++;
}


void writer::writeRecord(){
    ofile << record.str() << "\n";
    ofile.flush();
}
