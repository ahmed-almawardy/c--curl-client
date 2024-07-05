//
// Created by white-inside on 7/3/24.
//

#ifndef FORMMULTIPART_H
#define FORMMULTIPART_H

#include <string>
#include <curl/curl.h>


class FormMultiPart {
    CURL *curl {nullptr};
    std::string url {};
    struct curl_httppost *formpost{nullptr};
    struct curl_httppost *lastptr{nullptr};
    struct curl_slist *headerlist{nullptr};
    char user_name[250];
    char user_host[250];
    char machine[250];
    void generate_user_key();
    size_t key;
    std::string key_str;

    public:
        void add_file(
             const char buffer[],
             const long filesize
             );

        void request_post(std::string);
        void post_status(const char buffer[], long buffer_size);
        void post(
            const char buffer[], long buffer_size);
        void add_param(
                const std::string header_name, const std::string header_value);
        void cleanup();
        FormMultiPart();
        ~FormMultiPart();
};


#endif //FORMMULTIPART_H
