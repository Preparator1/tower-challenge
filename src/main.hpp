#ifndef MAIN_H
#define MAIN_H

#include <fstream>
#include <iostream>
#include <csignal>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <utility>
#include <regex>
#include <vector>
#include <tuple>
#include <algorithm>
#include <limits>
#include <nlohmann/json.hpp>
#include <pugixml.hpp>

#define GREEN "\033[1;92m"
#define RED "\033[1;91m"
#define YELLOW "\033[1;93m"
#define CYAN "\033[1;96m"
#define WHITE "\033[1;97m"
#define OFF "\033[0m"

#define OSRM_SERVER "http://localhost:5000"
#define OPENTOPODATA_SERVER "http://localhost:5001"

#define STR_NOT_INIT ""
#define DOUBLE_NOT_INIT 0.0
#define INT_NOT_INIT std::numeric_limits<uint8_t>::max()
#define VISITED std::numeric_limits<uint16_t>::max()
#define ELEVATION_PENALTY 10.0
#define SIX_PLACES_PRECISION 1000000

#define UNDEFINED_IDENTIFIER 0
#define UNDEFINED_CATEGORY 0

enum Datastream {
    STDIN = 0,
    STDOUT = 1,
    STDERR = 2
};

enum Mode {
    PREPROCESS = 0,
    CALCULATE = 1,
    REFINE = 2
};

enum Algorithm {
    GREEDY = 0,
    ANNEALING = 1,
    GENETIC = 2
};

enum GreedyResult {
    ID = 0,
    TIME = 1
};

typedef struct Parse {
    uint8_t mode;
    std::string input_file;
    std::string output_file;
    uint8_t algorithm;
    uint32_t time;
    double pace;
   
    Parse(uint8_t _mode, std::string _input_file, std::string _output_file, uint8_t _algorithm, uint32_t _time, double _pace): mode(_mode), input_file(_input_file), output_file(_output_file), algorithm(_algorithm), time(_time), pace(_pace) {}

} Parse_t;

typedef struct Tower {
    uint16_t id;
    std::string name;
    uint8_t category;
    std::pair<double, double> gps;

    Tower(uint16_t _id, std::string _name, uint8_t _category, std::pair<double, double> _gps): id(_id), name(_name), category(_category), gps(_gps) {}

} Tower_t;

typedef struct Path {
    std::string id;
    uint32_t distance;
    uint32_t elevation;

    Path(std::string _id, uint32_t _distance, uint32_t _elevation): id(_id), distance(_distance), elevation(_elevation) {}

} Path_t;

typedef struct Preprocessed_data {
    std::vector<Tower_t> towers;
    std::vector<Path_t> paths;
} Preprocessed_data_t;

Parse_t parse(int argc, char* argv[]);

int index_mapping(size_t towers_count, size_t lower_index, size_t higher_index);
nlohmann::json get_osrm_data(std::vector<Tower_t>& towers, size_t tower_1, size_t tower_2);
nlohmann::json get_otd_data(nlohmann::json& path_points);
nlohmann::json request(std::string url);

std::vector<Tower_t> extract_map(std::string file_path);
Preprocessed_data_t extract_preprocessed_data(std::string data_file);
std::vector<Tower_t> extract_solution(std::string solution_path);

std::vector<Path_t> preprocess_map_data(std::vector<Tower_t>& towers);
std::vector<std::pair<double, double>> refine_solution(std::vector<Tower_t>& towers);

std::vector<Tower_t> greedy_algorithm(Preprocessed_data_t& data, uint32_t time, double pace);

void save_preprocessed_data(std::string output_file, std::vector<Tower_t>& towers, std::vector<Path_t>& tower_paths);
void save_best_solution(std::string output_file, std::vector<Tower_t>& best_solution);
void save_refined_solution(std::string output_file, std::vector<Tower_t>& towers, std::vector<std::pair<double, double>>& path_coordinates);

void print(std::string text, std::string color, uint8_t datastream);
void progress(std::string work, int progress, int total);
void clear_line();
void hide_cursor();
void show_cursor();
void restore_cursor();
void signal_handler(int signum);
void help();

#endif