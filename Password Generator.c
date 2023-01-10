#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>
#include <uxtheme.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errhandlingapi.h>

#include "Functions.h"
#include "Definitions.h"
#include "Global Variables.h"
#include "resource.h"

/* Initializes a class */
ATOM MyRegisterClass(HINSTANCE hInst, char *name, UINT styles, HBRUSH bkg_brush, WNDPROC proc)
{
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = styles;
    wc.lpfnWndProc = proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInst;
    // wc.hIcon = LoadImageA(NULL, ICON, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
    wc.hIcon = LoadIconA(hInst, MAKEINTRESOURCE(MYICON));
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = bkg_brush;
    wc.lpszMenuName = NULL;
    // wc.hIcon = LoadImageA(NULL, "Icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
    // wc.hIconSm = LoadIcon(NULL, IDI_QUESTION);
    wc.hIconSm = LoadIconA(hInst, IDI_APPLICATION);
    wc.lpszClassName = name;

    return RegisterClassExA(&wc);
}

LRESULT CALLBACK UpDownProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_LBUTTONUP: {
            PostMessage(pswd_limit_numeric_field, WM_KILLFOCUS, 0, 0); // Disables highlighting buddy control when up-down buttons are pressed
            break;
        }
    }
    CallWindowProc(ud_old_proc, hwnd, msg, wp, lp);
}

LRESULT CALLBACK limitTxtProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_KEYDOWN: { // Needed to enable ctrl + backspace not working. I basically manually implement it
            short ctrl = GetAsyncKeyState(VK_CONTROL);
            int prev_press = lp & 0x40000000; // checks whether the key was held down previous
            // prevents repeatedly calling this message

            short z = GetAsyncKeyState(0x5A); // To prevent control z sending VK_BACK message
            if ((wp == VK_BACK) && !prev_press && ctrl && !z) {
                /* Following code is to allow for control and backspace */
                /* Basically I recreate Ctrl + Shift + Left and then I do Backspace */

                PostMessage(hwnd, WM_CLEAR, 0, 0); // Disables the following functionality if a selection is already present
                /* I think this happens because WM_CLEAR eats up the next messages ¯\_(ツ)_/¯ */

                short isCntrl = GetAsyncKeyState(VK_CONTROL); // Checks whether control is held down

                BlockInput(TRUE);

                sendKeySC(VK_CONTROL, 0, KEY_DOWN);
                sendKeySC(VK_SHIFT, 0, KEY_DOWN);

                sendKeySC(VK_LEFT, KEYEVENTF_EXTENDEDKEY, KEY_DOWN | KEY_UP);
                // KEYEVENTF_EXTENDEDKEY needed to stop malfunctioning when used with shift held down

                sendKeySC(VK_SHIFT, 0, KEY_UP);
                sendKeySC(VK_CONTROL, 0, KEY_UP);

                sendKeySC(VK_BACK, 0, KEY_DOWN | KEY_UP);

                BlockInput(FALSE);

                // isCntrl sometimes gives 0 and 1 both
                if (isCntrl != 1 && isCntrl) { // If control is held down
                    sendKeySC(VK_CONTROL, 0, KEY_DOWN);
                }
            }
            break;
        }
        case WM_KILLFOCUS: { // randomize password limit if limit<8 and you click out of focus
            char str[3] = { 0 };
            GetWindowText(hwnd, str, 3);
            int n = atoi(str);
            if (n < PSW_LOWER_LIMIT) {
                char s[3];
                int number = genRandomNum(8, 99);
                sprintf(str, "%i", number);
                SetWindowText(hwnd, str);
            }
            break;
        }
        case WM_LBUTTONDOWN: {
            PostMessage(hwnd, WM_SETFOCUS, 0, 0); // re-enables focusing because disabling buddy highlighting stops this
            break;
        }
    }
    CallWindowProc(limit_old_proc, hwnd, msg, wp, lp);
}

