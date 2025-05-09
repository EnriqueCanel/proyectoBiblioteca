#ifndef LOGIN_H
#define LOGIN_H

#include <windows.h>

LRESULT CALLBACK LoginProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool verificarCredenciales(const char* usuario, const char* password);

#endif