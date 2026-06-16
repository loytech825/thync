# Config

Config files are split into [Configuration files](#configuration-files) and [Color files](#color-files). The former specify global settings, while the latter defines the colors. The color file is what you must pass to the function when calling. [Syntax](#syntax) is explained further down.

## Configuration files
These specify the behaviour of the parser. Any section created will generate an additional config file, even if no fields are defined. The following fields are allowed, with defaults provided if missing.

| Name |Description|Default|
|------|-----------|-------|
|`path`|Where the color file will be generated. If a directory, the program will create a file called colors.conf inside, if a file, it will put the contents there|`${output_dir}/${section_name}`|
|`format`|What color definition should look like|`${color_name} #${color_value}`|
|`format-id`|If the color has an id (eg. `bright_red` or `color46`), this format will be used instead|`${color_name} #${color_value}`|
|`comment`|The comment initialization string (eg. `--` in lua or `#` in python)|`#`|
|`config_format`| Specifies what the output file will look like, see examples| too long to fit here, TBA |
|`mode`| Can be either `modify-add` or `modify-ignore`. The former overrides keys `config_format` and adds anything remaining, the former ignores fields that are not in `config_format`|`modify-add`|
|`defaults`| A string contaning default values in the same syntax as the config. These values are appended to colors from the color file, and can contain variables. If the color file specifies a color already defined as default, the default color will be overriden by the defined one.|
|`post_cmd`| A bash command to run after running the app (made so you can make a reload script and it gets called automatically)|

Variables available

|Name|Value|
|-|-|
| `output_dir`|the argument `-o` if provided, the default config path otherwise|
| `config_dir`|the default config path (usually `~/.config/)|
| `section_name`|the name of the current section |
| `color_name`[^1]| name of the color|
| `color_id`[^1]| if color has id, its id, otherwise same as `color_name`|
| `color_value`[^1]| the value of the color, `RRGGBB`|

[^1]: won't get parsed but will be used in `format`


## Color files

These files provide colors to parse.
Section names must be the same as in [config](#configuration-files), any additionals wont be parsed. A section must be present to be outputted.
Color of the type `colorXXX` where `XXX` is a number from 0 to 255 and standard ANDI colors (`black`, `bright_green`, ...) will have an id. Colors with an id 0 - 15 are equivalent to ANSI colors, and will have such as `color_name`. If `mode` is set to `modify-add`, all the fields will be outputted, otherwise only those whose `key` matches the first word in a line of `config_format` (excluding `"`).

Any already defined color can be used as a variable. 

**If `color1` to `color6` and (`background` or `color0`) and (`foreground` or `color7`) are present, the program will generate a full 255 color palette.**

Any color defined as well as those generated can be used as a variable. If a field is assigned multiple times, the last definition will be used.

### Special fields
|Name|Description|
|-|-|
|`file`| If present, this file gets copied to section's `path`. Every other field is then ignored (this can be used to change wallpapers)|

## Syntax

### Values
Setting values takes the format `key = value`. Leading and trailing spaces for the `key` and `value` are cut.

Example:
`red = #123456` 

Values can be put into quotes (`"`). This way they can take up multiple lines

Ex:
```conf
format = "# comment line
${color_name} = #${color_value}"
```

### Sections
Sections are started by typing `[section-name]` on an empty line. Any field after a section initializer will be section-specific. Sections may not be re-initialized. Without specifying a section, everything will be global.

### Variables
Variables can be referenced using `${variable_name}`. Any already defined field can be used as a variable, as well as the default variables provided in [config](#configuration-files) and [color](#color-files) files.

## Examples
Example configs are provided in [examples](examples/).