LRESULT CALLBACK pswdTxtProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_CHAR: {
            // Disables control backspace problem where it creates a character
            if (wp >= 127) {
                return 1;
            }
        }
        case WM_KEYDOWN: {
            short ctrl = GetAsyncKeyState(VK_CONTROL);
            int prev_press = lp & 0x40000000; // checks whether the key was held down previous
            // prevents repeatedly calling this message

            short z = GetAsyncKeyState(0x5A); // To prevent control z sending VK_BACK message
            if ((wp == VK_BACK) && !prev_press && ctrl && !z) {
                /* Following code is to allow for control and backspace */
                /* Basically I recreate Ctrl + Shift + Left and then I do Backspace */

                PostMessage(hwnd, WM_CLEAR, 0, 0); // Disables the following functionality if a selection is already present
                /* I think this happens because WM_CLEAR eats up the next messages ¯\_(ツ)_/¯ */

                short isCntrl = GetAsyncKeyState(VK_CONTROL);

                BlockInput(TRUE);

                sendKeySC(VK_CONTROL, 0, KEY_DOWN);
                sendKeySC(VK_SHIFT, 0, KEY_DOWN);

                sendKeySC(VK_LEFT, KEYEVENTF_EXTENDEDKEY, KEY_DOWN | KEY_UP);
                // KEYEVENTF_EXTENDEDKEY needed to stop malfunctioning when used with shift held down

                sendKeySC(VK_SHIFT, 0, KEY_UP);
                sendKeySC(VK_CONTROL, 0, KEY_UP);

                sendKeySC(VK_BACK, 0, KEY_DOWN | KEY_UP);

                BlockInput(FALSE);

                // isCntrl sometimes gives 0 and 1 both
                if (isCntrl != 1 && isCntrl) {
                    sendKeySC(VK_CONTROL, 0, KEY_DOWN);
                }
            }
            break;
        }
    }
    CallWindowProc(text_old_proc, hwnd, msg, wp, lp);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_NCLBUTTONDBLCLK: // Prevents closing when you double click the menu icon on top left
            return 0;
        case WM_COMMAND: {
            switch (LOWORD(wp)) {
                case LETTER_CB: {
                    switch (HIWORD(wp)) {
                        case BN_CLICKED: { // disables casing_options if letters are disabled
                            if (IsDlgButtonChecked(hwnd, LETTER_CB) != BST_CHECKED)
                                Button_Enable(casing_options, FALSE);
                            else
                                Button_Enable(casing_options, TRUE);
                            break;
                        }
                    }
                    break;
                }
                case SHOW_PSWD_CB: {
                    switch (HIWORD(wp)) {
                        case BN_CLICKED: { // hides the password text control if not clicked
                            if (IsDlgButtonChecked(hwnd, SHOW_PSWD_CB) == BST_CHECKED)
                                ShowWindow(pswd_text_field, SW_SHOW);
                            else
                                ShowWindow(pswd_text_field, SW_HIDE);
                            break;
                        }
                    }
                    break;
                }
                case SHOW_LIMIT_CB: { // hides the limit text control if not clicked
                    switch (HIWORD(wp)) {
                        case BN_CLICKED: {
                            if (IsDlgButtonChecked(hwnd, SHOW_LIMIT_CB) == BST_CHECKED)
                                ShowWindow(pswd_limit_numeric_field, SW_SHOW);
                            else
                                ShowWindow(pswd_limit_numeric_field, SW_HIDE);
                            break;
                        }
                    }
                    break;
                }
                case RND_PSWD_BTN: {
                    switch (HIWORD(wp)) {
                        case BN_CLICKED: {
                            char str[3];
                            int n = genRandomNum(8, 99);
                            sprintf(str, "%i", n);
                            SetWindowText(pswd_limit_numeric_field, str);
                            break;
                        }
                    }
                    break;
                }
                case CPY_BTN: {
                    switch (HIWORD(wp)) {
                        case BN_CLICKED: {
                            char temp[100];
                            GetWindowText(pswd_text_field, temp, 100);
                            int len = strlen(temp) + 1;
                            char pswd[len];
                            strncpy(pswd, temp, len); // strncpy is memory safe
                            pswd[len - 1] = '\0'; // Just to be safe, but not needed

                            // GlobalAlloc allocates global bytes the heap
                            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len); // needs GMEM_MOVEABLE since the copy buffer moves
                            if (!hMem) {
                                handleError("Copying failed");
                                break;
                            }
                            // that flag allows you to lock memory and unlock iirc
                            memcpy(GlobalLock(hMem), pswd, len); // Global lock makes memory non-moveable. Memcpy fails without locking memory
                            // You can't free locked objects and they stay with the same owner, until program termination
                            GlobalUnlock(hMem); // Makes memory movable again
                            OpenClipboard(0);
                            EmptyClipboard();
                            SetClipboardData(CF_TEXT, hMem);
                            CloseClipboard();

                            // Global lock kind of confuses me, so I don't know what it exactly does

                            SendMessage(copy_btn, WM_SETTEXT, (WPARAM)font, (LPARAM)"Copied");
                            break;
                        }
                    }
                    break;
                }
                case GEN_BTN: {
                    switch (HIWORD(wp)) {
                        case BN_CLICKED: {
                            /* gets the password limit */
                            char limit[3] = { '\0' };
                            GetWindowText(pswd_limit_numeric_field, limit, 3);
                            int n = atoi(limit);
                            char pswd[n + 1];

                            int is_num = IsDlgButtonChecked(hwnd, NUM_CB);
                            int is_special = IsDlgButtonChecked(hwnd, SPECIAL_CB);
                            int is_letter = IsDlgButtonChecked(hwnd, LETTER_CB);
                            if (!(is_letter || is_num || is_special)) // no password will be generated if all needed checkboxes are unchecked
                                break;
                            int is_proportionate = IsDlgButtonChecked(hwnd, PROPORTIONATE_CB);
                            int casing_option = SendMessage((HWND)casing_options, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                            // (HWND)lp fails for some reason hence I used casing_options HWND directly
                            generatePswd(pswd, n + 1, is_num, is_special, is_letter, is_proportionate, casing_option);
                            SetWindowText(pswd_text_field, pswd);
                            SendMessage(copy_btn, WM_SETTEXT, (WPARAM)font, (LPARAM)"Copy");
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
        case WM_XBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN: { // Shifts focus out of the controls when clicked anywhere outside controls
            RECT rect;
            GetWindowRect(pswd_text_field, &rect);
            int xPos = GET_X_LPARAM(lp);
            int yPos = GET_Y_LPARAM(lp);
            if (!(xPos <= rect.right && xPos >= rect.left && yPos <= rect.top && yPos >= rect.bottom))
                SetFocus(hwnd);
            break;
        }
        case WM_CTLCOLORSTATIC: {
            /* Without this, the windows 11 rounding happens incorrectly which causes white corners */
            RECT checkbox_rect;
            HWND cb_hwnd = (HWND)lp;
            HDC hdc = GetDC(hwnd);
            GetClientRect(cb_hwnd, &checkbox_rect);
            MapWindowPoints(cb_hwnd, hwnd, (LPPOINT)&checkbox_rect, 2);
            COLORREF topleft_colour = GetPixel(hdc, checkbox_rect.left, checkbox_rect.top);
            COLORREF topright_colour = GetPixel(hdc, checkbox_rect.right, checkbox_rect.top);
            COLORREF bottomleft_colour = GetPixel(hdc, checkbox_rect.left, checkbox_rect.bottom);
            COLORREF bottomright_colour = GetPixel(hdc, checkbox_rect.right, checkbox_rect.bottom);
            // SetBkColor((HDC)wp, topleft_colour); // Don't know why this is needed, but it was there in the documentation ¯\_(ツ)_/¯
            DeleteObject(hCB_brush_colour);// Make sure old brushes are freed(deleted)
            hCB_brush_colour = CreateGradientBrush4(topleft_colour, topright_colour, bottomright_colour, bottomleft_colour,
            checkbox_rect.bottom - checkbox_rect.top, checkbox_rect.right - checkbox_rect.left, hdc);
            DeleteDC(hdc);
            return (LRESULT)hCB_brush_colour;
        }

        case WM_CREATE: {
            /* Load modern control looks using latest Comctl32 version*/
            INITCOMMONCONTROLSEX controls = { sizeof(INITCOMMONCONTROLSEX), ICC_STANDARD_CLASSES };
            if (!InitCommonControlsEx(&controls))
                handleError("Loading controls failed");

            pswd_text_field = CreateWindow("EDIT",
                         "", WS_BORDER | WS_CHILD | ES_LEFT | WS_VSCROLL |
                         ES_MULTILINE | ES_AUTOVSCROLL | LBS_DISABLENOSCROLL, // works without ES_AUTOVSCROLL but I kept it in for future compatibility sakes
                         xpos_pswd_text, ypos_pswd_text, pswd_text_width, pswd_text_height,
                         hwnd, (HMENU)1, NULL, NULL);
            if (!pswd_text_field)
                handleError("Text box creation failed");
            SendMessage(pswd_text_field, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font
            text_old_proc = (WNDPROC)SetWindowLongPtr(pswd_text_field, GWLP_WNDPROC, (LONG_PTR)pswdTxtProc); // subclass control
            add_to_free(text_old_proc);

            int cx_check_box = GetSystemMetrics(SM_CXMENUCHECK) - GetSystemMetrics(SM_CXEDGE); // gets the default checkbox size
            int cy_check_box = GetSystemMetrics(SM_CYMENUCHECK) - GetSystemMetrics(SM_CYEDGE); // gets the default checkbox size
            HWND show_pswd_checkbox = CreateWindowW(L"BUTTON", L"Show Password", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, xpos_showpswd_checkbox, ypos_showpswd_checkbox,
                                      cx_check_box + pswd_cb_xfont_padding,
                                      cy_check_box + cb_yfont_padding,
                                      hwnd, (HMENU)SHOW_PSWD_CB, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            if (!show_pswd_checkbox)
                handleError("Show password checkbox creation failed");
            SendMessage(show_pswd_checkbox, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font

            HWND generate_btn = CreateWindowW(
                L"BUTTON", L"Generate Password", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                xpos_gen_btn, ypos_gen_btn, gen_btn_width, gen_btn_height,
                hwnd, (HMENU)GEN_BTN, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            if (!generate_btn)
                handleError("Generate button creation failed");
            SendMessage(generate_btn, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font

            copy_btn = CreateWindowW(L"BUTTON", L"Copy", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, xpos_cpy_btn, ypos_cpy_btn,
                                          cpy_btn_width, cpy_btn_height, hwnd, (HMENU)CPY_BTN, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            if (!copy_btn)
                handleError("Copy button creation failed");
            SendMessage(copy_btn, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font

            HWND rnd_pswd_limit_btn = CreateWindowW(
                L"BUTTON", L"Randomize password        limit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | BS_MULTILINE,
                xpos_rnd_pswd_limit_btn, ypos_rnd_pswd_limit_btn, rnd_pswd_limit_btn_width, rnd_pswd_limit_btn_height,
                hwnd, (HMENU)RND_PSWD_BTN, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            if (!rnd_pswd_limit_btn)
                handleError("Randomize limit button creation failed");
            SendMessage(rnd_pswd_limit_btn, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font

            /* Restore default control looks because I like them better :) */
            SetWindowTheme(copy_btn, L" ", L" ");
            SetWindowTheme(generate_btn, L" ", L" ");
            SetWindowTheme(rnd_pswd_limit_btn, L" ", L" ");

            HWND show_pswd_limit_checkbox = CreateWindowW(L"BUTTON", L"Show password limit", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, xpos_show_pswd_limit_CB, ypos_show_pswd_limit_CB,
                                      cx_check_box + limit_cb_xfont_padding,
                                      cy_check_box + cb_yfont_padding,
                                      hwnd, (HMENU)SHOW_LIMIT_CB, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            if (!show_pswd_limit_checkbox)
                handleError("Show password limit checkbox creation failed");
            SendMessage(show_pswd_limit_checkbox, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font

            pswd_limit_numeric_field = CreateWindowEx(WS_EX_LEFT | WS_EX_CLIENTEDGE | WS_EX_CONTEXTHELP,    //Extended window styles.
                              WC_EDIT,
                              NULL,
                              WS_CHILD  // Window styles.
                              | ES_NUMBER | ES_LEFT,                     // Edit control styles.
                              xpos_pswd_limit_numeric_field, ypos_pswd_limit_numeric_field,
                              pswd_limit_numeric_field_width, pswd_limit_numeric_field_height,
                              hwnd,
                              NULL,
                              (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                              NULL);
            if (!pswd_limit_numeric_field)
                handleError("Password limit numeric field creation failed");
            SendMessage(pswd_limit_numeric_field, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font
            SendMessage(pswd_limit_numeric_field, EM_SETLIMITTEXT, (WPARAM)2, 0); // Sets how many characters the user can enter
            limit_old_proc = (WNDPROC)SetWindowLongPtr(pswd_limit_numeric_field, GWLP_WNDPROC, (LONG_PTR)limitTxtProc); // subclass control
            add_to_free(limit_old_proc);
            char str[3];
            int n = genRandomNum(8, 99);
            sprintf(str, "%i", n);
            SetWindowText(pswd_limit_numeric_field, str); // sets a random limit

            HWND up_down_btns = CreateWindowEx(WS_EX_LEFT | WS_EX_LTRREADING,
                          UPDOWN_CLASS,
                          NULL,
                          WS_CHILD | WS_VISIBLE
                           | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK,
                          0, 0,
                          0, 0,         // Set to zero to automatically size to fit the buddy window.
                          hwnd,
                          NULL,
                          (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                          NULL);
            SendMessage(up_down_btns, UDM_SETBUDDY, (WPARAM)pswd_limit_numeric_field, (LPARAM)NULL); // Sets the up_down_btns to correspond
            // to the limits textbox
            SendMessage(up_down_btns, UDM_SETRANGE, 0, MAKELPARAM(PSW_UPPER_LIMIT, PSW_LOWER_LIMIT)); // Sets the ranges of the up_down_btns
            ud_old_proc = (WNDPROC)SetWindowLongPtr(up_down_btns, GWLP_WNDPROC, (LONG_PTR)UpDownProc);
            add_to_free(ud_old_proc);

            /* Uppercase or lower case choosing dropbox */
            casing_options = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | BS_DEFSPLITBUTTON | CBS_DROPDOWN | CBS_HASSTRINGS, xpos_casing_options, ypos_casing_options,
                                      casing_options_width,
                                      casing_options_height,
                                      hwnd, (HMENU)CASE_OPTIONS_DD, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            if (!casing_options)
                handleError("Casing options dropdown box creation failed");
            SendMessage(casing_options, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font
            SendMessage(casing_options, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Both");
            SendMessage(casing_options, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Uppercase only");
            SendMessage(casing_options, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)"Lowercase only");
            SendMessage(casing_options, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

            HWND num_checkbox = CreateWindowW(L"BUTTON", L"Allow numbers", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, xpos_num_CB, ypos_num_CB,
                                      cx_check_box + num_cb_xfont_padding,
                                      cy_check_box + cb_yfont_padding,
                                      hwnd, (HMENU)NUM_CB, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            if (!num_checkbox)
                handleError("Allow numbers checkbox creation failed");
            SendMessage(num_checkbox, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font
            CheckDlgButton(hwnd, NUM_CB, BST_CHECKED); // Changes checkbox state to checked when starting window

            HWND special_checkbox = CreateWindowW(L"BUTTON", L"Allow special characters", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, xpos_special_CB, ypos_special_CB,
                                      cx_check_box + special_cb_xfont_padding,
                                      cy_check_box + cb_yfont_padding,
                                      hwnd, (HMENU)SPECIAL_CB, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            if (!special_checkbox)
                handleError("Allow special characters checkbox creation failed");
            SendMessage(special_checkbox, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font
            CheckDlgButton(hwnd, SPECIAL_CB, BST_CHECKED); // Changes checkbox state to checked when starting window

            HWND letter_checkbox = CreateWindowW(L"BUTTON", L"Allow letters", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, xpos_letter_CB, ypos_letter_CB,
                                      cx_check_box + letter_cb_xfont_padding,
                                      cy_check_box + cb_yfont_padding,
                                      hwnd, (HMENU)LETTER_CB, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            if (!letter_checkbox)
                handleError("Allow letters checkbox creation failed");
            SendMessage(letter_checkbox, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font
            CheckDlgButton(hwnd, LETTER_CB, BST_CHECKED); // Changes checkbox state to checked when starting window

            /* Checkbox to specify whether to use the proportionate password generating algorithm */
            HWND isproportionate_CB = CreateWindowW(L"BUTTON", L"Proportionate password", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, xpos_isproportionate_CB, ypos_isproportionate_CB,
                                      cx_check_box + isproportionate_cb_xfont_padding,
                                      cy_check_box + cb_yfont_padding,
                                      hwnd, (HMENU)PROPORTIONATE_CB, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            if (!isproportionate_CB)
                handleError("Proportionate password checkbox creation failed");
            SendMessage(isproportionate_CB, WM_SETFONT, (WPARAM)font, (LPARAM)MAKELONG(TRUE, 0)); // To set font

            break;
        }
        case WM_CLOSE: {
            DestroyWindow(hwnd);
            break;
        }
        case WM_DESTROY: {
            /* Remove objects from memory */
            deleteObjects();
            DeleteObject(hCB_brush_colour);
            PostQuitMessage(0);
            break;
        }
        case WM_INITMENU: { // Prevents the menu ever starting up when the icon on the top left is clicked
            EndMenu();
            break;
        }
        case WM_GETMINMAXINFO: { // Sets the size to a constant window size
            MINMAXINFO *mminfo = (MINMAXINFO *)lp;
            mminfo->ptMinTrackSize.x = w_width;
            mminfo->ptMaxTrackSize.x = w_width;
            mminfo->ptMinTrackSize.y = w_height;
            mminfo->ptMaxTrackSize.y = w_height;
            break;
        }
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    {
        if (!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) // Sets the DPI awareness so the window won't malfunction in high DPI screens
            OutputDebugStringA("WARNING: could not set DPI awareness");

        /* Creating a better font than the default font */
        font = CreateFontW(22, 10, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, -1 /*ignore*/, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FIXED_PITCH | FF_DONTCARE, L"my_font"); // Cleartype has anti-aliasing
        add_to_free(font);
    }

    {
        HDC hdctemp = GetDC(hwnd);

        /* Make background a pattern */
        HBRUSH bkg_brush = CreateGradientBrush4(RGB(160, 0, 0), RGB(50, 0, 0), RGB(5, 5, 5), RGB(10, 10, 10),
                           w_height, w_width, hdctemp);
        add_to_free(bkg_brush);
        DeleteDC(hdctemp);
        if (!MyRegisterClass(hInstance, w_name, CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, bkg_brush, WndProc)) {
            handleError("Main window registration failed :(");
            return 0;
        }
        // Having no WS_THICKFRAME disables resizing arrows
        hwnd = CreateWindowExA(WS_EX_OVERLAPPEDWINDOW, w_name, w_title, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
                               0, 0, w_width, w_height, (HWND)NULL, (HMENU)NULL, hInstance,
                               (LPVOID)NULL); // maximize box and resizing arrows are removed
        if (!hwnd) {
            handleError("Main window creation failed :(");
            return 0;
        }
        SendMessage(hwnd, WM_CHANGEUISTATE, (WPARAM)(0x10001), (LPARAM)(0)); // Disables focus control highlighting when opened through terminal
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return msg.wParam;
}

/* TODO:
(6) Make sure to link with Visual styles >:)
(19) Add icons
*/