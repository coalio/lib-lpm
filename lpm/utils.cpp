#include <fstream>
#include <zip.h>
#include "utils.h"
#include "macros.h"

void LPM::Utils::format(
    std::string& format_str,
    std::map<std::string, std::string> args
) {
    // Replace ${key} with the value of the environment variable named key
    size_t start_pos = format_str.find("${");
    while (start_pos != std::string::npos) {
        size_t end_pos = format_str.find("}", start_pos);
        if (end_pos == std::string::npos) {
            break;
        }

        std::string key = format_str.substr(
            start_pos + 2,
            end_pos - start_pos - 2
        );

        std::string value = args[key];

        if (value == "") {
            throw std::runtime_error(
                "Failed to format string: " + format_str +
                " because key \"" + key + "\" is empty"
            );
        }

        format_str.replace(
            start_pos,
            end_pos - start_pos + 1,
            value
        );

        start_pos = format_str.find("${", start_pos + value.length());
    }
}

bool LPM::Utils::write_file(
    const std::string& path,
    const std::string& content
) {
    LPM_PRINT_DEBUG("Writing file " << path);
    // Split path into segments
    std::vector<std::string> segments = split(path, '/');

    // Print segments
    for (const auto& segment : segments) {
        LPM_PRINT_DEBUG("Segment: " << segment);
    }
    // Check if all segments except the last one exist
    for (size_t i =  0; i < segments.size() - 1; i++) {
        std::string segment = segments[i];
        if (segment == "") {
            continue;
        }

        std::string curr_path = join(segments, '/', 0, i);
        LPM_PRINT_DEBUG("Checking if directory " << curr_path << " exists");
        if (!fs::exists(curr_path)) {
            LPM_PRINT_DEBUG("Creating directory " << curr_path);
            if (!fs::create_directory(curr_path)) {
                LPM_PRINT_DEBUG("Failed to create directory " << curr_path);
                return false;
            }
        }
    }

    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }

    try {
        file << content;
    } catch (...) {
        LPM_PRINT_ERROR("Failed to write file " << path);
        return false;
    }

    file.close();

    LPM_PRINT_DEBUG("Wrote file " << path);

    return true;
}

bool LPM::Utils::unzip(
    zip* zip_file,
    const std::string& dest_path,
    std::string& error
) {
    char buffer[LPM_ZIP_BUFFER_SIZE];

    // Naturally, I'd use zip_file->nentry, but it's not available in the
    // zip.h header file.
    zip_uint64_t n_entries = static_cast<zip_uint64_t>(
        zip_get_num_entries(zip_file, 0)
    );

    // Iterate over all files in the zip file
    for (int i = 0; i < n_entries; i++) {
        struct zip_stat sb;

        if (zip_stat_index(zip_file, i, 0, &sb) != 0) {
            error =
                "Unable to retrieve file information from zip. (Index: " + std::to_string(i) + ")";

            return false;
        }

        struct zip_file* zip_file_entry = zip_fopen_index(zip_file, i, 0);
        if (!zip_file_entry) {
            error = "Failed to open zip file entry";

            return false;
        }

        // Get the file name
        const char* file_name = zip_get_name(zip_file, i, 0);
        if (!file_name) {
            error = "Failed to get zip file entry name";

            // Close the zip file entry
            zip_fclose(zip_file_entry);

            return false;
        }

        LPM_PRINT_DEBUG("Unzipping file: " << file_name);

        // Create the full path
        std::string file_path = dest_path + "/" + file_name;

        // Create an empty file at the path
        // (we use write_file because it handles directories)
         if (!write_file(file_path, "")) {
            error = "Failed to create file at path: " + file_path;

            // Close the zip file entry
            zip_fclose(zip_file_entry);

            return false;
         }

        // Open the file via index
        struct zip_file* current_file = zip_fopen_index(zip_file, i, 0);
        if (!current_file) {
            error = "Failed to open file at path: " + file_path;

            // Close the zip file entry
            zip_fclose(zip_file_entry);

            return false;
        }

        // Open a file stream to the file_path
        std::ofstream file(file_path, std::ios::app);
        if (!file.is_open()) {
            error = "Failed to open file: " + file_path;

            // Close the current file
            zip_fclose(current_file);

            return false;
        }

        // Transfer data from the zip file to the file stream
        int bytes_read = 0;
        while (
            // We cast buffer to a void pointer to allow libzip to perform
            // casting of the buffer to any other data type
            (bytes_read = zip_fread(current_file, static_cast<void*>(buffer), LPM_ZIP_BUFFER_SIZE))
            > 0
        ) {
            // Write the buffer to the file
            try {
                file.write(buffer, bytes_read);
            } catch (...) {
                error = "Failed to write to file: " + file_path;

                // Close the current file
                zip_fclose(current_file);

                return false;
            }
        }

        // Finally close the file
        file.close();

        // Close the current file
        zip_fclose(current_file);

        // Close the zip file entry
        zip_fclose(zip_file_entry);

        LPM_PRINT_DEBUG("Unzipped file: " << file_name);
    }

    return true;
}

std::vector<std::string> LPM::Utils::split(
    const std::string& str,
    char delimiter
) {
    std::vector<std::string> segments;

    size_t start_pos = 0;
    size_t end_pos = str.find(delimiter);
    while (end_pos != std::string::npos) {
        segments.push_back(str.substr(start_pos, end_pos - start_pos));

        start_pos = end_pos + 1;
        end_pos = str.find(delimiter, start_pos);
    }

    segments.push_back(str.substr(start_pos));

    return segments;
}

std::string LPM::Utils::join(
    std::vector<std::string>& segments,
    char delimiter,
    size_t start_index,
    size_t end_index
) {
    if (end_index == std::string::npos) {
        end_index = segments.size();
    }

    LPM_PRINT_DEBUG("Joining segments: " << start_index << " - " << end_index);

    std::string result = "";
    for (size_t i = start_index; i < end_index + 1; i++) {
        LPM_PRINT_DEBUG("Segment: " << segments[i]);
        result += segments[i];

        // Don't add the delimiter to the last segment
        if (i != end_index) {
            result += delimiter;
        }
    }

    return result;
}