/*
    Source code for config.h for more info see that
*/
#include "config.h"
#include "helpers.h"
#include <iostream>


/*
    Processes the config
    merges with global
    replaces tokens
    initializes missing fields...
*/
void process_config(Config& config, const std::string& output_dir, const std::string& config_dir)
{
    //parse all that can be parsed here
    for(auto& [k, v] : config.global.key_value_pairs)
    {
        if(k == "post-cmd")
        {
            v = replace_all(v, "${section_name}", config.global.name);
        }
        v = replace_all(v, "${config_dir}", config_dir);
        v = replace_all(v, "${output_dir}", output_dir);

        if(k == "format" || k == "format_id" || k == "config_format" || k == "defaults") { v = insert_variables(v, std::filesystem::path(config_dir) / "thync"); }
        else    { v = insert_variables(v, config.global.key_value_pairs); }
    }


    for(auto& section_config : config.sections)
    {
        
        //if section has defined only format, we need to override format_id as well
        if(section_config.key_value_pairs.contains("format") && !section_config.key_value_pairs.contains("format_id")) { section_config.key_value_pairs["format_id"] = section_config.key_value_pairs["format"]; }
        
        //since merge messes up stuff
        auto cfg_global = config.global.key_value_pairs;
        section_config.key_value_pairs.merge(cfg_global);

        //initializes missing fields
        if(!section_config.key_value_pairs.contains("path"))        { section_config.key_value_pairs["path"]            = DEFAULT_PATH;                               }
        if(!section_config.key_value_pairs.contains("format"))      { section_config.key_value_pairs["format"]          = DEFAULT_FORMAT;                             } 
        if(!section_config.key_value_pairs.contains("format_id"))   { section_config.key_value_pairs["format_id"]       = section_config.key_value_pairs["format"];   }
        if(!section_config.key_value_pairs.contains("comment"))     { section_config.key_value_pairs["comment"]         = DEFAULT_COMMENT;                            }
        if(!section_config.key_value_pairs.contains("config_format")){section_config.key_value_pairs["config_format"]   = DEFAULT_CONFIG_FORMAT;                      }
        if(!section_config.key_value_pairs.contains("mode"))        { section_config.key_value_pairs["mode"]            = DEFAULT_MODE;                               }

        //checks if mode is valid
        if(!(section_config.key_value_pairs["mode"] != "modify-add" 
            ||  section_config.key_value_pairs["mode"] != "modify-ignore")) { section_config.key_value_pairs["mode"] = DEFAULT_MODE; }

        section_config.key_value_pairs.erase("color_name");
        section_config.key_value_pairs.erase("color_id");
        section_config.key_value_pairs.erase("color_value");

        //replace all tokens
        for(auto& pair : section_config.key_value_pairs)
        {
            auto& v = pair.second;
            v = replace_all(v, "${section_name}", section_config.name);
            v = replace_all(v, "${config_dir}", config_dir);
            v = replace_all(v, "${output_dir}", output_dir);
        }
        //replaces variables
        for(auto& [k, v] : section_config.key_value_pairs){


            if(k == "format" || k == "format_id" || k == "config_format" || k == "defaults") { v = insert_variables(v, std::filesystem::path(config_dir) / "thync"); }
            else    { v = insert_variables(v, section_config.key_value_pairs); }

            
        }
        // NEED TI ADD FILE INSERTION
    }
}


/*
    Changes #RRGGBB into RRGGBB
    and color names to ids where possible
*/
ConfigSection prepare_section(const ConfigSection& config)
{

    ConfigSection out;

    for(const auto& [k, v] : config.key_value_pairs)
    {

        std::string key = k;
        std::string color = v;
        if( int id = get_color_id(k); id != -1)
        {
            //this code changes a key in the map
            key = std::to_string(id);
        }

        //is v is of type #rrggbb or #rrggbbaa strip #
        //for now only parses RRGGBB
        if(v.starts_with("#") && (v.length() == 7 || v.length() == 9) && (v.find("${") == std::string::npos)) { color = v.substr(1, 6); }

        out.key_value_pairs[key] = color;
    }

    out.name = config.name;

    return out;
}

/*
    changes color names to id where possible
    (note: black=0, red=1...bright_black=8, bright_red=9...)
*/
ColorConfig process_colors(std::vector<ConfigSection> config)
{

    //we have key value pairs
    //first we check if global contains the base 8 colors
    //if it does, we generate the pallete

    //then we change the base colors to RGB
    //then we replace variables

    ColorConfig out;

    //prepares sections by changing names to id's and colors to RRGGBB
    for(auto& sect : config) sect = prepare_section(sect);
    
    auto global_map = config.at(0).key_value_pairs;
    bool generated{false};

    //here all colors are id's and all values arre RRGGBB without #

    //we check if map contains colors 1-6 and (color 0 or background) and (color 7 or foreground)
    if(     global_map.contains("1") 
        &&  global_map.contains("2")
        &&  global_map.contains("3")
        &&  global_map.contains("4")
        &&  global_map.contains("5")
        &&  global_map.contains("6")
        &&  ( global_map.contains("0") || global_map.contains("background") )
        &&  ( global_map.contains("7") || global_map.contains("foreground") )
    )
    {
        //TODO: background / foreground and color0 / color7 parsing
        /*
            BASE COLOR INITIALIZATION
        */
        //we need a base16 for the color function, palette is separate from colors anyway
        //so we can populate the first 8 and leave the others empty
        std::array<Color, 16> base16;
        if(global_map.contains("background"))   base16[0] = hex2rgb(global_map.at("background"));
        else                                    base16[0] = hex2rgb(global_map.at("0"));

        base16[1] = hex2rgb(global_map.at("1"));
        base16[2] = hex2rgb(global_map.at("2"));
        base16[3] = hex2rgb(global_map.at("3"));
        base16[4] = hex2rgb(global_map.at("4"));
        base16[5] = hex2rgb(global_map.at("5"));
        base16[6] = hex2rgb(global_map.at("6"));

        if(global_map.contains("foreground"))   base16[7] = hex2rgb(global_map.at("foreground"));
        else                                    base16[7] = hex2rgb(global_map.at("7"));

        for(int i = 8; i < 16; i++)
        {
            if(global_map.contains(std::to_string(i))) base16[i] = hex2rgb(global_map.at(std::to_string(i)));
        }

        /*
            GENERATE AND MERGE PALETTE
        */
        out.palette = generate_256(base16, base16[0], base16[7]);
        generated = true;
    
    }
    else { std::cout << "Not enough default colors to generate a full pallete, skipping\n";}



    //second loop replaces variables
    //this is separate because if a default color refers to a variable that gets
    //overridden in a section, it wont work


    for(auto it = config.begin(); it != config.end(); it++)
    { 
        auto& sect = *it;

        //globals and defaults merged already
        for(auto& [k, v] : sect.key_value_pairs)
        {
            v = insert_variables(v, sect.key_value_pairs, out.palette); 
        }
    }

    out.global = config.at(0);
    out.sections.insert(out.sections.begin(), config.begin() + 1, config.end());

    return out;
}
