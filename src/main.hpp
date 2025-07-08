#ifndef MAIN_H
#define MAIN_H

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <regex>
#include <vector>
#include <algorithm>
#include <nlohmann/json.hpp>

#define GREEN "\033[1;92m"
#define RED "\033[1;91m"
#define YELLOW "\033[1;93m"
#define CYAN "\033[1;96m"
#define WHITE "\033[1;97m"
#define OFF "\033[0m"

#define OSRM_SERVER "http://router.project-osrm.org"
#define OPENTOPODATA_SERVER "https://api.opentopodata.org"

#define STR_NOT_INIT ""
#define INT_NOT_INIT 7

enum Datastream {
    STDIN = 0,
    STDOUT = 1,
    STDERR = 2
};

enum Mode {
    PREPROCESS = 0,
    CALCULATE = 1
};

enum Algorithm {
    GREEDY = 0,
    ANNEALING = 1,
    GENETIC = 2
};

typedef struct Parse {
    uint8_t mode;
    std::string input_file;
    std::string output_file;
    uint8_t algorithm;

    Parse(uint8_t _mode, std::string _input_file, std::string _output_file, uint8_t _algorithm): mode(_mode), input_file(_input_file), output_file(_output_file), algorithm(_algorithm) {}

} Parse_t;

typedef struct Place {
    uint id;
    std::string name;
    uint8_t category;
    std::pair<double, double> gps;

    Place(uint _id, std::string& _name, uint8_t _category, std::pair<double, double> _gps): id(_id), name(_name), category(_category), gps(_gps) {}

} Place_t;

Parse_t parse(int argc, char* argv[]);
std::vector<Place_t> extract_map(std::string file_path);
nlohmann::json request(std::string url);
std::vector<double> get_distances(std::vector<Place_t> towers);
void help();
void print(std::string text, std::string color, uint8_t datastream);
void preprocess_data(std::vector<Place_t> towers);

#endif