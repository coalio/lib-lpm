#include "requests.h"
#include "macros.h"

// Perform a GET request and return a Response object
size_t LPM::Requests::write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    Response* response = static_cast<Response*>(userdata);
    response->body.append(ptr, size * nmemb);
    return size * nmemb;
}

LPM::Requests::Response LPM::Requests::get(std::string url, CURL* curl_handle) {
    Response response;
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "lpm-agent/1.0");
    curl_easy_perform(curl_handle);

    // Get status code
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response.status_code);

    return response;
}