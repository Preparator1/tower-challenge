#include "main.hpp"

int distance_mapping(size_t towers_count, size_t index_l, size_t index_g) {
    if (index_l > index_g)
        std::swap(index_l, index_g);

    return towers_count * index_l - (index_l * (index_l + 1) / 2) + index_g - (index_l + 1);
}

std::string create_table_request_string(std::vector<Place_t> towers, size_t src_s, size_t src_e, size_t dst_s, size_t dst_e) {
    std::string request_string(OSRM_SERVER"/table/v1/walking/");
    bool same_dimensions = true;

    for (size_t i = src_s; i < towers.size() && i <= src_e; i++) {
        request_string.append(std::to_string(towers[i].gps.first) + "," + std::to_string(towers[i].gps.second));
        if (i < src_e) request_string.append(";");
    }

    if (src_s != dst_s && src_e != dst_e) {
        request_string.append(";");
        same_dimensions = false;

        for (size_t i = dst_s; i < towers.size() && i <= dst_e; i++) {
            request_string.append(std::to_string(towers[i].gps.first) + "," + std::to_string(towers[i].gps.second));
            if (i < dst_e) request_string.append(";");
        }
    }

    request_string.append("?annotations=distance");

    if (!same_dimensions) {
        request_string.append("&sources=");
        for (size_t i = src_s; i < towers.size() && i <= src_e; i++) {
            request_string.append(std::to_string(i));
            if (i < src_e) request_string.append(";");
        }

        request_string.append("&destinations=");
        for (size_t i = dst_s; i < towers.size() && i <= dst_e; i++) {
            request_string.append(std::to_string(i));
            if (i < dst_e) request_string.append(";");
        }
    }

    return request_string;
}

std::vector<double> get_distances(std::vector<Place_t> towers) {
    size_t towers_count = towers.size();
    int distances_count = (towers_count * towers_count) / 2 - (towers_count / 2);
    std::vector<double> distances(distances_count);

    std::string request_string;
    nlohmann::json request_data;

    for (size_t src_s = 0, src_e = 99; src_s < towers_count; src_s += 100, src_e += 100) {
        for (size_t dst_s = src_s, dst_e = src_e; dst_s < towers_count; dst_s += 100, dst_e += 100) {
            request_string = create_table_request_string(towers, src_s, src_e, dst_s, dst_e);
            request_data = request(request_string);

            if (request_data["code"] != "Ok") {
                std::cerr << RED"Request error: " << request_data["code"] << OFF << std::endl;
                exit(1);
            }

            if (src_s == dst_s) {
                
            } else {
                for (size_t i = 0; i < request_data["distances"].size(); i++) {
                    for (size_t j = 0; j < request_data["distances"][i].size(); j++) {
                        distances[distance_mapping(towers_count, src_s + i, dst_s + j)] = request_data["distances"][i][j];
                    }
                }
            }

            std::cout << request_data["distances"][0] << std::endl;
        }
    }
    return distances;
}

void preprocess_data(std::vector<Place_t> towers) {
    std::string distance_string_base(OSRM_SERVER"/route/v1/walking/");
    std::string elevation_string_base(OPENTOPODATA_SERVER"/v1/eudem25m?locations=");

    nlohmann::json request_data;

    size_t towers_count = towers.size();
    int paths_count = towers_count * (towers_count - 1) / 2;
    std::vector<std::pair<int, int>> preprocessed_data(paths_count);

    for (size_t tower_id_1 = 0; tower_id_1 < towers_count; tower_id_1++) {
        for (size_t tower_id_2 = tower_id_1 + 1; tower_id_2 < towers_count; tower_id_2++) {
            std::string request_string = distance_string_base;

            request_string.append(std::to_string(towers[tower_id_1].gps.first) + "," + std::to_string(towers[tower_id_1].gps.second) + ";");
            request_string.append(std::to_string(towers[tower_id_2].gps.first) + "," + std::to_string(towers[tower_id_2].gps.second) + "?");
            request_string.append("overview=simplified&geometries=geojson");

            request_data = request(request_string);
            if (request_data["code"] != "Ok") {
                print("Request error: " + std::string(request_data["code"]), RED, STDERR);
                exit(1);
            }
            
            std::cout << request_string << std::endl;
            request_string = elevation_string_base;

            auto path_points = request_data["routes"][0]["geometry"]["coordinates"];
            size_t path_points_size = path_points.size();

            for (size_t point_id = 0; point_id < path_points_size; point_id++) {
                request_string.append(std::to_string(path_points[point_id][1].get<double>()) + "," + std::to_string(path_points[point_id][0].get<double>()));
                if (point_id + 1 < path_points_size) request_string.append("|");
            }

            std::cout << request_string << std::endl;
            std::cout << request_data["routes"][0]["distance"] << std::endl;
            std::cout << towers[tower_id_1].name << "|" << towers[tower_id_2].name << std::endl;
            exit(1);
        }
    }
}