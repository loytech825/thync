#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>

#include "config.h"
#include "helpers.h"
#include "color256.h"
#include <cstring>
#include <unordered_map>


/*Global settings*/
std::string output_dir{""};
std::string color_file{""};
//bool generate_pallete{false};
bool preview{false}; 
bool preview_gen{false};

namespace fs = std::filesystem;

void print_cfg(const ConfigSection& section_config, const ConfigSection& global_colors,  const ConfigSection& section_colors, const Palette& palette, std::ostream& stream);
std::string format_value(const std::string& format, const std::string& id_format, const std::string& key, const std::string& value);
int parse_arguments(const int argc, char* argv[]);

int main(int argc, char *argv[])
{
    //check if arguments parse successfully
    if(parse_arguments(argc, argv)) return -1;

    if(color_file.empty()) { std::cout << "No file provided!\nDo thync -h for help\n"; return -1; }
    
    //TODO verbose?

    output_dir = fs::absolute(fs::path(output_dir)).lexically_normal();


    fs::path config_dir{get_config_dir()};
    fs::path config_path{config_dir / APP_NAME / "config.conf"};

    create_conf(config_path);

    /*
     *
     * Test area
     *
     */



    //return 0;
    /*
        CONFIG FILE PARSING
    */

    std::vector<ConfigSection> config_vec = parse_config(fs::path{config_path});

    ///default section guaranteed to exist

    //setup config data structure
    Config config;
    config.global = config_vec.at(0);
    config.sections.insert(config.sections.begin(), config_vec.begin()+1, config_vec.end());

    
    process_config(config, output_dir, config_dir);

    //we need to keep a reference of the original argument value
    std::string default_color_loc = color_file;
    //check default file locations
    if(config.global.key_value_pairs.contains("theme_dir"))
    {
        fs::path theme_dir{config.global.key_value_pairs.at("theme_dir")};
        color_file = fs::absolute(fs::path(theme_dir) / color_file).lexically_normal();
    }
   

    if(!config.global.key_value_pairs.contains("theme_dir") || !fs::exists(color_file))
    {
        std::cout << "File not found in default dir, checking relative...\n";
        color_file = fs::absolute(fs::path(default_color_loc)).lexically_normal();
        std::cout << color_file << "\n";
    }


    //check if defaults exist for each section
    std::vector<ConfigSection> defaults;

    //since defaults are stored as separate strings, we need to process them separately

    //global
    if(config.global.key_value_pairs.contains("defaults"))
    {
        ConfigSection parsed = parse_config(config.global.key_value_pairs.at("defaults"))[0];
        if(!parsed.key_value_pairs.empty())
        {
            parsed.name = config.global.name;
            defaults.emplace_back(parsed);
        }
    }

    //local
    for(auto& sect : config.sections)
    {
        if(sect.key_value_pairs.contains("defaults"))
        {
            ConfigSection parsed = parse_config(sect.key_value_pairs.at("defaults"))[0];
            if(!parsed.key_value_pairs.empty())
            {
                parsed.name = sect.name;
                defaults.emplace_back(parsed);
            }
        }

    }


    /*
        COLOR PARSING
    */   
    if(!fs::is_regular_file(color_file)) { std::cout << "File " << fs::absolute(color_file) << " doesn't exist!\n"; return -1; }
    
    std::vector<ConfigSection> config_colors = parse_config(fs::path{color_file});
    


    //here we insert defaults if present
    for(auto& color_sect : config_colors)
    {
        //first merge globals before merging defaults
        //otherwise default overrides global
        auto WHY = config_colors.at(0).key_value_pairs;
        color_sect.key_value_pairs.merge(WHY);

        if(auto it = std::find_if(defaults.begin(), defaults.end(), [&color_sect](const ConfigSection& cfg){ return cfg.name == color_sect.name;}); 
        it != defaults.end())
        {
           //std::cout << "found defaults for " << it->name << "\n";
           
           color_sect.key_value_pairs.merge(it->key_value_pairs);
        }

        //std::cout << color_sect << "\n";
    }
    


    //set up color data structure 
    ColorConfig colors = process_colors(config_colors);

    if(preview) { print_colors(colors); std::cout << "\n\n"; return 0; }
    if(preview_gen) { print_256(colors.palette); std::cout << "\n\n"; return 0; }




    /*
        Prints colors to files
    */
    for(auto& sect : config.sections)
    {

        ConfigSection to_print;
        to_print.name = sect.name;

        //check if colors has a special section
        if(auto loc = std::find_if(colors.sections.begin(), colors.sections.end(), [&sect](const ConfigSection& int_sect){ return int_sect.name == sect.name; }); loc != colors.sections.end()) to_print = *loc;
        else continue;

        if(sect.name != "default")
        {
            fs::path path{sect.key_value_pairs["path"]};
            path = path.lexically_normal();

            //config specifies filename not directory
            if(path.has_extension())
            {
                if(!fs::exists(path.parent_path())) fs::create_directories(path.parent_path());
                if(path.extension() == ".noext") path = path.replace_extension("");
            }else
            {
                if(!fs::exists(path)) fs::create_directories(path);
                if(fs::is_directory(path)) path /= "colors.conf";
            }

            std::cout << "Writing section "  << sect.name << " to " << fs::absolute(path).lexically_normal() << "\n";
           
            //std::cout << to_print << "\n";
            //if section has a file field, we just copy the file over to path
            if(to_print.key_value_pairs.contains("file"))
            {
                fs::path copy_from{to_print.key_value_pairs.at("file")};

                if(!fs::exists(copy_from))
                {
                    std::cout << "File " << copy_from.lexically_normal() << " not found, skipping\n";
                }
                else
                {
                    fs::copy(copy_from, path, std::filesystem::copy_options::overwrite_existing);
                    std::cout << "\t\"file\" field found, copying from " << copy_from.lexically_normal() << "\n";
                }
                continue;
            }


            std::ofstream file{path};
            print_cfg(sect, colors.global, to_print, colors.palette, file);
            file.close();
        }
    }

    //execute post command
    if(config.global.key_value_pairs.contains("post_cmd"))
    {
        int exit = std::system(config.global.key_value_pairs.at("post_cmd").c_str());
        //std::cout << config.global.key_value_pairs.at("post-cmd") << "\n";
        if(exit) std::cout << "Post execution command failed! " << exit << "\n";
    }
}

