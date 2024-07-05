#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <fstream>

#include "FormMultiPart.h"

void load_bin_file(
    std::ifstream &file,
    char *buffer,
    long filesize
    );

void get_filesize(
    std::ifstream &file,
    long &filesize
);

void get_filesize(
    std::ifstream &file,
    long &filesize
) {
    file.seekg(0, std::ios::end);
    filesize = file.tellg();
    file.clear();
    file.seekg(0);
}


void load_bin_file(
    std::ifstream &file,
    char *buffer,
    long filesize
    ) {
    file.read(buffer, filesize);
}


int main() {
    // c++ auto start when os starts google
    // c++ take screen shot

    std::string current_path_name = get_current_dir_name();
    std::filesystem::path current_path{current_path_name};
    std::string filename{current_path.concat("/1.jpg")};
    std::ifstream file{filename, std::ios::binary};
    long filesize {};
    get_filesize(file, filesize);
    char *buffer {nullptr};
    buffer = new char[filesize];
    load_bin_file(file, buffer, filesize);
    file.close();

    FormMultiPart form_data;
    form_data.post(buffer, filesize);
    form_data.post_status(buffer, filesize);

    // ClientSOC socket_client;
    // socket_client.run(buffer, filesize);


    delete [] buffer;
    return EXIT_SUCCESS;
}
