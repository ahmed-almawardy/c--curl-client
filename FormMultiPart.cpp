//
// Created by white-inside on 7/3/24.
//

#include "FormMultiPart.h"

#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include <unistd.h>

void FormMultiPart::add_file(
    const char buffer[],
    const long size
) {
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, "file", // <--- the (in this case) wanted file-Tag!
                 CURLFORM_BUFFER, "data",
                 CURLFORM_BUFFERPTR, buffer,
                 CURLFORM_BUFFERLENGTH, size,
                 CURLFORM_END);
}

void FormMultiPart::add_param(
    const std::string header_name,
    const std::string header_value
) {
    curl_formadd(&formpost,
                 &lastptr,
                 CURLFORM_COPYNAME, header_name.c_str(),
                 CURLFORM_COPYCONTENTS, header_value.c_str(),
                 CURLFORM_END);
}

void FormMultiPart::post(
    const char buffer[],
    const long size
) {
    key_str = std::to_string(key);
    add_file(buffer, size);
    add_param("status", "online");
    add_param("user_key", std::to_string(key));
    add_param("machine", machine);
    add_param("user", user_name);
    headerlist = curl_slist_append(headerlist, buffer);
    request_post(url);
}

FormMultiPart::FormMultiPart()
    : url {"http://localhost:8000/"},
    curl {curl_easy_init()},
    user_name {},
    machine {}
{
    curl_global_init(CURL_GLOBAL_ALL);
    generate_user_key();
}


void FormMultiPart::generate_user_key() {
    std::string key{};
    gethostname(machine, sizeof(machine));
    getlogin_r(user_name, sizeof(user_name));
    key+=machine;
    key+=user_name;
    this->key=std::hash<std::string>{}(key);
}


FormMultiPart::~FormMultiPart() {
    cleanup();
}


void FormMultiPart::cleanup() {
    curl_easy_cleanup(curl);
    curl_formfree(formpost);
    curl_slist_free_all(headerlist);
}

void FormMultiPart::request_post(std::string url) {
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_perform(curl);
    }
}

void FormMultiPart::post_status(const char buffer[], long buffer_size) {
    std::string url_user {url + key_str};
    while (1) {
        add_file(buffer, buffer_size);
        headerlist = curl_slist_append(headerlist, buffer);
        request_post(url_user);
        sleep(5);
    }
}