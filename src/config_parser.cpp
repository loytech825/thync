#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>
#include "config_parser.h"
#include "helpers.h"


std::ostream& operator<<(std::ostream& os, const ConfigSection& cfg)
{
    std::cout << "[" << cfg.name << "]\n";
    for(const auto& [k, v] : cfg.key_value_pairs)
    {
        os << k << " : " << v << "\n";
    }

    return os;
}

/*
    Parses a config string for now
*/
std::vector<ConfigSection> parse_config(const std::string& config)
{
    int index = -1;
    int last_word_index = 0;

    //are we currently parsing the key or the value
    bool is_key{true};
    //are we currently in parentheses
    bool in_string{false};

    std::vector<ConfigSection> sections;

    //loops thorugh each character in the file
    do
    {

        index++;

        //if we encounter parentheses, we swap in_string
        //unless parentheses are preceded by "\"
        if(config[index] == '\"' && !(index > 0 && config[index-1] == '\\')) { in_string = !in_string; }

        //end of line and we're not in string
        if( ((config[index] == '\n') && !in_string) || config[index] == '\0')
        {
            std::string line = config.substr(last_word_index, index-last_word_index);
            last_word_index = index+1;
            parse_line( line, sections );
        }

    }while(config[index] != '\0');

    return sections;

}

std::vector<ConfigSection> parse_config(const std::filesystem::path& file_path)
{
    std::ifstream config_file{file_path};
    
    auto size = std::filesystem::file_size(file_path);
    std::string content(size, '\0');
    config_file.read(&content[0], size);
    config_file.close();

    return parse_config(content);
}

void parse_line(const std::string& line, std::vector<ConfigSection>& sections)
{

    //std::cout << line << "\n";
    if(line.starts_with("#")) return;
    if(line.starts_with("\n")) return;
    if(line.empty()) return;
    

    //syntax for new section
    if(line.starts_with("[") && line.ends_with("]"))
    {
        sections.emplace_back();
        int first = line.find_first_not_of(" \t", 1);
        int last = line.find_last_not_of(" \t", line.length()-2);

        sections.back().name = line.substr(first, last-first+1);
        return;
    }

    if(sections.size() == 0)
    {
        sections.emplace_back();
        sections.back().name = "default";
    }

    //skip whitespace
    int pos = line.find_first_not_of(" \t");
    //std::cout << pos << " first nonwhite\n";
    
    if(line[pos] == '\n') return;

    //first word [stops at ' ', '\t' or '=']
    int start = pos;
    pos = line.find_first_of(" \t=", pos);
    std::string key = line.substr(start, pos-start);
    key = replace_all(key, "\\\"", "\"");


    //whitespace 2
    pos = line.find_first_not_of(" \t", pos); //while(line[pos] == ' ' || line[pos] == '\t') pos++;

    //check for =
    if(line[pos] != '=')
    {
        std::cout << line << ": Expected character \'=\' at pos " << pos << "; Character found: " << line[pos] << "\n";
        //for(int i = 0; i < line.length(); i++) std::cout << i << ": " << line[i] << "\n";
        //std::cout << "\n\n\n";
        return;
    }

    //skip whitespace
    pos++;
    pos = line.find_first_not_of(" \t", pos); //while(line[pos] == '\t' || line[pos] == ' ') pos++;

    //second word
    std::string value = line.substr(pos, line.find_last_not_of(" \t")-pos+1);
    value = replace_all(value, "\\\"", "\"");

    //remove enclosing ""
    if(value.starts_with("\"") && value.ends_with("\""))
    {
        value = value.substr(1, value.length()-2);
    }
    sections.back().key_value_pairs[key] = value;
}

void create_conf(const std::filesystem::path& path)
{
    //check if config folder exists
    if(!std::filesystem::exists(path.parent_path()) || !std::filesystem::is_directory(path.parent_path())) { std::filesystem::create_directories(path.parent_path()); }


    //create config file if missing
    if(!std::filesystem::is_regular_file(path))
    {
        std::cout << "No config found, generating!\n";

        std::ofstream generated_config{path};
        generated_config << DEFAULT_CONFIG;
        generated_config.close();
    }
}
