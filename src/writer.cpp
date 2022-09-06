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

writer::writer(const string &path):
path(path), diritems(0)
{
    if (!filesystem::exists(path)){
        exit(EXIT_FAILURE);
        cout << "Path " << path << " does not exist." << endl;

    }
    for (auto const& dir_entry : filesystem::directory_iterator{path}) {
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

void writer::writeHeader(){
    ofile << "ID,Record,Raspberry_Time,GPS_Time,Altitude,Latitude,Longitude,Temperature,TemperatureRaw,RelHumidity,RelHumidityRaw,VapourPressure,VapourPressureRaw,BlackBulbTemp,Velocity\n";
    ofile.flush();
}