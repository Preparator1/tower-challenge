#include "main.hpp"

std::vector<std::pair<double, double>> refine_solution(std::vector<Tower_t>& towers) {
    size_t towers_count = towers.size();
    std::vector<std::pair<double, double>> path_coordinates;

    double longitude = towers[0].gps.first;
    double latitude = towers[0].gps.second;

    path_coordinates.emplace_back(std::make_pair(longitude, latitude));

    for (size_t tower = 1; tower < towers_count; ++tower) {
        size_t previous_tower = tower - 1;
        nlohmann::json request_data = get_osrm_data(towers, previous_tower, tower);        

        auto path_points = request_data["routes"][0]["geometry"]["coordinates"];
        size_t path_points_size = path_points.size();

        for (size_t point_id = 1; point_id < path_points_size; ++point_id) {
            longitude = path_points[point_id][0].get<double>();
            latitude = path_points[point_id][1].get<double>();

            path_coordinates.emplace_back(std::make_pair(longitude, latitude));
        }
    }

    return path_coordinates;
}