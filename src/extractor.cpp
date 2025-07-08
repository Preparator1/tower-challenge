#include "main.hpp"
#include <pugixml.hpp>

std::vector<Place_t> extract_map(std::string file_path) {
    pugi::xml_document kml_file;
    if (!kml_file.load_file(file_path.c_str())) {
        std::cerr << "Failed to load KML file\n";
        exit(1);
    }

    pugi::xml_node folder = kml_file.child("kml").child("Document").child("Folder");

    std::string name;
    std::string style;
    uint8_t category;
    std::string gps;

    std::regex pattern("[\\d]+\\.[\\d]+");
    double longtitude, latitude;

    std::vector<Place_t> places;
    uint identifier = 1;

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

        longtitude = std::stod(gps_coordinates[0]);
        latitude = std::stod(gps_coordinates[1]);

        Place_t place(identifier, name, category, std::make_pair(longtitude, latitude));
        places.push_back(place);

        identifier++;
    }

    return places;
}