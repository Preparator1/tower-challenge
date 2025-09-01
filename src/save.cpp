#include "main.hpp"

void save_preprocessed_data(std::string output_file, std::vector<Tower_t>& towers, std::vector<Path_t>& tower_paths) {
    size_t towers_count = towers.size();
    size_t paths_count = tower_paths.size();

    nlohmann::ordered_json preprocessed_file;
    preprocessed_file["towers"] = nlohmann::ordered_json::array();

    for (size_t i = 0; i < towers_count; i++) {
        nlohmann::ordered_json gps;
        gps["longitude"] = towers[i].gps.first;
        gps["latitude"] = towers[i].gps.second;

        nlohmann::ordered_json tower;
        tower["id"] = towers[i].id;
        tower["name"] = towers[i].name;
        tower["category"] = towers[i].category;
        tower["gps"] = gps;

        preprocessed_file["towers"].push_back(tower);
    }

    preprocessed_file["paths"] = nlohmann::ordered_json::array();

    for (size_t i = 0; i < paths_count; i++) {
        nlohmann::ordered_json path;
        path["id"] = tower_paths[i].id;
        path["distance"] = tower_paths[i].distance;
        path["elevation"] = tower_paths[i].elevation;

        preprocessed_file["paths"].push_back(path);
    }

    std::ofstream result_file(output_file);
    if (!result_file.is_open()) {
        print("Save error: Failed to open result file for writing preprocessed data.", RED, STDERR);
        exit(1);
    }
    
    result_file << preprocessed_file.dump(4);
    result_file.close();

    print("Preprocessed data has been saved (file '" + output_file + "').", GREEN, STDOUT);
}

std::string format_coordinate(double coordinate) {
    std::ostringstream formated_coordinate;
    formated_coordinate << std::fixed << std::setprecision(6) << coordinate;
    return formated_coordinate.str();
}

void save_best_solution(std::string output_file, std::vector<Tower_t>& best_solution) {
    size_t solution_count = best_solution.size();
    pugi::xml_document solution;

    pugi::xml_node declaration = solution.append_child(pugi::node_declaration);
    declaration.append_attribute("version") = "1.0";
    declaration.append_attribute("encoding") = "UTF-8";

    pugi::xml_node gpx = solution.append_child("gpx");
    gpx.append_attribute("version") = "1.1";
    gpx.append_attribute("creator") = "Tower Challenge";
    gpx.append_attribute("xmlns") = "http://www.topografix.com/GPX/1/1";

    for (size_t tower = 0; tower < solution_count; ++tower) {
        pugi::xml_node wpt = gpx.append_child("wpt");
        wpt.append_attribute("lon") = format_coordinate(best_solution[tower].gps.first).c_str();
        wpt.append_attribute("lat") = format_coordinate(best_solution[tower].gps.second).c_str();
        wpt.append_child("name").text().set(best_solution[tower].name.c_str());
    }

    pugi::xml_node trk = gpx.append_child("trk");
    trk.append_child("name").text().set("Solution");

    pugi::xml_node trkseg = trk.append_child("trkseg");
    for (size_t tower = 0; tower < solution_count; ++tower) {
        pugi::xml_node trkpt = trkseg.append_child("trkpt");
        trkpt.append_attribute("lon") = format_coordinate(best_solution[tower].gps.first).c_str();
        trkpt.append_attribute("lat") = format_coordinate(best_solution[tower].gps.second).c_str();
    }

    if (!solution.save_file(output_file.c_str(), "  ")) {
        print("Save error: Failed to generate GPX file.", RED, STDERR);
        exit(1);
    }

    print("Solution has been generated (file '" + output_file + "').", GREEN, STDOUT);
}

void save_refined_solution(std::string output_file, std::vector<Tower_t>& towers, std::vector<std::pair<double, double>>& path_coordinates) {
    size_t towers_count = towers.size();
    size_t path_points_count = path_coordinates.size();

    pugi::xml_document refined_solution;

    pugi::xml_node declaration = refined_solution.append_child(pugi::node_declaration);
    declaration.append_attribute("version") = "1.0";
    declaration.append_attribute("encoding") = "UTF-8";

    pugi::xml_node gpx = refined_solution.append_child("gpx");
    gpx.append_attribute("version") = "1.1";
    gpx.append_attribute("creator") = "Tower Challenge";
    gpx.append_attribute("xmlns") = "http://www.topografix.com/GPX/1/1";

    for (size_t tower = 0; tower < towers_count; ++tower) {
        pugi::xml_node wpt = gpx.append_child("wpt");
        wpt.append_attribute("lon") = format_coordinate(towers[tower].gps.first).c_str();
        wpt.append_attribute("lat") = format_coordinate(towers[tower].gps.second).c_str();
        wpt.append_child("name").text().set(towers[tower].name.c_str());
    }

    pugi::xml_node trk = gpx.append_child("trk");
    trk.append_child("name").text().set("Refined Solution");

    pugi::xml_node trkseg = trk.append_child("trkseg");
    for (size_t path_point = 0; path_point < path_points_count; ++path_point) {
        pugi::xml_node trkpt = trkseg.append_child("trkpt");
        trkpt.append_attribute("lon") = format_coordinate(path_coordinates[path_point].first).c_str();
        trkpt.append_attribute("lat") = format_coordinate(path_coordinates[path_point].second).c_str();
    }

    if (!refined_solution.save_file(output_file.c_str(), "  ")) {
        print("Save error: Failed to generate refined GPX file.", RED, STDERR);
        exit(1);
    }

    print("Refined solution has been generated (file '" + output_file + "').", GREEN, STDOUT);
}