#include "writer.h"
#include <iostream>
#include <chrono>
#include <
using namespace std;

writer::~writer()
{
}

writer::writer(string path):
path(path) 
{
    ofile.open(path);
    if (!ofile.is_open())
    {
        cout << "File " << path << " could not be opened." << endl;
        exit(-1);
    }
}