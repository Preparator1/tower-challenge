#include "main.hpp"

size_t get_distance(size_t towers_count, std::vector<Tower_t>& candidate_solution, std::vector<Path_t>& paths) {
    size_t distance = 0;
    size_t from_tower, to_tower, index;

    for (size_t i = 1; i < candidate_solution.size(); ++i) {
        from_tower = candidate_solution[i - 1].id - 1;
        to_tower = candidate_solution[i].id - 1;

        index = index_mapping(towers_count, from_tower, to_tower);
        distance += paths[index].distance;
    }

    return distance;
}

size_t get_elevation(size_t towers_count, std::vector<Tower_t>& candidate_solution, std::vector<Path_t>& paths) {
    size_t elevation = 0;
    size_t from_tower, to_tower, index;

    for (size_t i = 1; i < candidate_solution.size(); ++i) {
        from_tower = candidate_solution[i - 1].id - 1;
        to_tower = candidate_solution[i].id - 1;

        index = index_mapping(towers_count, from_tower, to_tower);
        elevation += paths[index].elevation;
    }

    return elevation;
}

double get_time(size_t towers_count, size_t current_tower, size_t candidate_tower, double pace, std::vector<Path_t>& paths) {
    Path_t path = paths[index_mapping(towers_count, current_tower, candidate_tower)];

    double km_distance = path.distance / 1000.0;
    double km_elevation = path.elevation / 1000.0;
    double hours = km_distance / pace + (km_elevation * ELEVATION_PENALTY) / pace;

    return hours * 3600;
}

uint8_t get_points(std::vector<Tower_t>& candidate_solution) {
    uint8_t points = 0;
    for (Tower_t& tower : candidate_solution) {
        points += tower.category;
    }

    return points;
}

std::tuple<uint16_t, uint32_t> greedy_search(size_t towers_count, size_t current_tower, std::vector<Tower_t>& candidate_towers, double pace, std::vector<Path_t>& paths) {
    double best_time = INFINITY;
    uint16_t best_id;
    
    double time;
    for (size_t candidate_tower = 0; candidate_tower < towers_count; ++candidate_tower) {
        if (candidate_towers[candidate_tower].id == VISITED)
            continue;

        time = get_time(towers_count, current_tower, candidate_tower, pace, paths);
        if (time < best_time) {
            best_id = candidate_tower;
            best_time = time;
        }
    }

    return std::make_tuple(best_id, static_cast<uint32_t>(best_time)); 
}

std::vector<Tower_t> greedy_algorithm(Preprocessed_data_t& data, uint32_t time_limit, double pace) {
    std::vector<Tower_t>& towers = data.towers;
    std::vector<Path_t>& paths = data.paths;

    size_t towers_count = towers.size();
    size_t current_tower;

    std::vector<std::pair<std::vector<Tower_t>, uint32_t>> solutions;
    uint8_t highest_points = 0;

    uint32_t time;
    uint8_t points;

    for (size_t starting_tower = 0; starting_tower < towers_count; ++starting_tower) {
        std::vector<Tower_t> candidate_towers = towers;
        std::vector<Tower_t> candidate_solution;

        current_tower = starting_tower;
        candidate_towers[starting_tower].id = VISITED;
        candidate_solution.push_back(towers[starting_tower]);
        
        time = 0;
        while (true) {
            std::tuple<uint16_t, uint32_t> greedy_result = greedy_search(towers_count, current_tower, candidate_towers, pace, paths);
            if (time + std::get<TIME>(greedy_result) > time_limit) {
                break;
            }

            current_tower = std::get<ID>(greedy_result);
            time += std::get<TIME>(greedy_result);

            candidate_towers[current_tower].id = VISITED;
            candidate_solution.push_back(towers[current_tower]);
        }

        points = get_points(candidate_solution);

        if (points > highest_points) {
            highest_points = points;
        }

        solutions.push_back(std::make_pair(candidate_solution, time));
    }

    std::vector<std::pair<std::vector<Tower_t>, uint32_t>> best_points_solutions;
    for (size_t solution = 0; solution < solutions.size(); ++solution) {
        if (get_points(solutions[solution].first) == highest_points) {
            best_points_solutions.push_back(solutions[solution]);
        }
    }

    for (size_t trial = 0; trial < best_points_solutions.size(); ++trial) {
        print("Result: " + std::to_string(trial), YELLOW, STDOUT);
        
        std::vector<Tower_t> solution = best_points_solutions[trial].first;
        for (size_t tower = 0; tower < solution.size(); ++tower) {
            print("Tower: " + solution[tower].name, GREEN, STDOUT);
        }

        print("Distance: " + std::to_string(get_distance(towers_count, solution, paths)), YELLOW, STDOUT);
        print("Elevation: " + std::to_string(get_elevation(towers_count, solution, paths)), YELLOW, STDOUT);
    }

    uint32_t best_time = time_limit + 1;
    std::vector<Tower_t> best_solution;
    for (size_t solution = 0; solution < best_points_solutions.size(); ++solution) {
        time = best_points_solutions[solution].second;
        if (time < best_time) {
            best_solution = best_points_solutions[solution].first;
            best_time = time;
        }
    }

    return best_solution;
}