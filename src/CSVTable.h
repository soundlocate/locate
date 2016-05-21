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

    //    std::vector<std::vector<std::string>> getRows();

    std::vector<std::vector<std::string>> getRows() {

        std::stringstream                     file(m_fileContent);
        std::string                           line;
        std::string                           cell;
        std::vector<std::string>              cells;
        std::vector<std::vector<std::string>> rows;

        // read all lines
        while(std::getline(file, line)) {
            std::stringstream lineStream(line);

            // hack getline to use the cell seperator to read all cells in one
            // line
            while(std::getline(lineStream, cell, m_seperator)) {
                cells.push_back(cell);
            }

            // push copy of cells to the total rows
            rows.push_back(cells);

            // clear this copy of all cells
            cells.clear();
        }

        return rows;
    }

    ~CSVTable();

private:
    std::string m_fileContent;
    char        m_seperator;
};

#endif
