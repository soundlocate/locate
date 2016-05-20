#ifndef _CSVTABLE_H
#define _CSVTABLE_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class CSVTable {
public:
    CSVTable(const char * filename, char seperator = ',');

    std::vector<std::vector<std::string>> getRows();

    ~CSVTable();

private:
    std::string m_fileContent;
    char        m_seperator;
};

#endif
