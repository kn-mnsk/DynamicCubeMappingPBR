#pragma once

#ifndef MYCURSOR_H
#define MYCUESOR_H// custom cursor creation

#include <stdio.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWcursor* createCuresor(GLFWwindow * window) {

    unsigned char pixels[16 * 16 * 4];
    memset(pixels, 0xff, sizeof(pixels));
    GLFWimage cursorImage;
    cursorImage.width = 16;
    cursorImage.height = 16;
    cursorImage.pixels = pixels;


    /*unsigned char color[4] = { 255, 0, 0, 1 };
    for (int i = 0; i < 16 * 16; i += 4)
    {
        pixels[i] =  0x80;
        pixels[i + 1] = 0x80;
        pixels[i + 2] = 0x00;
        pixels[i + 3] = 0xFF;
    }

    unsigned char c = pixels[1];
    for (int i = 0; i < 8; ++i) {
        bool is_set = c & (1 << i);
        std::cout << "Bit " << i << ": " << is_set << '\n';
    }

    */

    GLFWcursor* cursor = glfwCreateCursor(&cursorImage, 0, 0);
    glfwSetCursor(window, cursor);
    // end of custom cursor creation#pragma once

    return cursor;
}


#endif