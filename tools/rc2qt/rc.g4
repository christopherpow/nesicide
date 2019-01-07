grammar rc;

rc:	statement* ;

statement: bitmap_statement
         | toolbar_statement
         | menu_statement
         | accelerator_statement
         | icon_statement
         | dialogex_statement
         | dialoginit_statement
         | stringtable_statement
         ;

bitmap_statement: ID BITMAP String ;

BUTTON: 'BUTTON' ;
SEPARATOR: 'SEPARATOR' ;
toolbar_statement: ID TOOLBAR Number ',' Number BEGIN toolbar_item* END ;
toolbar_item: BUTTON ID
            | SEPARATOR
            ;

menu_statement: ID MENU BEGIN popup_item+ END ;
popup_item: 'POPUP' String ((',')? MENU_OPTIONS)* BEGIN (menu_item | popup_item)* END ;
MENU_OPTIONS: 'CHECKED'
      | 'GRAYED'
      | 'HELP'
      | 'INACTIVE'
      | 'MENUBARBREAK'
      | 'MENUBREAK'
      ;
MENUITEM: 'MENUITEM' ;
menu_item: MENUITEM String ',' ID  ((',')? MENU_OPTIONS)*
         | MENUITEM SEPARATOR
         ;

dialogex_statement: ID DIALOGEX Number ',' Number ',' Number ',' Number (',' ID)? dialog_option* BEGIN dialog_item* END ;
CAPTION: 'CAPTION' ;
CHARACTERISTICS: 'CHARACTERISTICS' ;
CLASS: 'CLASS' ;
FONT: 'FONT' ;
MENU: 'MENU' ;
STYLE: 'STYLE' ;
EXSTYLE: 'EXSTYLE' ;
dialog_option: CAPTION String
             | CHARACTERISTICS Number
             | CLASS dialog_class_identifier
             | FONT Number ',' String ',' Number ',' Number ',' Number
             | MENU ID
             | STYLE stylelist
             | EXSTYLE window_exstylelist
             ;
dialog_class_identifier: ID
                | String
                ;
NOT: 'NOT' ;
stylelist: stylelist '|' NOT? any_style
                | NOT? any_style
                ;
window_exstylelist: window_exstylelist '|' 'NOT'? window_exstyles
                    | 'NOT'? window_exstyles
                    ;
dialog_item: generic_control_statement
           | static_control_statement
           | button_control_statement
           | edit_control_statement
           | groupbox_control_statement
           | combobox_control_statement
           | listbox_control_statement
           | defpushbutton_control_statement
           | icon_control_statement
           ;

generic_control_statement: CONTROL String ',' ID ',' String ',' stylelist ',' Number ',' Number ',' Number ',' Number (',' window_exstylelist)? ;

static_control_statement: static_class_identifier String ',' ID ',' Number ',' Number ',' Number ',' Number (',' stylelist (',' window_exstylelist)?)? ;
static_class_identifier: 'LTEXT'
                       | 'RTEXT'
                       | 'CTEXT'
                       ;

button_control_statement: button_class_identifier String ',' ID ',' Number ',' Number ',' Number ',' Number (',' stylelist (',' window_exstylelist)?)? ;
button_class_identifier: 'AUTO3STATE'
                       | 'AUTOCHECKBOX'
                       | 'AUTORADIOBUTTON'
                       | 'CHECKBOX'
                       | 'PUSHBOX'
                       | 'PUSHBUTTON'
                       | 'RADIOBUTTON'
                       | 'STATE3'
                       | 'USERBUTTON'
                       ;

defpushbutton_control_statement: 'DEFPUSHBUTTON' String ',' ID ',' Number ',' Number ',' Number ',' Number (',' stylelist (',' window_exstylelist)?)? ;

edit_control_statement: edit_class_identifier ID ',' Number ',' Number ',' Number ',' Number (',' stylelist (',' window_exstylelist)?)? ;
edit_class_identifier: 'EDITTEXT'
                     | 'BEDIT'
                     | 'HEDIT'
                     | 'IEDIT'
                     ;

groupbox_control_statement: 'GROUPBOX' String ',' ID ',' Number ',' Number ',' Number ',' Number (',' stylelist (',' window_exstylelist)?)? ;

combobox_control_statement: 'COMBOBOX' ID ',' Number ',' Number ',' Number ',' Number (',' stylelist (',' window_exstylelist)?)? ;

listbox_control_statement: 'LISTBOX' ID ',' Number ',' Number ',' Number ',' Number (',' stylelist (',' window_exstylelist)?)? ;

dialoginit_statement: ID DLGINIT BEGIN dialoginit_item* END ;
dialoginit_data: ((String | Number) ','?) ;
dialoginit_item: ID ',' '0x403' ',' Number ',' Number dialoginit_data+ ;

