#pragma once

/*
    This file includes functions related to the main config file (config.conf) and not the color files
    They are separate from config_parser.h, as they deal with the internal structure only and are
    specific to this program
*/

#include "config_parser.h"
#include "color256.h"
/*
    A struct representing a parsed config file
    sections only holds section specific data
*/
struct Config
{
    ConfigSection global;
    std::vector<ConfigSection> sections;
};

/*
    Represents a parsed color file
    Sections only hold section specific data
*/
struct ColorConfig
{
    ConfigSection global;
    std::vector<ConfigSection> sections;
    Palette palette;
};

void process_config(Config& config, const std::string& output_dir, const std::string& config_dir); //, const std::string& theme_dir);
ColorConfig process_colors(std::vector<ConfigSection> config, const std::string& theme_dir);
