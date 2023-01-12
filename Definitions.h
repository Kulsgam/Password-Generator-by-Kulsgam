/** Constants for position and other related info **/

#pragma once

/* Basic window info */
#define w_name "Password Generator v1.0"
#define w_title "Password Generator by Kulsgam v1.0"
#define w_width 600
#define w_height 275

/* Positions */
#define xpos_pswd_text 5
#define ypos_pswd_text 5
#define xpos_gen_btn 370
#define ypos_gen_btn 180
#define xpos_cpy_btn 500
#define ypos_cpy_btn 20
#define xpos_rnd_pswd_limit_btn 370
#define ypos_rnd_pswd_limit_btn 100
#define xpos_pswd_limit_numeric_field 360 - pswd_limit_numeric_field_width
#define ypos_pswd_limit_numeric_field 112
#define xpos_casing_options 5
#define ypos_casing_options 90 + 1 + 6 * (cy_check_box + cb_yfont_padding)
#define xpos_showpswd_checkbox 5
#define ypos_showpswd_checkbox 90
#define xpos_show_pswd_limit_CB 5
#define ypos_show_pswd_limit_CB 90 + cb_yfont_padding + cy_check_box
#define xpos_isproportionate_CB 5
#define ypos_isproportionate_CB 90 + 2 * (cy_check_box + cb_yfont_padding)
#define xpos_num_CB 5
#define ypos_num_CB 90 + 3 * (cy_check_box + cb_yfont_padding)
#define xpos_special_CB 5
#define ypos_special_CB 90 + 4 * (cy_check_box + cb_yfont_padding)
#define xpos_letter_CB 5
#define ypos_letter_CB 90 + 5 * (cy_check_box + cb_yfont_padding)

/* Control sizes */
#define pswd_text_width 480
#define pswd_text_height 80
#define gen_btn_width 200
#define gen_btn_height 50
#define cpy_btn_width 70
#define cpy_btn_height 50
#define rnd_pswd_limit_btn_width 200
#define rnd_pswd_limit_btn_height 50
#define pswd_limit_numeric_field_width 52
#define pswd_limit_numeric_field_height 30
#define casing_options_width 200
#define casing_options_height 100

/* Other */
#define PSW_UPPER_LIMIT 99
#define PSW_LOWER_LIMIT 8

/* Checkbox font padding */
#define cb_yfont_padding 5
#define pswd_cb_xfont_padding 140
#define limit_cb_xfont_padding 200
#define num_cb_xfont_padding 140
#define special_cb_xfont_padding 250
#define letter_cb_xfont_padding 140
#define isproportionate_cb_xfont_padding 240

/* Control identifiers */
#define SHOW_PSWD_CB 101
#define SHOW_LIMIT_CB 102
#define NUM_CB 103
#define SPECIAL_CB 104
#define LETTER_CB 105
#define PROPORTIONATE_CB 106
#define RND_PSWD_BTN 107
#define CASE_OPTIONS_DD 108
#define CPY_BTN 109
#define GEN_BTN 110

/* Array sizes */
#define COLOUR_ARRAY_SIZE PROPORTIONATE_CB - SHOW_PSWD_CB + 1