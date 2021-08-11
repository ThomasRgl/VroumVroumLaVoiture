#ifndef CSV_HH
#define CSV_HH

#include <string>
#include <fstream>
#include <vector>
#include <utility> // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream
#include <iostream>

std::vector<std::pair<std::string, std::vector<int>>> read_csv(std::string filename);

#endif
