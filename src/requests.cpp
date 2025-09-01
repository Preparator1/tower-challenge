#include "main.hpp"
#include <curl/curl.h>

int index_mapping(size_t towers_count, size_t lower_index, size_t higher_index) {
    if (lower_index > higher_index)
        std::swap(lower_index, higher_index);

    return towers_count * lower_index + higher_index - (lower_index * (lower_index + 1) / 2) - (lower_index + 1);
}

nlohmann::json get_osrm_data(std::vector<Tower_t>& towers, size_t tower_1, size_t tower_2) {
    std::string request_string(OSRM_SERVER"/route/v1/walking/");
    request_string.append(std::to_string(towers[tower_1].gps.first) + "," + std::to_string(towers[tower_1].gps.second) + ";");
    request_string.append(std::to_string(towers[tower_2].gps.first) + "," + std::to_string(towers[tower_2].gps.second) + "?");
    request_string.append("overview=simplified&geometries=geojson");

    nlohmann::json request_data = request(request_string);
    if (request_data["code"] != "Ok") {
        print("Request error: " + std::string(request_data["message"]) + " (" + std::string(request_data["code"]) + ")", RED, STDERR);
        exit(1);
    }

    return request_data;
}

nlohmann::json get_otd_data(nlohmann::json& path_points) {
    size_t path_points_size = path_points.size();
    std::string request_string(OPENTOPODATA_SERVER"/v1/eudem25m?locations=");
    for (size_t point_id = 0; point_id < path_points_size; ++point_id) {
        request_string.append(std::to_string(path_points[point_id][1].get<double>()) + "," + std::to_string(path_points[point_id][0].get<double>()));
        if (point_id + 1 < path_points_size) request_string.append("|");
    }

    nlohmann::json request_data = request(request_string);
    if (request_data["status"] != "OK") {
        print("Request error: " + std::string(request_data["error"]) + " (" + std::string(request_data["status"]) + ")", RED, STDERR);
        exit(1);
    }

    return request_data;
}

size_t request_callback(void *contents, size_t size, size_t nmemb, std::string* userp) {
    size_t real_size = size * nmemb;
    userp->append((char *) contents, real_size);
    return real_size;
}

nlohmann::json request(std::string url) {
    CURL *curl_handler;
    CURLcode response;
    std::string buffer;
    nlohmann::json api_data;

    curl_global_init(CURL_GLOBAL_ALL);
    
    curl_handler = curl_easy_init();
    if (curl_handler) {
        curl_easy_setopt(curl_handler, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_handler, CURLOPT_WRITEFUNCTION, request_callback);
        curl_easy_setopt(curl_handler, CURLOPT_WRITEDATA, &buffer);

        response = curl_easy_perform(curl_handler);

        if (response != CURLE_OK) {
            print("cURL error: " + std::string(curl_easy_strerror(response)), RED, STDERR);
            exit(1);
        }

        try {
            api_data = nlohmann::json::parse(buffer);
        } catch(nlohmann::json::parse_error& error) {
            print("JSON parse error: " + std::string(error.what()), RED, STDERR);
            exit(1);
        }
        
        curl_easy_cleanup(curl_handler);
    }

    curl_global_cleanup();

    return api_data;
}