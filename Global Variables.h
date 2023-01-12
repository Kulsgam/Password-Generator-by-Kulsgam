#pragma once

#ifndef WINDOWS_H
#include <windows.h>
#endif

WINBOOL afterCreation = FALSE; // Needed to track the initial creation of the controls(check WM_CTLCOLORSTATIC for more details)
COLORREF topleft_colour[COLOUR_ARRAY_SIZE];
COLORREF topright_colour[COLOUR_ARRAY_SIZE];
COLORREF bottomleft_colour[COLOUR_ARRAY_SIZE];
COLORREF bottomright_colour[COLOUR_ARRAY_SIZE];

HWND hwnd = NULL;

HWND casing_options = NULL;

HBRUSH hCB_brush_colour = NULL;
HFONT font = NULL;

/* Text Fields */
WNDPROC text_old_proc, limit_old_proc = NULL;
HWND pswd_text_field, pswd_limit_numeric_field = NULL;

/* Buttons */
WNDPROC ud_old_proc = NULL;
HWND copy_btn = NULL;