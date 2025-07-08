#include "main.hpp"
#include <curl/curl.h>

std::string create_request_string(std::vector<Place_t> towers) {
    std::string request_string(OSRM_SERVER"/table/v1/walking/");

    for (size_t i = 0; i < 100 - 1; i++) {
        request_string.append(std::to_string(towers[i].gps.first) + "," + std::to_string(towers[i].gps.second));

        if (i < 100 - 2)
            request_string.append(";");
    }

    request_string.append("?annotations=distance");

    return request_string;
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
            std::cerr << "cURL error: " << curl_easy_strerror(response) << std::endl;
            exit(1);
        }

        try {
            api_data = nlohmann::json::parse(buffer);
        } catch(nlohmann::json::parse_error& error) {
            std::cerr << "JSON parse error: " << error.what() << std::endl;
            exit(1);
        }
        
        curl_easy_cleanup(curl_handler);
    }

    curl_global_cleanup();

    return api_data;
}