icon_control_statement: ICON ( ID | String ) ',' ID ',' Number ',' Number (',' Number ',' Number ',' stylelist (',' window_exstylelist)?)? ;

accelerator_statement: ID ACCELERATORS BEGIN accelerator_item+ END ;
accelerator_item: accelerator_event ',' ID (',' ACCELERATOR_TYPE)? (',' (ACCELERATOR_OPTIONS|CONTROL))* ;
accelerator_event: ID
                 | Number
                 | String
                 ;

icon_statement: ID ICON String ;

stringtable_statement: STRINGTABLE BEGIN stringtable_item+ END ;
stringtable_item: ID String ;

fragment Integer : [0-9] ;
fragment HexInteger: (('0'..'9')|('A'..'F')|('a'..'f')) ;
Number  : ( Integer Integer* | ( '0x' | '0X' ) HexInteger HexInteger* ) ;

fragment Valid_id_start: [A-Za-z] ;
fragment Valid_id_char: [A-Za-z0-9_] ;

BITMAP: 'BITMAP' ;
TOOLBAR: 'TOOLBAR' ;
CONTROL: 'CONTROL' ;
ACCELERATORS: 'ACCELERATORS' ;
ACCELERATOR_TYPE: 'ASCII'
                | 'VIRTKEY'
                ;
ACCELERATOR_OPTIONS: 'NOINVERT'
                   | 'ALT'
                   | 'SHIFT'
                   | 'CONTROL'
                   ;

ICON: 'ICON' ;

STRINGTABLE: 'STRINGTABLE' ;

DIALOGEX: 'DIALOGEX' ;
DLGINIT:  'DLGINIT' ;

BEGIN: 'BEGIN' ;
END: 'END' ;

window_styles: 'WS_BORDER'
             | 'WS_CAPTION'
             | 'WS_CHILD'
             | 'WS_CHILDWINDOW'
             | 'WS_CLIPCHILDREN'
             | 'WS_CLIPSIBLINGS'
             | 'WS_DISABLED'
             | 'WS_DLGFRAME'
             | 'WS_GROUP'
             | 'WS_HSCROLL'
             | 'WS_ICONIC'
             | 'WS_MAXIMIZE'
             | 'WS_MAXIMIZEBOX'
             | 'WS_MINIMIZE'
             | 'WS_MINIMIZEBOX'
             | 'WS_OVERLAPPED'
             | 'WS_OVERLAPPEDWINDOW'
             | 'WS_POPUP'
             | 'WS_POPUPWINDOW'
             | 'WS_SIZEBOX'
             | 'WS_SYSMENU'
             | 'WS_TABSTOP'
             | 'WS_THICKFRAME'
             | 'WS_TILED'
             | 'WS_TILEDWINDOW'
             | 'WS_VISIBLE'
             | 'WS_VSCROLL'
             ;
window_exstyles: 'WS_EX_ACCEPTFILES'
               | 'WS_EX_APPWINDOW'
               | 'WS_EX_CLIENTEDGE'
               | 'WS_EX_COMPOSITED'
               | 'WS_EX_CONTEXTHELP'
               | 'WS_EX_CONTROLPARENT'
               | 'WS_EX_DLGMODALFRAME'
               | 'WS_EX_LAYERED'
               | 'WS_EX_LAYOUTRTL'
               | 'WS_EX_LEFT'
               | 'WS_EX_LEFTSCROLLBAR'
               | 'WS_EX_LTRREADING'
               | 'WS_EX_MDICHILD'
               | 'WS_EX_NOACTIVATE'
               | 'WS_EX_NOINHERITLAYOUT'
               | 'WS_EX_NOPARENTNOTIFY'
               | 'WS_EX_NOREDIRECTIONBITMAP'
               | 'WS_EX_OVERLAPPEDWINDOW'
               | 'WS_EX_PALETTEWINDOW'
               | 'WS_EX_RIGHT'
               | 'WS_EX_RIGHTSCROLLBAR'
               | 'WS_EX_RTLREADING'
               | 'WS_EX_STATICEDGE'
               | 'WS_EX_TOOLWINDOW'
               | 'WS_EX_TOPMOST'
               | 'WS_EX_TRANSPARENT'
               | 'WS_EX_WINDOWEDGE'
               ;
