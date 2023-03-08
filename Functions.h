#pragma once

#ifndef WINDOWS_H
#include <windows.h>
#endif
#ifndef BCRYPT_H
#include <bcrypt.h>
#endif
#ifndef STDIO_H
#include <stdio.h>
#endif
#ifndef ERRHANDLINGAPI_H
#include <errhandlingapi.h>
#endif

/* Macro for recording indexes */
#define RECORDINDEX(name) \
                          name##_idxs[name##_n] = i; \
                          name##_n++;


#define CHANGECONDITIONARRAY \
                            if(digit_n > 1) conditionOK[0] = 1; \
                            if(spec_char_n > 1) conditionOK[1] = 1; \
                            if(u_letter_n > 1) conditionOK[2] = 1; \
                            if(l_letter_n > 1) conditionOK[3] = 1;

/* Macro for proportionate password generating algorithm */
#define DOPROPORTIONATE(func_name) \
        int cycle_num = genRandomNum(0, buffer_sz - 1);\
        int array_idx = 0; \
        for (int i = 0; i < cycle_num; array_idx++) { \
            if (array_idx == buffer_sz - 1) { \
                array_idx = -1; \
                continue; \
            } \
            if (buffer[array_idx] != '\0') \
                continue; \
            i++; \
        } \
        array_idx--;\
        buffer[array_idx] = func_name;

/* Macro for basic checks to see if password meets conditions(eg:- has letters) */
#define DOCHECK(func_name) \
int cycle_num = genRandomNum(0, 4); \
int array_idx = 0; \
for (int i = 0; i < cycle_num; array_idx++) { \
    if (array_idx == 4) { \
        array_idx = -1; \
        continue; \
    } \
    if (!conditionOK[array_idx]) \
        continue; \
    i++; \
} \
array_idx--; \
switch (array_idx) { \
    case 0: { \
        int pswd_buffer_idx = digit_idxs[genRandomNum(0, digit_n - 1)]; \
        buffer[pswd_buffer_idx] = func_name; \
        break; \
    } \
    case 1: { \
        int pswd_buffer_idx = spec_char_idxs[genRandomNum(0, spec_char_n - 1)]; \
        buffer[pswd_buffer_idx] = func_name; \
        break; \
    } \
    case 2: { \
        int pswd_buffer_idx = u_letter_idxs[genRandomNum(0, u_letter_n - 1)]; \
        buffer[pswd_buffer_idx] = func_name; \
        break; \
    } \
    case 3: { \
        int pswd_buffer_idx = l_letter_idxs[genRandomNum(0, l_letter_n - 1)]; \
        buffer[pswd_buffer_idx] = func_name; \
        break; \
    } \
}

/* For sending keys */
#define KEY_DOWN 0x1
#define KEY_UP 0x2

#define FREE_ARRAY_SIZE 16

void *array_to_free[FREE_ARRAY_SIZE] = { NULL };
int array_number = 0;

void handleError(char *s) {
    char str[256] = { '\0' };
    DWORD error = GetLastError();
    snprintf(str, 256, "%s.\nError code: %lu", s, error);
    MessageBoxA(NULL, str, 0, 0);
}

/* Objects which need to be deleted using DeleteObject() are added here) */
void add_to_free(void *object) {
    for (int i = 0; i < array_number; i++) { // check to see if the object is already in the array
        if (array_to_free[i] == object)
            return;
    }
    array_to_free[array_number] = object;
    array_number++;
}

void deleteObjects() {
    for (int i = array_number - 1; i >= 0; i--) {
        DeleteObject(array_to_free[i]);
    }
}

/* Simulates user input */
void sendKeySC(BYTE code, BYTE extra_codes, DWORD action) {
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = MapVirtualKey(code, MAPVK_VK_TO_VSC); // changes virtual key code to scan code
    // scan code is sent as they are the codes related to the physical input and can prevent some bugs
    if (action & KEY_DOWN) {
        input.ki.dwFlags = KEYEVENTF_SCANCODE | extra_codes;
        SendInput(1, &input, sizeof(INPUT));
    }
    if (action & KEY_UP) {
        input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP | extra_codes;
        SendInput(1, &input, sizeof(INPUT));
    }
}

