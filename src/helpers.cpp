#include "helpers.h"
#include "color256.h"
#include <exception>
#include <filesystem>
#include <fstream>
#include <pwd.h>
#include <string>
#include <unistd.h>
#include <iostream>

/*
    @brief replaces all occurences of `from` to `to` in a given `orig` DOES NOT MODIFY THE STRING, BUT RETURNS A NEW ONE 
*/
std::string replace_all(const std::string& orig, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;

    std::string formatted = orig;

    while(true)
    {
        start_pos = formatted.find(from, start_pos);
        if(start_pos == std::string::npos) break;

        formatted.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return formatted;
}

/*
    Returns the root folder for configs (such as ~/.config)
    to `dir`
*/
std::string get_config_dir()
{

    char* config_dir;
    std::string dir;

    //check for XDG_CONFIG first
    if((config_dir = getenv("$XDG_CONFIG_HOME")) != NULL)
    {
        return config_dir;
    }

    //then check HOME env
    if((config_dir = getenv("HOME")) != NULL)
    {
        dir = config_dir;
        dir += "/.config";
        return dir;
    }

    //neither env have been found
    config_dir = getpwuid(getuid())->pw_dir;
    dir = config_dir;
    dir += "/.config";

    return dir;
}

/*
    Turns color name to id, returns -1 if fails
    id can be higher than 15!
*/
int get_color_id(const std::string& color)
{

    if(color.starts_with("color"))
    {
        try{
            int id = stoi(color.substr(5));
            return id;
        }catch(std::exception e)
        {
            return -1;
        }
    }

    for(int i = 0; i < 16; i++)
    {
        if(COLORS[i] == color)  return i;
    }

    return -1;
}

std::vector<std::string> split_string(const std::string &input, const std::string &token)
{

    std::vector<std::string> out;

    int pos = -1;
    int last_pos = 0;
    while((pos = input.find(token, pos+1)) != std::string::npos)
    {
        out.push_back(input.substr(last_pos, pos-last_pos));
        last_pos = pos+1;
    }

    //last line
    out.push_back(input.substr(last_pos));

    return out;
}

std::string insert_variables(const std::string& input, const std::unordered_map<std::string, std::string>& variables_source)
{
    std::string out_line = input;


    /*std::cout << "NEW CALL\n";
    for(auto [k, v] : variables_source)
    {
        std::cout << k << ":\t" << v << "\n———————\n";
    }*/
    for(const auto& [k, v] : variables_source)
    {
        out_line = replace_all(out_line, "${" + k + "}", v);
    }

    /*int begin = 0;
    while(true)
    {
        begin = out_line.find("${", begin);
        if(begin == std::string::npos) break;

        //std::cout << out_line << "\n";
        //loop through closing brackets until one right after the opening is found
        int end = out_line.find("}", begin+2);
        if(end == std::string::npos) continue;

        //holds whats between ${ and }
        std::string v_name = out_line.substr(begin+2, end-begin-2);
        
        //holds the same as v_name unless v_name has id, in which case it holds the id
        std::string var_name = v_name;
        if(int id = get_color_id(v_name); id != -1) var_name = std::to_string(id);
        
        //replace the token
        if(variables_source.contains(var_name))
        {
            out_line = replace_all(out_line, "${"+v_name+"}", variables_source.at(var_name));
            //std::cout << out_line << "\n";
        }
        //if line is left in, we need to move search begin
        else
        {
            //std::cout << "\t" << ++begin << "\n";
            if(++begin > out_line.length()) break;
        }

        //std::cout << v_name << ", " << var_name << "\n";
    }*/
    return out_line;
}


std::string insert_variables(const std::string& input, const std::filesystem::path& file_search_dir)
{
    std::string out_line = input;

    //replace files

    int begin = 0;
    while(true)
    {
        begin = out_line.find("f{", begin);
        if(begin == std::string::npos) break;

        //std::cout << out_line << "\n";
        //loop through closing brackets until one right after the opening is found
        int end = out_line.find("}", begin+2);
        if(end == std::string::npos) continue;

        //holds whats between ${ and }
        std::string f_name = out_line.substr(begin+2, end-begin-2);
        
        auto file_to_insert = file_search_dir / f_name;

        if(std::filesystem::exists(file_to_insert) && !std::filesystem::is_directory(file_to_insert))
        {
            std::ifstream config_file{file_to_insert};
            
            auto size = std::filesystem::file_size(file_to_insert);
            std::string content(size, '\0');
            config_file.read(&content[0], size);
            config_file.close();

            out_line = replace_all(out_line, "f{"+f_name+"}", content);
        }
        else
        {
            std::cout << "Couldn't insert file " << file_to_insert.lexically_normal() << "\n";
        }
        
        if(++begin > out_line.length()) break;

    }

    return out_line;

}

std::string insert_variables(const std::string &input, const std::unordered_map<std::string, std::string> &variables_source, Palette gen_colors)
{
    std::string out_line = input;

    for(const auto& [k, v] : variables_source)
    {
        out_line = replace_all(out_line, "${" + k + "}", v);
    }

    for(int i = 0; i < 256; i++)
    {
        out_line = replace_all(out_line, "${color" + std::to_string(i) + "}", rgb2hex(gen_colors[i]));
    }
    /*std::cout << "NEW CALL\n";
    for(auto [k, v] : variables_source)
    {
        std::cout << k << ":\t" << v << "\n———————\n";
    }*/

    /*int begin = 0;
    while(true)
    {
        begin = out_line.find("${", begin);
        if(begin == std::string::npos) break;

        //std::cout << out_line << "\n";
        //loop through closing brackets until one right after the opening is found
        int end = out_line.find("}", begin+2);
        if(end == std::string::npos) continue;

        //holds whats between ${ and }
        std::string v_name = out_line.substr(begin+2, end-begin-2);
        
        //holds the same as v_name unless v_name has id, in which case it holds the id
        std::string var_name = v_name;
        int id = get_color_id(v_name);
        if(id != -1) var_name = std::to_string(id);
        
        //std::cout << var_name << "\n";

        //TODO: HERE ALL VARIABLES ARE TRANSLATED INTO CODES
        //SO IF WE HAVE SOMETHING LKE main4, main here would be an ID so main4 can be processed according to that id
        
        //first check the variable source
        if(variables_source.contains(var_name)) { out_line = replace_all(out_line, "${"+v_name+"}", variables_source.at(var_name)); }


        //if id is valid and is not in primary colors, use generated ones
        else if(id != -1)   { out_line = replace_all(out_line, "${"+v_name+"}", rgb2hex(gen_colors.at(id))); }


        //if line is left in, we need to move search begin
        else { if(++begin > out_line.length()) break; }
    }*/

    return out_line;
}
