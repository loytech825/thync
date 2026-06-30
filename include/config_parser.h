#pragma once
#include <vector>
#include <unordered_map>
#include <filesystem>


//struct ProgramConfig
//{
//    std::string name;
//    std::string directory;
//    std::string format_16_color;
//    std::string format_default;
//};

/*
    When parsing a config file in the form
    [Section]
    key1=v1
    key2=v2, v3

    stores `name` = `"Section"`
    and `key_value_pairs` = 
    `{
        {"key1", "v1"},
        {"key2", "v2, v3"}
    `}
*/
struct ConfigSection
{
    std::string name;
    std::unordered_map<std::string, std::string> key_value_pairs;
};

/*
    Holds the data for a single section of the color file
*/
std::ostream& operator<<(std::ostream& os, const ConfigSection& cfg);

//parses a "line" of config
void parse_line(const std::string& line, std::vector<ConfigSection>& sections);
/*
 * @brief literally parses key value pairs from config files. Supports sections via [section_name]
 */
std::vector<ConfigSection> parse_config(const std::string& config);
std::vector<ConfigSection> parse_config(const std::filesystem::path& file_path);
/*
 * Creates a config at location `path` if it doesn't exist
 */
void create_conf(const std::filesystem::path& path);
