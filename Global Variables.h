#pragma once

#ifndef WINDOWS_H
#include <windows.h>
#endif

WINBOOL afterCreation = FALSE; // Needed to track the initial creation of the controls(check WM_CTLCOLORSTATIC for more details)
COLORREF topleft_colour[PROPORTIONATE_CB - SHOW_PSWD_CB + 1];
COLORREF topright_colour[PROPORTIONATE_CB - SHOW_PSWD_CB + 1];
COLORREF bottomleft_colour[PROPORTIONATE_CB - SHOW_PSWD_CB + 1];
COLORREF bottomright_colour[PROPORTIONATE_CB - SHOW_PSWD_CB + 1];

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