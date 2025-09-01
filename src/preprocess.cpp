#include "main.hpp"

size_t get_elevation(nlohmann::json elevation_array) {
    size_t elevation = 0;
    for (size_t i = 1; i < elevation_array.size(); i++)
        elevation += abs(elevation_array[i - 1]["elevation"].get<double>() - elevation_array[i]["elevation"].get<double>());

    return elevation;
}

std::vector<Path_t> preprocess_map_data(std::vector<Tower_t>& towers) {
    std::string elevation_string_base(OPENTOPODATA_SERVER"/v1/eudem25m?locations=");

    nlohmann::json request_data;

    size_t towers_count = towers.size();
    size_t paths_count = towers_count * (towers_count - 1) / 2;

    std::vector<Path_t> tower_paths;
    size_t distance, elevation;

    hide_cursor();
    
    size_t path_num = 1;
    for (size_t tower_id_1 = 0; tower_id_1 < towers_count; ++tower_id_1) {
        for (size_t tower_id_2 = tower_id_1 + 1; tower_id_2 < towers_count; ++tower_id_2) {
            request_data = get_osrm_data(towers, tower_id_1, tower_id_2);
            distance = request_data["routes"][0]["distance"].get<int>();

            nlohmann::json path_points = request_data["routes"][0]["geometry"]["coordinates"];
            request_data = get_otd_data(path_points);
            elevation = get_elevation(request_data["results"]);

            Path_t path(std::to_string(tower_id_1) + ":" + std::to_string(tower_id_2), distance, elevation);
            tower_paths.push_back(path);

            progress("Analyzing paths", path_num, paths_count);
            path_num++;
        }
    }

    clear_line();
    show_cursor();
   
    return tower_paths;
}