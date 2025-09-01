#include "main.hpp"

int main(int argc, char* argv[]) {
    Parse_t arguments = parse(argc, argv);
    std::vector<Tower_t> towers;
    std::vector<Path_t> tower_paths;

    std::atexit(restore_cursor);
    std::signal(SIGINT, signal_handler);

    if (arguments.mode == PREPROCESS) {
        towers = extract_map(arguments.input_file);
        tower_paths = preprocess_map_data(towers);
        save_preprocessed_data(arguments.output_file, towers, tower_paths);
    } else if (arguments.mode == CALCULATE) {
        Preprocessed_data_t data = extract_preprocessed_data(arguments.input_file);
        std::vector<Tower_t> best_solution;

        switch (arguments.algorithm) {
            case GREEDY:
                best_solution = greedy_algorithm(data, arguments.time, arguments.pace);
                break;
            
            case ANNEALING:
                break;
    
            case GENETIC:
                break;
        }

        save_best_solution(arguments.output_file, best_solution);
    } else {
        std::vector<Tower_t> towers = extract_solution(arguments.input_file);
        std::vector<std::pair<double, double>> path_coordinates = refine_solution(towers);
        save_refined_solution(arguments.output_file, towers, path_coordinates);
    }
    
    return 0;
}