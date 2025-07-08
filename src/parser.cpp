#include "main.hpp"

// TODO: neni to result.json jako output!!!!!

Parse_t parse(int argc, char* argv[]) {
    Parse_t parsed(INT_NOT_INIT, STR_NOT_INIT, STR_NOT_INIT, INT_NOT_INIT);
    std::string argument, option, input;

    std::regex kml_pattern(".*\\.kml$"); 
    std::regex json_pattern(".*\\.json$"); 

    int arg_index = 1;
    while (arg_index < argc) {
        argument = std::string(argv[arg_index]);

        if (argument == "--mode") {
            if (arg_index + 1 >= argc) {
                print("Parser error: Invalid mode argument. Mode argument must be either 'preprocess' or 'calculate'.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            } else if (parsed.mode != INT_NOT_INIT) {
                print("Parser error: Duplicit mode argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            option = std::string(argv[++arg_index]);
            if (option != "preprocess" && option != "calculate") {
                print("Parser error: Invalid mode argument. Mode argument must be either 'preprocess' or 'calculate'.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            parsed.mode = (option == "preprocess") ? PREPROCESS : CALCULATE;
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

            input = "map";
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

            input = "data";
            parsed.input_file = option;
        } else if (argument == "--algorithm") {
            if (arg_index + 1 >= argc) {
                print("Parser error: Invalid algorithm argument. Algorithm argument must be one of the following: 'greedy-algorithm', 'simulated-annealing' or 'genetic-algorithm'.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            } else if (parsed.algorithm != INT_NOT_INIT) {
                print("Parser error: Duplicit algorithm argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            option = std::string(argv[++arg_index]);
            if (option != "greedy-algorithm" && option != "simulated-annealing" && option != "genetic-algorithm") {
                print("Parser error: Invalid algorithm argument. Algorithm argument must be one of the following: 'greedy-algorithm', 'simulated-annealing' or 'genetic-algorithm'.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            parsed.algorithm = (option == "greedy-algorithm") ? GREEDY : (option == "simulated-annealing") ? ANNEALING : GENETIC;
        } else if (argument == "--output") {
            if (arg_index + 1 >= argc) {
                print("Parser error: Invalid output argument. Output argument must specify a valid <result> path.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            } else if (parsed.output_file != STR_NOT_INIT) {
                print("Parser error: Duplicit output argument.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            option = std::string(argv[++arg_index]);
            std::ifstream file(option);
            if (!file) {
                print("Parser error: Invalid output argument. Output argument must specify a valid <result> path.", RED, STDERR);
                print("Use ./towers --help for more information.", YELLOW, STDERR);
                exit(1);
            }

            parsed.output_file = option;
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
        print("Parser error: Mode argument required.", RED, STDERR);
        print("Use ./towers --help for more information.", YELLOW, STDERR);
        exit(1);
    } else if (parsed.input_file == STR_NOT_INIT) {
        print("Parser error: Map/data argument required.", RED, STDERR);
        print("Use ./towers --help for more information.", YELLOW, STDERR);
        exit(1);
    }

    if (parsed.mode == PREPROCESS) {
        if (parsed.algorithm != INT_NOT_INIT) {
            print("Parser error: Algorithm argument can be used only in 'calculate' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        } else if (input == "data") {
            print("Parser error: Data argument can be used only in 'calculate' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        }
    } else {
        if (input == "map") {
            print("Parser error: Map argument can be used only in 'preprocess' mode.", RED, STDERR);
            print("Use ./towers --help for more information.", YELLOW, STDERR);
            exit(1);
        }
    }

    if (parsed.mode == CALCULATE) {
        if (parsed.algorithm == INT_NOT_INIT)
            parsed.algorithm = GREEDY;
        if (parsed.output_file == STR_NOT_INIT)
            parsed.output_file = "result.json";
    } else {
        if (parsed.output_file == STR_NOT_INIT)
            parsed.output_file = "preprocessed.json";
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
    print("  --mode <mode>                Required. Mode of operation: 'preprocess' or 'calculate'.\n", OFF, STDOUT);
    print("  Preprocess Mode Options:", CYAN, STDOUT);
    print("    --map <map_file.kml>       Required. Input map file in KML format.", OFF, STDOUT);
    print("    --output <output_file>     Path to save the generated data file in JSON format. Default is 'preprocessed.json' in current working directory.\n", OFF, STDOUT);
    print("  Calculate Mode Options:", CYAN, STDOUT);
    print("    --data <data_file.json>    Required. Input data file generated from preprocess mode in JSON format.", OFF, STDOUT);
    print("    --algorithm <name>         Algorithm to use. Options: 'greedy-algorithm', 'simulated-annealing', 'genetic-algorithm'. Default is 'greedy-algorithm'.", OFF, STDOUT);
    print("    --output <output_file>     Path to save the result. Default is 'result.json' in current working directory.\n", OFF, STDOUT);
    print("General:", CYAN, STDOUT);
    print("  --help                       Show this help message and exit.\n", OFF, STDOUT);
    print("Examples:", CYAN, STDOUT);
    print("  ./towers --mode preprocess --map towers.kml --output preprocessed.json", OFF, STDOUT);
    print("  ./towers --mode calculate --data preprocessed.json --algorithm simulated-annealing --output result.json", OFF, STDOUT);
    
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