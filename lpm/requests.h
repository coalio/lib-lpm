#pragma once
#include <curl/curl.h>
#include <string>

namespace LPM::Requests {
    size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

    class Response {
    public:
        Response() {}

        Response(
            std::string _body,
            std::string _url,
            int _status_code
        ) : body(_body),
            url(_url),
            status_code(_status_code) {}

        std::string body, url;
        int status_code;
    };

    Response get(std::string url, CURL* curl_handle);
}