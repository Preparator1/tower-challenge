#include "main.hpp"

std::vector<Tower_t> extract_map(std::string map_path) {
    pugi::xml_document kml_file;
    if (!kml_file.load_file(map_path.c_str())) {
        print("Loading error: Failed to load KML map.", RED, STDERR);
        exit(1);
    }

    pugi::xml_node folder = kml_file.child("kml").child("Document").child("Folder");

    std::string name;
    std::string style;
    uint8_t category;
    std::string gps;

    std::regex pattern("[\\d]+\\.[\\d]+");
    double longitude, latitude;

    std::vector<Tower_t> towers;
    uint16_t identifier = 1;

    for (pugi::xml_node placemark : folder.children()) {
        if (std::string(placemark.name()) != "Placemark")
            continue;
            
        name = placemark.child("name").first_child().text().as_string();
        style = placemark.child("styleUrl").first_child().text().as_string();
        category = (style == "#icon-1899-0288D1-labelson-nodesc") ? 2 : 1;
        gps = placemark.child("Point").child("coordinates").first_child().text().as_string();
        
        auto begin = std::sregex_iterator(gps.begin(), gps.end(), pattern);
        auto end = std::sregex_iterator();

        std::vector<std::string> gps_coordinates;
        for (std::sregex_iterator iteration = begin; iteration != end; ++iteration)
            gps_coordinates.push_back(iteration->str());

        longitude = std::stod(gps_coordinates[0]);
        latitude = std::stod(gps_coordinates[1]);

        towers.emplace_back(identifier, name, category, std::make_pair(longitude, latitude));
        identifier++;
    }

    return towers;
}

Preprocessed_data_t extract_preprocessed_data(std::string data_path) {
    std::ifstream data_file(data_path);
    if (!data_file.is_open()) {
        print("Loading error: Failed to open JSON file.", RED, STDERR);
        exit(1);
    }

    nlohmann::json preprocessed_data;
    data_file >> preprocessed_data;

    uint16_t tower_id;
    std::string name;
    uint8_t category;
    double longitude, latitude;
    std::pair<double, double> gps;

    std::vector<Tower_t> towers;
    for (size_t i = 0; i < preprocessed_data["towers"].size(); ++i) {
        tower_id = preprocessed_data["towers"][i]["id"].get<uint16_t>();
        name = preprocessed_data["towers"][i]["name"].get<std::string>();
        category = static_cast<uint8_t>(preprocessed_data["towers"][i]["category"].get<int>());

        longitude = preprocessed_data["towers"][i]["gps"]["longitude"].get<double>();
        latitude = preprocessed_data["towers"][i]["gps"]["latitude"].get<double>();
        gps = std::make_pair(longitude, latitude);

        towers.emplace_back(tower_id, name, category, gps);
    }

    std::string path_id;
    uint32_t distance;
    uint32_t elevation;

    std::vector<Path_t> paths;
    for (size_t i = 0; i < preprocessed_data["paths"].size(); ++i) {
        path_id = preprocessed_data["paths"][i]["id"].get<std::string>();
        distance = preprocessed_data["paths"][i]["distance"].get<uint32_t>();
        elevation = preprocessed_data["paths"][i]["elevation"].get<uint32_t>();

        paths.emplace_back(path_id, distance, elevation);
    }

    return {towers, paths};
}

std::vector<Tower_t> extract_solution(std::string solution_path) {
    pugi::xml_document gpx_file;
    if (!gpx_file.load_file(solution_path.c_str())) {
        print("Loading error: Failed to load GPX solution.", RED, STDERR);
        exit(1);
    }

    pugi::xml_node gpx = gpx_file.child("gpx");

    std::string name;
    double longitude, latitude;
    std::vector<Tower_t> towers;

    for (pugi::xml_node wpt : gpx.children("wpt")) { 
        name = wpt.child("name").first_child().text().as_string();
        longitude = std::stod(wpt.attribute("lon").as_string());
        latitude = std::stod(wpt.attribute("lat").as_string());
        
        towers.emplace_back(UNDEFINED_IDENTIFIER, name, UNDEFINED_CATEGORY, std::make_pair(longitude, latitude));
    }
    
    return towers;
}