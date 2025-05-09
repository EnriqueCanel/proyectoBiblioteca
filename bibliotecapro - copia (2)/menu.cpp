#include "menu.h"
#include <windows.h>
#include "agregar_libro.h"
#include "prestar_libro.h"
#include "devolver_libro.h"



LRESULT CALLBACK MenuProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1:
                    CrearVentanaAgregarLibro(hwnd);
                    break;
                case 2:
                    MessageBox(hwnd, "Buscar libro...", "Menú", MB_OK);
                    break;
                case 3:
                    MessageBox(hwnd, "Listar libros...", "Menú", MB_OK);
                    break;
                case 4:
                    CrearVentanaPrestarLibro(hwnd);
                    break;
                case 5:
                    CrearVentanaDevolverLibro(hwnd);
                    break;
                case 6:
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void mostrarMenu(HINSTANCE hInstance) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = MenuProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MenuWindow";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("MenuWindow", "Menú Biblioteca",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        150, 150, 400, 320, NULL, NULL, hInstance, NULL);

    CreateWindow("BUTTON", "1. Agregar libro", WS_VISIBLE | WS_CHILD,
        100, 30, 200, 30, hwnd, (HMENU)1, hInstance, NULL);
    CreateWindow("BUTTON", "2. Buscar libro", WS_VISIBLE | WS_CHILD,
        100, 70, 200, 30, hwnd, (HMENU)2, hInstance, NULL);
    CreateWindow("BUTTON", "3. Listar libros", WS_VISIBLE | WS_CHILD,
        100, 110, 200, 30, hwnd, (HMENU)3, hInstance, NULL);
    CreateWindow("BUTTON", "4. Prestar libro", WS_VISIBLE | WS_CHILD,
        100, 150, 200, 30, hwnd, (HMENU)4, hInstance, NULL);
    CreateWindow("BUTTON", "5. Devolver libro", WS_VISIBLE | WS_CHILD,
        100, 190, 200, 30, hwnd, (HMENU)5, hInstance, NULL);
    CreateWindow("BUTTON", "6. Salir", WS_VISIBLE | WS_CHILD,
        100, 230, 200, 30, hwnd, (HMENU)6, hInstance, NULL);

    // Bucle de mensajes específico para el menú
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

// Esta es la función que llamas desde login.cpp
void CrearVentanaMenu(HINSTANCE hInstance) {
    mostrarMenu(hInstance);
}