/* Creates a pattern brush from the specified colours */
HBRUSH CreateGradientBrush4(COLORREF top_left, COLORREF top_right, COLORREF bottom_right, COLORREF bottom_left,
                           int height, int width, HDC hdc) {
    HBRUSH Brush = NULL;
    HDC hdcmem = CreateCompatibleDC(hdc);
    HBITMAP hbitmap = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(hdcmem, hbitmap);

    for (float y = 0; y < height; y++) {
        for (float x = 0; x < width; x++) { // 2 for loops represent each pixel
            RECT temp;

            float y_factor = (height - y) / height;
            float x_factor = (width - x) / width;

            /* length avg is basically saying that the further you are from the point the lesser the opacity of that colour */
            /* First I get the "length avg" from the top and bottom, of the left side */
            float red_left_avg = (GetRValue(top_left) * y_factor + GetRValue(bottom_left) * (1 - y_factor));
            float green_left_avg = (GetGValue(top_left) * y_factor + GetGValue(bottom_left) * (1 - y_factor));
            float blue_left_avg = (GetBValue(top_left) * y_factor + GetBValue(bottom_left) * (1 - y_factor));

            /* Then I get the the "length avg" from the top and bottom, of the right side */
            float red_right_avg = (GetRValue(top_right) * y_factor + GetRValue(bottom_right) * (1 - y_factor));
            float green_right_avg = (GetGValue(top_right) * y_factor + GetGValue(bottom_right) * (1 - y_factor));
            float blue_right_avg = (GetBValue(top_right) * y_factor + GetBValue(bottom_right) * (1 - y_factor));

            /* I find the "length avgs" from the previous 2 "length avgs" */
            float r = (red_left_avg * x_factor + red_right_avg * (1 - x_factor));
            float g = (green_left_avg * x_factor + green_right_avg * (1 - x_factor));
            float b = (blue_left_avg * x_factor + blue_right_avg * (1 - x_factor));

            COLORREF colour = RGB(r, g, b);

            Brush = CreateSolidBrush(colour);
            temp.left = x;
            temp.top = y;
            temp.right = x + 1;
            temp.bottom = y + 1;
            FillRect(hdcmem, &temp, Brush); // Paint the pattern into a bitmap
            DeleteObject(Brush);
        }
    }
    HBRUSH pattern = CreatePatternBrush(hbitmap); // Make the pattern into a brush

    /* Delete allocated memory objects or else a memory leak will be present */
    DeleteDC(hdcmem);
    DeleteObject(Brush);
    DeleteObject(hbitmap);

    return pattern; // Return the pattern brush
}

/* Fills a buffer with random bytes*/
NTSTATUS fillRandomBytes(BYTE *buffer, DWORD64 buffer_size) {
    NTSTATUS status = 0;
    BCRYPT_ALG_HANDLE handle;

    /* Open any algorithm for generating randomness */
    status = BCryptOpenAlgorithmProvider(&handle, BCRYPT_RNG_ALGORITHM, NULL, 0);
    if (!BCRYPT_SUCCESS(status))
        return status;

    /* Generate the randomness */
    status = BCryptGenRandom(handle, buffer, buffer_size, 0);
    if (!BCRYPT_SUCCESS(status))
        return status;

    /* Close the algorithm so there will be no memory leaks */
    status = BCryptCloseAlgorithmProvider(handle, 0);
    if (!BCRYPT_SUCCESS(status))
        return status;
}

/* Only works with + non-float limits. Min and Max inclusive */
DWORD64 genRandomNum(DWORD64 min, DWORD64 max) {
    DWORD64 max_length = 1;
    for (DWORD64 temp = max / 10;temp;max_length *= 10, temp /= 10); // determining max-length for the maximum number
    DWORD64 number = 0; // number to return

    do { // Keep doing this until number is within range
        fillRandomBytes((BYTE *)&number, sizeof(DWORD64));
        number %= max_length * 10; // Get the least significant digits for the max-length
    } while (number > max || number < min); // This method is used since the usual modulo method generates skewed results

    return number;
}

/* Generates a random special character(eg:- /&%#*) */
char genSpecChar(int is_sp_allowed) {
    // 32 to 47 && 58 to 64 && 91 to 96 && 123 to 126
    char ch;
    if (is_sp_allowed) {
        do {
            ch = genRandomNum(32, 126);
        } while (((ch > 47 && ch < 58) || (ch > 64 && ch < 91) || (ch > 96 && ch < 123)));
    }
    else {
        do {
            ch = genRandomNum(33, 126);
        } while (((ch > 47 && ch < 58) || (ch > 64 && ch < 91) || (ch > 96 && ch < 123)));
    }
    /* Ascii values for the non-special character */

    return ch;
}

/* Generates a random uppercase or lowercase letter */
char genLetter() {
    char ch;
    do {
        ch = genRandomNum(65, 122);
    } while (ch > 90 && ch < 97);
    /* Ascii values for the non-letter characters */

    return ch;
}

void checkSp(char *buffer, int buffer_sz) {
    if (buffer[buffer_sz - 2] == ' ') {
        char ch = genSpecChar(0);
        buffer[buffer_sz - 2] = ch;
    }
}

