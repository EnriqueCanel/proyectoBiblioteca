#include "login.h"
#include "menu.h"  // Asegúrate de tener el header corregido
#include <pqxx/pqxx>
#include <string>

using namespace pqxx;

LRESULT CALLBACK LoginProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hUsuario, hClave, hBoton;

    switch (msg) {
        case WM_CREATE:
            CreateWindow("STATIC", "Usuario:", WS_VISIBLE | WS_CHILD,
                         50, 50, 100, 20, hwnd, NULL, NULL, NULL);
            hUsuario = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
                                    150, 50, 150, 20, hwnd, (HMENU)1, NULL, NULL);

            CreateWindow("STATIC", "Clave:", WS_VISIBLE | WS_CHILD,
                         50, 90, 100, 20, hwnd, NULL, NULL, NULL);
            hClave = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
                                  150, 90, 150, 20, hwnd, (HMENU)2, NULL, NULL);

            hBoton = CreateWindow("BUTTON", "Iniciar sesion", WS_VISIBLE | WS_CHILD,
                                  120, 140, 120, 30, hwnd, (HMENU)3, NULL, NULL);
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == 3) {
                char usuario[100], password[100];
                GetWindowText(hUsuario, usuario, 100);
                GetWindowText(hClave, password, 100);

                try {
                    connection c("dbname=biblioteca user=postgres password=1992 hostaddr=127.0.0.1 port=5432");
                    work txn(c);
                    result r = txn.exec_params(
                        "SELECT * FROM public.usuarios WHERE TRIM(usuario) = $1 AND TRIM(contrasena) = $2",
             usuario, password
                    );

                    if (!r.empty()) {
                        MessageBox(hwnd, "Inicio de sesión exitoso.", "Login", MB_OK);
                        DestroyWindow(hwnd);
                        CrearVentanaMenu(GetModuleHandle(NULL));  // Llamada correcta al menú
                    } else {
                        MessageBox(hwnd, "Usuario o clave incorrectos.", "Error", MB_OK | MB_ICONERROR);
                    }
                } catch (const std::exception& e) {
                    MessageBox(hwnd, e.what(), "Error de conexión", MB_OK | MB_ICONERROR);
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void mostrarLogin(HINSTANCE hInstance) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = LoginProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "LoginWindow";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("LoginWindow", "Iniciar sesión",
                             WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                             200, 200, 400, 250, NULL, NULL, hInstance, NULL);

    // Bucle de mensajes
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
