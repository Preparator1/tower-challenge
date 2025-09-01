#include "main.hpp"

Parse_t parse(int argc, char* argv[]) {
    Parse_t parsed(INT_NOT_INIT, STR_NOT_INIT, STR_NOT_INIT, INT_NOT_INIT, INT_NOT_INIT, DOUBLE_NOT_INIT);
    std::string argument, option, input;

    std::regex kml_pattern(".*\\.kml$"); 
    std::regex json_pattern(".*\\.json$"); 
    std::regex gpx_pattern(".*\\.gpx$"); 
    std::regex time_pattern(R"(^(?:[1-9]|[1-9]\d|1\d\d|2[0-4]\d|25[0-5])$)");
    std::regex pace_pattern(R"(^(?:[1-9]|1[0-5])(\.\d)?$)");

    int arg_index = 1;
    while (arg_index < argc) {
        argument = std::string(argv[arg_index]);

        if (argument == "--mode") {
            if (arg_index + 1 >= argc) {
                print("Parser error: Invalid mode argument. Mode argument must be either 'preprocess', 'calculate' or 'refine'.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            } else if (parsed.mode != INT_NOT_INIT) {
                print("Parser error: Duplicit mode argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            option = std::string(argv[++arg_index]);
            if (option != "preprocess" && option != "calculate" && option != "refine") {
                print("Parser error: Invalid mode argument. Mode argument must be either 'preprocess', 'calculate' or 'refine'.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            parsed.mode = (option == "preprocess") ? PREPROCESS : (option == "calculate") ? CALCULATE : REFINE;
        } else if (argument == "--map") {
             if (arg_index + 1 >= argc) {
                print("Parser error: Invalid map argument. Map argument must specify a valid <map_file.kml> path.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            } else if (parsed.input_file != STR_NOT_INIT) {
                print("Parser error: Duplicit map/data argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            option = std::string(argv[++arg_index]);
            std::ifstream file(option);
            if (!file || !std::regex_match(option, kml_pattern)) {
                print("Parser error: Invalid map argument. Map argument must specify a valid <map_file.kml> path.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            input = "Map";
            parsed.input_file = option;
        } else if (argument == "--data") {
            if (arg_index + 1 >= argc) {
                print("Parser error: Invalid data argument. Data argument must specify a valid <data_file.json> path.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            } else if (parsed.input_file != STR_NOT_INIT) {
                print("Parser error: Duplicit map/data argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            option = std::string(argv[++arg_index]);
            std::ifstream file(option);
            if (!file || !std::regex_match(option, json_pattern)) {
                print("Parser error: Invalid data argument. Data argument must specify a valid <data_file.json> path.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            input = "Data";
            parsed.input_file = option;
        } else if (argument == "--solution") {
            if (arg_index + 1 >= argc) {
                print("Parser error: Invalid solution argument. Solution argument must specify a valid <solution.gpx> path.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            } else if (parsed.input_file != STR_NOT_INIT) {
                print("Parser error: Duplicit solution argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            option = std::string(argv[++arg_index]);
            std::ifstream file(option);
            if (!file || !std::regex_match(option, gpx_pattern)) {
                print("Parser error: Invalid solution argument. Solution argument must specify a valid <solution.gpx> path.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            input = "Solution";
            parsed.input_file = option;
        } else if (argument == "--time") {
            if (arg_index + 1 >= argc) {
                print("Parser error: Invalid time argument. Time argument must specify a valid numeric value in hours between 1 and 255.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            } else if (parsed.time != INT_NOT_INIT) {
                print("Parser error: Duplicit time argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            option = std::string(argv[++arg_index]);
            if (!std::regex_match(option, time_pattern)) {
                print("Parser error: Invalid time argument. Time argument must specify a valid numeric value in hours between 1 and 255.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            parsed.time = std::stoi(option) * 3600;
        } else if (argument == "--pace") {
            if (arg_index + 1 >= argc) {
                print("Parser error: Invalid pace argument. Pace argument must specify a valid numeric value in kilometers per hour between 1 and 15, with at most one digit after the decimal point.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            } else if (parsed.pace != DOUBLE_NOT_INIT) {
                print("Parser error: Duplicit pace argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            option = std::string(argv[++arg_index]);
            if (!std::regex_match(option, pace_pattern)) {
                print("Parser error: Invalid pace argument. Pace argument must specify a valid numeric value in kilometers per hour between 1 and 15, with at most one digit after the decimal point.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            parsed.pace = std::stod(option);
        } else if (argument == "--algorithm") {
            if (arg_index + 1 >= argc) {
                print("Parser error: Invalid algorithm argument. Algorithm argument must be one of the following: 'greedy-search', 'simulated-annealing' or 'genetic-algorithm'.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            } else if (parsed.algorithm != INT_NOT_INIT) {
                print("Parser error: Duplicit algorithm argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            option = std::string(argv[++arg_index]);
            if (option != "greedy-search" && option != "simulated-annealing" && option != "genetic-algorithm") {
                print("Parser error: Invalid algorithm argument. Algorithm argument must be one of the following: 'greedy-search', 'simulated-annealing' or 'genetic-algorithm'.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            parsed.algorithm = (option == "greedy-search") ? GREEDY : (option == "simulated-annealing") ? ANNEALING : GENETIC;
        } else if (argument == "--output") {
            if (arg_index + 1 >= argc) {
                print("Parser error: Invalid output argument. Output argument must specify a valid <output_file> path.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            } else if (parsed.output_file != STR_NOT_INIT) {
                print("Parser error: Duplicit output argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            parsed.output_file = std::string(argv[++arg_index]);
        } else if (argument == "--help") {
            if (argc != 2) {
                print("Parser error: Invalid help argument. Help argument must be used alone, without any other argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            help();
        } else {
            print("Parser error: Invalid argument format.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        }

        arg_index++;
    }

    if (parsed.mode == INT_NOT_INIT) {
        print("Parser error: Mode argument is required.", RED, STDERR);
        print("Use ./towers --help for more information.", YELLOW, STDERR);
        exit(1);
    } else if (parsed.input_file == STR_NOT_INIT) {
        std::string right_input = (parsed.mode == PREPROCESS) ? "Map" : (parsed.mode == CALCULATE) ? "Data" : "Solution";
        print("Parser error: " + right_input + " argument is required.", RED, STDERR);
        print("Use ./towers --help for more information.", YELLOW, STDERR);
        exit(1);
    }

    if (parsed.mode == PREPROCESS) {
        if (parsed.algorithm != INT_NOT_INIT) {
            print("Parser error: Algorithm argument can be used only in 'calculate' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        } else if (parsed.time != INT_NOT_INIT) {
            print("Parser error: Time argument can be used only in 'calculate' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        } else if (parsed.pace != DOUBLE_NOT_INIT) {
            print("Parser error: Pace argument can be used only in 'calculate' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        } else if (input == "Data") {
            print("Parser error: Data argument can be used only in 'calculate' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        } else if (input == "Solution") {
            print("Parser error: Solution argument can be used only in 'refine' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        }
    } else if (parsed.mode == CALCULATE) {
        if (parsed.time == INT_NOT_INIT) {
            print("Parser error: Time argument is required in 'calculate' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        } else if (input == "Map") {
            print("Parser error: Map argument can be used only in 'preprocess' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        } else if (input == "Solution") {
            print("Parser error: Solution argument can be used only in 'refine' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        }
    } else {
        if (parsed.algorithm != INT_NOT_INIT) {
            print("Parser error: Algorithm argument can be used only in 'calculate' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        } else if (parsed.time != INT_NOT_INIT) {
            print("Parser error: Time argument can be used only in 'calculate' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        } else if (parsed.pace != DOUBLE_NOT_INIT) {
            print("Parser error: Pace argument can be used only in 'calculate' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        } else if (input == "Map") {
            print("Parser error: Map argument can be used only in 'preprocess' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        } else if (input == "Data") {
            print("Parser error: Data argument can be used only in 'calculate' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        }
    }

    if (parsed.mode == PREPROCESS) {
        if (parsed.output_file == STR_NOT_INIT)
            parsed.output_file = "preprocessed.json";
            
    } else if (parsed.mode == CALCULATE) {
        if (parsed.algorithm == INT_NOT_INIT)
            parsed.algorithm = GREEDY;

        if (parsed.output_file == STR_NOT_INIT)
            parsed.output_file = "solution.gpx";

        if (parsed.pace == DOUBLE_NOT_INIT)
            parsed.pace = 4.0;

    } else {
        if (parsed.output_file == STR_NOT_INIT)
            parsed.output_file = parsed.input_file.substr(0, parsed.input_file.size() - 4) + "_refined.gpx";
    }

    return parsed;
}

void help() {
    print("Usage:", CYAN, STDOUT);
    print("  ./towers --mode <mode> [options]\n", OFF, STDOUT);
    print("Modes:", CYAN, STDOUT);
    print("  preprocess                   Preprocess a map file and generate a preprocessed data file.", OFF, STDOUT);
    print("  calculate                    Calculate results using a specified algorithm.\n", OFF, STDOUT);
    print("Options:", CYAN, STDOUT);
    print("  --mode <mode>                Required. Mode of operation: 'preprocess', 'calculate' or 'refine'.\n", OFF, STDOUT);
    print("  Preprocess Mode Options:", CYAN, STDOUT);
    print("    --map <map_file.kml>       Required. Input map file in KML format.", OFF, STDOUT);
    print("    --output <output_file>     Path to save the generated data file in JSON format. Default is 'preprocessed.json' in current working directory.\n", OFF, STDOUT);
    print("  Calculate Mode Options:", CYAN, STDOUT);
    print("    --data <data_file.json>    Required. Input data file generated from preprocess mode in JSON format.", OFF, STDOUT);
    print("    --time <hours>             Required. Time limit for the route, specified in hours. Argument must be between 1 and 255.", OFF, STDOUT);
    print("    --pace <km/h>              Walking speed in kilometers per hour. Argument must be between 1 and 15, with at most one digit after the decimal point. Default is 4 km/h.", OFF, STDOUT);
    print("    --algorithm <name>         Algorithm to use. Options: 'greedy-search', 'simulated-annealing', 'genetic-algorithm'. Default is 'greedy-search'.", OFF, STDOUT);
    print("    --output <output_file>     Path to save the solution in GPX format. Default is 'solution.gpx' in current working directory.\n", OFF, STDOUT);
    print("  Refine Mode Options:", CYAN, STDOUT);
    print("    --solution <solution.gpx>  Required. Solution generated in calculate mode in GPX format.", OFF, STDOUT);
    print("    --output <output_file>     Path to save the refined data file in GPX format. Default is '<solution>_refined.gpx' in current working directory.\n", OFF, STDOUT);
    print("General:", CYAN, STDOUT);
    print("  --help                       Show this help message and exit.\n", OFF, STDOUT);
    print("Examples:", CYAN, STDOUT);
    print("  ./towers --mode preprocess --map towers.kml --output preprocessed.json", OFF, STDOUT);
    print("  ./towers --mode calculate --data preprocessed.json --algorithm simulated-annealing --output solution.gpx", OFF, STDOUT);
    
    exit(0);
}

void print(std::string text, std::string color, uint8_t output) {
    switch (output) {
        case STDOUT:
            std::cout << color + text + OFF << std::endl;
            break;

        case STDERR:
            std::cerr << color + text + OFF << std::endl;
            break;
    }
}