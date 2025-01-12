#pragma once

#include <iostream>
#include <string>
#include <fstream> 
#include <sstream>
#include <atomic>
#include "measurement.h"


class writer
{
private:
    /* data */
    std::string path, hostname, ip;
    std::stringstream record;
    std::ofstream ofile;
    int diritems;
    void writeHeader();    
public:
    writer(const std::string &path, const std::string &hostname, const std::string &ip);
    void createRecord(const results_r &myresult);
    void writeRecord();
    std::atomic<int> counter=0;
    ~writer();
};