void generatePswd(char *buffer, int buffer_sz, int is_num, int is_special, int is_letter, int is_proportionate, int is_sp_allowed, int casing_option) {
    int u_letter_n = 0, l_letter_n = 0, spec_char_n = 0, digit_n = 0; // ints to keep track of the number of character types

    /* Buffers to store the index in password buffer, of the character types */
    int digit_idxs[buffer_sz];

    int spec_char_idxs[buffer_sz];

    int u_letter_idxs[buffer_sz];

    int l_letter_idxs[buffer_sz];


    buffer[buffer_sz - 1] = '\0';

    if (!is_proportionate) { // Proportionate algorithm is not used
        for (int i = 0, ch = 0; i < buffer_sz - 1; i++) { // For each index of the password buffer
            int selector = genRandomNum(0, 2);
            if (is_num && !selector) { // For digits
                ch = genRandomNum(48, 57);
                RECORDINDEX(digit)
            }
            else if (is_special && selector == 1) { // For special characters
                ch = genSpecChar(is_sp_allowed);
                RECORDINDEX(spec_char)
            }
            else if (!is_letter) { // if selector is 2 and !is_letter
                i--;
                continue;
            } // the below code will not be executed if is_letter = 0, because of the above `else if` statement
            else if (!casing_option) { // For uppercase and lowercase letters
                ch = genLetter();
                if (ch <= 90) {
                    RECORDINDEX(u_letter) // Thought about using a function but it seems overkill
                }
                else {
                    RECORDINDEX(l_letter)
                }
            }
            else if (casing_option == 1) { // For uppercase letters
                ch = genRandomNum(65, 90);
                // Another way to do this is generate letters using genLetter() then uppercase them later
                RECORDINDEX(u_letter)
                    // But this way works fine
            }
            else { // For lowercase letters
                ch = genRandomNum(97, 122);
                RECORDINDEX(l_letter)
            }
            buffer[i] = ch;
        }

        /** Check to see if all conditions are met **/

        int conditionOK[4] = { 0 }; // Basically a structure but an array
        /* conditionOK[4] = {digit, special, u_lett, l_lett};
           If any of these values are 1, it means that they have more than 1 instance in the pswd buffer
        */
        CHANGECONDITIONARRAY; // change the conditionOK[] array if there are more than 1 instance of the character types

        if (is_num && !digit_n) {
            DOCHECK(genRandomNum(48, 57))
        }
        if (is_special && !spec_char_n) {
            DOCHECK(genSpecChar(is_sp_allowed))
        }
        if (!is_letter)
            return;
        if ((!casing_option || casing_option == 1) && !u_letter_n) {
            DOCHECK(genRandomNum(65, 90))
        }
        if ((!casing_option || casing_option == 2) && !l_letter_n) {
            DOCHECK(genRandomNum(97, 122))
        }
        checkSp(buffer, buffer_sz);
        return;
    }


    /** Proportionate part **/
    for (int i = 0; i < buffer_sz - 1;buffer[i] = '\0', i++); // Zero-memorying the buffer
    int n_of_instances = (buffer_sz - 1) / 4; // Gets approximate number of characters for each character type
    /*
    How the proportionate algorithm works:
    (1) Select a number which is about the buffer size
    (2) Cycle through the password array that number of time, but ignoring the cases where char != '\0'
    (3) Replace that index which the cycling lands on with a random character(of respective character type)
    (4) Repeat this n_of_instances many times for each character type
    */
    if (is_num) {
        for (int i = 0; i < n_of_instances; i++) { // for digit
            DOPROPORTIONATE(genRandomNum(48, 57))
        }
    }
    else if (is_special) {
        for (int i = 0; i < n_of_instances; i++) { // for special characters
            DOPROPORTIONATE(genSpecChar(is_sp_allowed))
        }
    }
    else if (is_letter && !casing_option) {
        for (int i = 0; i < n_of_instances; i++) { // for letter
            DOPROPORTIONATE(genLetter())
        }
    }
    else if (is_letter && casing_option == 1) {
        for (int i = 0; i < n_of_instances; i++) { // for upper
            DOPROPORTIONATE(genRandomNum(65, 90))
        }
    }
    else if (is_letter && casing_option == 2) { // else won't work because of is_letter
        for (int i = 0; i < n_of_instances; i++) { // for lower
            DOPROPORTIONATE(genRandomNum(97, 122))
        }
    }

    /* Check whether any blank spaces are present, if so replace them with a random character */
    for (int i = 0, ch = 0; i < buffer_sz - 1; i++) {
        if (buffer[i] != '\0')
            continue;
        int selector = genRandomNum(0, 2);
        if (is_num && !selector) { // For digits
            ch = genRandomNum(48, 57);
        }
        else if (is_special && selector == 1) { // For special characters
            ch = genSpecChar(is_sp_allowed);
        }
        else if (!is_letter) { // if selector is 2 and !is_letter
            i--;
            continue;
        } // the below code will not be executed if is_letter = 0, because of the above `else if` statement
        else if (!casing_option) { // For uppercase and lowercase letters
            ch = genLetter();
        }
        else if (casing_option == 1) { // For uppercase letters
            ch = genRandomNum(65, 90);
        }
        else { // For lowercase letters
            ch = genRandomNum(97, 122);
        }
        buffer[i] = ch;
    }
    checkSp(buffer, buffer_sz);
}