#pragma once

#include <iostream>
#include <string>
#include <fstream> 
using namespace std;

class writer
{
private:
    /* data */
    string path;
    ofstream ofile;
    int diritems;    
public:
    writer(string path);
    ~writer();
};

