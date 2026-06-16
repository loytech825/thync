# https://github.com/loytech825/thync
#

highlight clear


#remember to :set cursorline or :set cursorline! 

set background=dark
let g:colors_name="test_theme"



#
#
#   DEFAULT SECTION
#
#
# Default colors for everything (use fg and bg)
hi Normal guifg=#${foreground} guibg=#${background}
#hi NormalNC non curent window

# Line numbers
hi LineNr guifg=#${fg4} guibg=bg
#hi CursorLineNr guifg=#${fg4} guibg=bg #inherits anyways

# Left of line numbers 
hi SignColumn guifg=#${fg2} guibg=#${background}
#hi FoldColumn guifg=#00ffff guibg=#FFFF00 #inherits anyways

# Below buffer end mostly (EndOfBuffer links to this)
# hi NonText guifg=#FF0F00 guibg=#00ff80


# Between the windows (fallback to Normal)
hi WinSeparator guifg=#${accent} guibg=bg

# Current directory in nvimtree and status line (bottom)
hi Title guifg=#${accent} guibg=NONE "also applies to #Info# and such on autocomplete windows
hi StatusLine guifg=fg guibg=#${bg1}
#hi StatusLineNc non current window
#hi StatusLineTerm terminal windows
#Selectionmenu  
hi PMenu guifg=${fg2} guibg=#${bg0} "works for mini.completion
hi PMenuSel guifg=#${green} guibg=bg "selected value (bg and fg are inverted!)
#hi PMenuSBar guifg=#ffff00 guibg=#808000 #scrollbar (fg no work ?)
#hi PMenuThumb guifg=#008000 guibg=#800000 #this bg is scrollbar guifg

#Tabs idek bro do :tabnew
#hi TabLine guifg=#ff00ff guibg=#800080
#hi TabLineSel guifg=#800080 guibg=#ff00ff
#TabLineFill filler area in tablines
hi Visual guifg=#${foreground} guibg=#${blue} "Highlight in visual mode
#VisualNOS when visual not allowed fallback is ^


hi MsgArea guifg=#${fg} guibg=#${bg0}#cmd line fg works not for : ?
#MsgSeparator between messages
#MoreMsg #more# propmt and similar
hi ModeMsg guifg=#${accent} guibg=NONE "message showing current mode
#Question prompts that ask for user input

#
#
#   Search/selection
#
#
hi Search guifg=#${background} guibg=#${accent2} "all matched patterns
hi IncSearch guifg=#${background} guibg=#${blue}
# Substitite hl for substituted text
#hi MathcParen 


hi Directory guifg=#${accent2}

#
# 
#   Syntax
# 
#
hi Comment guifg=#${clr_comment} guibg=NONE
hi SpecialComment guifg=bg guibg=fg

hi Constant guifg=#${clr_constant}
#hi Number <- Constant
#hi Float <-Number
#hi Boolean <- Constant
hi String guifg=#${clr_string}
#hi Character <-String
#hi SpecialChar

hi Special guifg=#${clr_special} "for some reason this affects int, auto...

hi Identifier guifg=#${clr_identifier}
hi @variable guifg=#${clr_variable}
hi Function guifg=#${clr_function}
hi Statement guifg=#${clr_statement}
#hi Repeat
#hi Label guibg=#ff0000
hi Operator guifg=#${clr_operator}
hi Delimiter guifg=#${clr_delimiter}
#hi Exception
#hi @lsp.type.namespace

#hi PreProc guibg=
#hi Include
#hi Define
#hi Macro
#hi PreCondit


hi Type guifg=#${clr_type}
#hi StorageClass guibg=#${clr_storage_class}
#hi Structure
#hi Typedef

#hi Tag
#hi Debug
hi Keyword guifg=#${clr_keyword}

#
#
# LSP 
#
#
hi @lsp.type.parameter guifg=#${clr_parameter}
hi @lsp.type.qualifier guifg=#${clr_qualifier}

hi CursorLine guifg=None guibg=None

#idk what this does (Unprintable characters)
#hi SpecialKey guifg=#00ff80 guibg=#ff0f00

hi NvimTreeGitIgnored guifg=#${clr_nv_tree_git_ignored}
hi NvimTreeExecFile guifg=#${clr_nv_tree_exec_file}
hi NvimTreeImageFile guifg=#${clr_nv_tree_iamge_file}

