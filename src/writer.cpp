#include "writer.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <sstream>
using namespace std;


writer::~writer()
{
}

writer::writer(string path):
path(path), diritems(0)
{
    if (!filesystem::exists(path)){
        exit(1);
        cout << "Path " << path << " does not exist." << endl;

    }
    for (auto const& dir_entry : filesystem::directory_iterator{path}) {
        cout << dir_entry << endl;
        diritems++;
    }
    cout << diritems << " directory items in " << path << endl;
    stringstream path_s;
    path_s << path << "/" << std::setfill('0') << std::setw(4) << diritems << "_meteobike.csv";
    cout << path_s.str() << endl;
    ofstream ofile(path_s.str());
    if (!ofile.is_open())
    {
        cout << "File " << path_s.str() << " could not be opened." << endl;
        exit(-1);
    }
}