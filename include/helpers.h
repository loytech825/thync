#pragma once

/*
This file include random helper functions
*/

#include <unordered_map>
#include <vector>
#include "constants.h"
#include "config_parser.h"
#include "color256.h"

std::string replace_all(const std::string& orig, const std::string& from, const std::string& to);
std::string get_config_dir();
int get_color_id(const std::string& color);
std::vector<std::string> split_string(const std::string& input, const std::string& token);

/*
    @brief Replaces all occurences of ${var_name}
    @param input string
    @param variables_source where to search for variables 
*/
std::string insert_variables(const std::string& input, const std::unordered_map<std::string, std::string>& variables_source);

/*
    @brief Replaces all occurences of ${var_name}, taking into account generated colors
    @param input string
    @param variable_source primary variable source
    @param gen_colors a Palette of colors if `variable_source` doesn't have a variable
*/
std::string insert_variables(const std::string& input, const std::unordered_map<std::string, std::string>& variables_source, Palette gen_colors);

/*
 *
 * @brief Inserts and files
 *
 */

std::string insert_variables(const std::string& input, const std::filesystem::path& file_search_dir);