dialog_styles: 'DS_3DLOOK'
             | 'DS_ABSALIGN'
             | 'DS_CENTER'
             | 'DS_CENTERMOUSE'
             | 'DS_CONTEXTHELP'
             | 'DS_CONTROL'
             | 'DS_FIXEDSYS'
             | 'DS_LOCALEDIT'
             | 'DS_MODALFRAME'
             | 'DS_NOFAILCREATE'
             | 'DS_NOIDLEMSG'
             | 'DS_SETFONT'
             | 'DS_SETFOREGROUND'
             | 'DS_SHELLFONT'
             | 'DS_SYSMODAL'
             ;
button_styles: 'BS_3STATE'
             | 'BS_AUTO3STATE'
             | 'BS_AUTOCHECKBOX'
             | 'BS_AUTORADIOBUTTON'
             | 'BS_CHECKBOX'
             | 'BS_COMMANDLINK'
             | 'BS_DEFCOMMANDLINK'
             | 'BS_DEFPUSHBUTTON'
             | 'BS_DEFSPLITBUTTON'
             | 'BS_GROUPBOX'
             | 'BS_OWNERDRAW'
             | 'BS_PUSHBUTTON'
             | 'BS_RADIOBUTTON'
             | 'BS_SPLITBUTTON'
             | 'BS_USERBUTTON'
             | 'BS_LEFTTEXT'
             | 'BS_RIGHTBUTTON'
             | 'BS_PUSHLIKE'
             | 'BS_LEFT'
             | 'BS_RIGHT'
             | 'BS_CENTER'
             | 'BS_TOP'
             | 'BS_BOTTOM'
             | 'BS_VCENTER'
             | 'BS_BITMAP'
             | 'BS_ICON'
             | 'BS_TEXT'
             | 'BS_FLAT'
             | 'BS_MULTILINE'
             | 'BS_NOTIFY'
             ;
static_styles: 'SS_BITMAP'
             | 'SS_BLACKFRAME'
             | 'SS_BLACKRECT'
             | 'SS_CENTER'
             | 'SS_CENTERIMAGE'
             | 'SS_EDITCONTROL'
             | 'SS_ENDELLIPSIS'
             | 'SS_ENHMETAFILE'
             | 'SS_ETCHEDFRAME'
             | 'SS_ETCHEDHORZ'
             | 'SS_ETCHEDVERT'
             | 'SS_GRAYFRAME'
             | 'SS_GRAYRECT'
             | 'SS_ICON'
             | 'SS_LEFT'
             | 'SS_LEFTNOWORDWRAP'
             | 'SS_NOPREFIX'
             | 'SS_NOTIFY'
             | 'SS_OWNERDRAW'
             | 'SS_PATHELLIPSIS'
             | 'SS_REALSIZECONTROL'
             | 'SS_REALSIZEIMAGE'
             | 'SS_RIGHT'
             | 'SS_RIGHTJUST'
             | 'SS_SIMPLE'
             | 'SS_SUNKEN'
             | 'SS_TYPEMASK'
             | 'SS_WHITEFRAME'
             | 'SS_WHITERECT'
             | 'SS_WORDELLIPSIS'
             ;
edit_styles: 'ES_AUTOHSCROLL'
           | 'ES_AUTOVSCROLL'
           | 'ES_CENTER'
           | 'ES_LEFT'
           | 'ES_LOWERCASE'
           | 'ES_MULTILINE'
           | 'ES_NOHIDESEL'
           | 'ES_NUMBER'
           | 'ES_OEMCONVERT'
           | 'ES_PASSWORD'
           | 'ES_READONLY'
           | 'ES_RIGHT'
           | 'ES_UPPERCASE'
           | 'ES_WANTRETURN'
           ;
listbox_styles: 'LBS_COMBOBOX'
              | 'LBS_DISABLENOSCROLL'
              | 'LBS_EXTENDEDSEL'
              | 'LBS_HASSTRINGS'
              | 'LBS_MULTICOLUMN'
              | 'LBS_MULTIPLESEL'
              | 'LBS_NODATA'
              | 'LBS_NOINTEGRALHEIGHT'
              | 'LBS_NOREDRAW'
              | 'LBS_NOSEL'
              | 'LBS_NOTIFY'
              | 'LBS_OWNERDRAWFIXED'
              | 'LBS_OWNERDRAWVARIABLE'
              | 'LBS_SORT'
              | 'LBS_STANDARD'
              | 'LBS_USETABSTOPS'
              | 'LBS_WANTKEYBOARDINPUT'
              ;
combobox_styles: 'CBS_AUTOHSCROLL'
               | 'CBS_DISABLENOSCROLL'
               | 'CBS_DROPDOWN'
               | 'CBS_DROPDOWNLIST'
               | 'CBS_HASSTRINGS'
               | 'CBS_LOWERCASE'
               | 'CBS_NOINTEGRALHEIGHT'
               | 'CBS_OEMCONVERT'
               | 'CBS_OWNERDRAWFIXED'
               | 'CBS_OWNERDRAWVARIABLE'
               | 'CBS_SIMPLE'
               | 'CBS_SORT'
               | 'CBS_UPPERCASE'
               ;
