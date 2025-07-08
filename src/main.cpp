#include "main.hpp"

int main(int argc, char* argv[]) {
    Parse_t arguments = parse(argc, argv);
    std::vector<Place_t> towers;

    if (arguments.mode == PREPROCESS) {
        towers = extract_map(arguments.input_file);
        preprocess_data(towers);
    } else {

    }
    
    //std::vector<Place_t> towers = extract(arguments.map);
    //std::vector<double> distances = get_distances(towers);

    //request(request_string.c_str());
    return 0;
}