/*
    Parse arguments and setup variables
*/
int parse_arguments(const int argc, char* argv[])
{
    //if passing setting via -C, -f

    output_dir = get_config_dir();

    //if(argc == 1) { std::cout << "Invalid usage:\n" << HELP; return -1; };
    //if(argc == 2) { color_file = argv[1]; return 0;}
    //if(argc == 3) { color_file = argv[1]; output_dir = argv[2]; return 0;}

    for(int i = 1; i < argc; i++)
    {
        //boolean arguments
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) { std::cout << HELP; continue;}
        if(strcmp(argv[i], "--preview") == 0)           { preview = true; continue;}
        if(strcmp(argv[i], "-p") == 0)                  { preview = true; continue;}
        //if(strcmp(argv[i], "g") == 0) { generate_pallete = true; continue;}
        if(strcmp(argv[i], "--preview-gen") == 0)       { preview_gen = true; continue;}
        if(strcmp(argv[i], "-P") == 0)                  { preview_gen = true; continue;}

        //arguments that require another field
        if(i < argc-1)
        {
            if(strcmp(argv[i], "-o") == 0) { output_dir = argv[i+1]; i++; continue; }
            if(strcmp(argv[i], "-f") == 0) { color_file = argv[i+1]; i++; continue; }  
        }

        //none of the options match
        //the first non option argument is color_file, the second is output_dir
        if( color_file.empty() )    { color_file = argv[i]; continue; }
        else if( output_dir.empty() )  { output_dir = argv[i]; continue; }

        else { std::cout << "Unknown argument: " << argv[i] << "! " << "do thync -h for help.\n"; return -1;};
    }

    return 0;
}
/*
    This actually prints the colors into a file
*/
void print_cfg(const ConfigSection& section_config, const ConfigSection& global_colors,  const ConfigSection& section_colors, const Palette& palette, std::ostream& stream)
{

    //all of these values should have benn configured in config.cpp: process_config
    std::string format = section_config.key_value_pairs.at("format");
    std::string id_format = section_config.key_value_pairs.at("format_id");
    std::string comment = section_config.key_value_pairs.at("comment");
    std::string config_format = section_config.key_value_pairs.at("config_format");
    
    //whether to modify-add or modify-ignore existing values in the config
    std::string mode = section_config.key_value_pairs.at("mode");

    auto col_global = global_colors.key_value_pairs;
    auto col_section = section_colors.key_value_pairs;


    std::vector<std::string> already_printed;

    //std::cout << "Defaults: " << col_global << "\n";
    //std::cout << "Section: " << col_section << "\n";

    //unifies fields
    //col_section.merge(col_global);
    
    /*for(const auto& [k, v] : col_section)
    {
        std::cout << "\t" << k << ":\t" << v << "\n";
    }*/

    const auto lines = split_string(config_format, "\n");

    /*
        Loop through config_format to set a more organized order of final file
    */
    for(const auto& line : lines)
    {

        //this will be the line to check for anything since we can remove whitespace and make life easier
        if(line.empty()) { stream << "\n"; continue; }
        std::string check_line = line;

        //remove leading and trailing space
        int line_begin = line.find_first_not_of(" \t");
        int line_end = line.find_last_not_of(" \t");

        if(line_begin == std::string::npos) continue;
        if(line_begin > line_end) continue;

        check_line = line.substr(line_begin, line_end-line_begin+1);

        //split line into words
        auto words = split_string(check_line, " ");



        //split lines at tabs also
        std::vector<std::string> words_temp;
        for(auto& word : words)
        {
            auto temp = split_string(word, "\t");
            words_temp.insert(words_temp.end(), temp.begin(), temp.end());
        }
        
        words.insert(words.end(), words_temp.begin(), words_temp.end());

        //remove quotes (to get key if config is json)
        for(auto& word : words)
        {
            if(word.length() < 2) continue;
            if(word.starts_with('"') && word.ends_with('"')) word = word.substr(1, word.length()-2);
        }


        //insert a comment
        if(line.starts_with("#")) 
        {

            if(comment.empty()) continue;

            std::string out = line.substr(1);
            out = replace_all(out, "${section_name}", section_config.name); 
            out = replace_all(out, "${output_dir}", output_dir);
            out = replace_all(out, "${path}", section_config.key_value_pairs.at("path")); 

            stream << comment << line.substr(1) << "\n";
        }
        else if(line.starts_with("\n")) { stream << line; }

        //this also covers the case where the entire line is key
        //check if a key is overriden
        //FIXME: THIS ASSUMES FIRST WORD IS KEY!
        else if(col_section.contains(words[0]))
        {
            stream << format_value(format, id_format, words[0], col_section[words[0]]) << "\n";
            already_printed.emplace_back(words[0]);
        }
        //check if we have a key
        else if(int begin = line.find("${key="); begin != std::string::npos)
        {
            int end = line.find("}", begin+6);
            if(end == std::string::npos) continue;

            //here we have a valid ${key=abdec}
            std::string key = line.substr(begin+6, end-begin-6);

            if(col_section.contains(key))
            {
                stream << format_value(format, id_format, key, col_section[key]) << "\n";
                already_printed.emplace_back(key);
            }

        }
        //here we check if line contains the entire format
        //if it contains a token, its safe to assume its a formatted line
        else if(line.find("${") != std::string::npos)
        {
            /*
                Variable replace (TODO factor this into a function)
            */
            std::string out_line = insert_variables(line, col_section, palette);

            //if variables couldn't be replace we skip the line to avoid random stuff
            if(out_line.find("${") != std::string::npos)
                out_line = "";

            stream << out_line << "\n";
        }
        else
        {
            stream << line << "\n";
        }
    }

    /*
        Loop through all the fields
        If color is one of the indexed ones (black, red, ...)
        Or provided as colorXXX where XXX is a number,
        Use alternate format

        ONLY ADD IF MODE IS SET TO MODIFY ADD (for vscode, we dont want this)
    */
    if(mode == "modify-add")
    {
        for(const auto [k, v] : col_section)
        {
            if(std::find(already_printed.begin(), already_printed.end(), k) != already_printed.end()) continue;
            stream << format_value(format, id_format, k, v) << "\n";
        }
    }
}

std::string format_value(const std::string& format, const std::string& id_format, const std::string& key, const std::string& value)
{

    std::string formatted = format;

    //std::cout << key << ": " << value << "\n";
    //if key is a number
    try
    {
        int id = stoi(key);

        //if its a bae color (0-255) we use the other format;
        formatted = id_format;

        if(id >= 0 && id <= 15)
            formatted = replace_all(formatted, "${color_name}", COLORS[id]);

    }catch(std::exception e) {}

    formatted = replace_all(formatted, "${color_name}", key);
    formatted = replace_all(formatted, "${color_id}", key);
    formatted = replace_all(formatted, "${color_value}", value);

    return formatted;
}