scrollbar_styles: 'SBS_BOTTOMALIGN'
                | 'SBS_HORZ'
                | 'SBS_LEFTALIGN'
                | 'SBS_RIGHTALIGN'
                | 'SBS_SIZEBOX'
                | 'SBS_SIZEBOXBOTTOMRIGHTALIGN'
                | 'SBS_SIZEBOXTOPLEFTALIGN'
                | 'SBS_SIZEGRIP'
                | 'SBS_TOPALIGN'
                | 'SBS_VERT'
                ;
listview_styles: 'LVS_ALIGNLEFT'
               | 'LVS_ALIGNMASK'
               | 'LVS_ALIGNTOP'
               | 'LVS_AUTOARRANGE'
               | 'LVS_EDITLABELS'
               | 'LVS_ICON'
               | 'LVS_LIST'
               | 'LVS_NOCOLUMNHEADER'
               | 'LVS_NOLABELWRAP'
               | 'LVS_NOSCROLL'
               | 'LVS_NOSORTHEADER'
               | 'LVS_OWNERDATA'
               | 'LVS_OWNERDRAWFIXED'
               | 'LVS_REPORT'
               | 'LVS_SHAREIMAGELISTS'
               | 'LVS_SHOWSELALWAYS'
               | 'LVS_SINGLESEL'
               | 'LVS_SMALLICON'
               | 'LVS_SORTASCENDING'
               | 'LVS_SORTDESCENDING'
               | 'LVS_TYPEMASK'
               | 'LVS_TYPESTYLEMASK'
               ;
updowncontrol_styles: 'UDS_ALIGNLEFT'
                    | 'UDS_ALIGNRIGHT'
                    | 'UDS_ARROWKEYS'
                    | 'UDS_AUTOBUDDY'
                    | 'UDS_HORZ'
                    | 'UDS_HOTTRACK'
                    | 'UDS_NOTHOUSANDS'
                    | 'UDS_SETBUDDYINT'
                    | 'UDS_WRAP'
                    ;
trackbarcontrol_styles: 'TBS_AUTOTICKS'
                      | 'TBS_VERT'
                      | 'TBS_HORZ'
                      | 'TBS_TOP'
                      | 'TBS_BOTTOM'
                      | 'TBS_LEFT'
                      | 'TBS_RIGHT'
                      | 'TBS_BOTH'
                      | 'TBS_NOTICKS'
                      | 'TBS_ENABLESELRANGE'
                      | 'TBS_FIXEDLENGTH'
                      | 'TBS_NOTHUMB'
                      | 'TBS_TOOLTIPS'
                      | 'TBS_REVERSED'
                      | 'TBS_DOWNISLEFT'
                      | 'TBS_NOTIFYBEFOREMOVE'
                      | 'TBS_TRANSPARENTBKGND'
                      ;
treeview_styles: 'TVS_CHECKBOXES'
               | 'TVS_DISABLEDRAGDROP'
               | 'TVS_EDITLABELS'
               | 'TVS_FULLROWSELECT'
               | 'TVS_HASBUTTONS'
               | 'TVS_HASLINES'
               | 'TVS_INFOTIP'
               | 'TVS_LINESATROOT'
               | 'TVS_NOHSCROLL'
               | 'TVS_NONEVENHEIGHT'
               | 'TVS_NOSCROLL'
               | 'TVS_NOTOOLTIPS'
               | 'TVS_RTLREADING'
               | 'TVS_SHOWSELALWAYS'
               | 'TVS_SINGLEEXPAND'
               | 'TVS_TRACKSELECT'
               ;
progressbar_styles: 'PBS_MARQUEE'
                  | 'PBS_SMOOTH'
                  | 'PBS_SMOOTHREVERSE'
                  | 'PBS_VERTICAL'
                  ;
any_style: Number
         | window_styles
         | dialog_styles
         | button_styles
         | static_styles
         | edit_styles
         | listbox_styles
         | listview_styles
         | scrollbar_styles
         | combobox_styles
         | updowncontrol_styles
         | trackbarcontrol_styles
         | treeview_styles
         | progressbar_styles
         ;

ID      : Number
        | Valid_id_start Valid_id_char*
        ;

String  : '"' (~[\r\n"])* '"' ;
WS : (' ' | '\t')+ -> channel(HIDDEN) ;
NEWLINE: ('\r' | '\n') -> channel(HIDDEN) ;
Comment: '//' (~[\r\n])* -> channel(HIDDEN) ;
