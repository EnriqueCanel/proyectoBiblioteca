#include "prestar_libro.h"
#include <pqxx/pqxx>
#include <windows.h>
#include <string>
#include <ctime>

using namespace pqxx;

HWND txtISBNPrestamo;
HWND hwndPrestarLibro;

std::string obtenerFechaActual() {
    time_t t = time(NULL);
    tm* timePtr = localtime(&t);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", timePtr);
    return std::string(buffer);
}

LRESULT CALLBACK PrestarLibroProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Botón Prestar
                char isbn[20];
                GetWindowText(txtISBNPrestamo, isbn, 20);

                if (strlen(isbn) == 0) {
                    MessageBox(hwnd, "Debe ingresar el ISBN.", "Error", MB_OK | MB_ICONWARNING);
                    break;
                }

                try {
                    connection conn("dbname=biblioteca user=postgres password=1992 hostaddr=127.0.0.1 port=5432");
                    work txn(conn);

                    // Buscar libro en la tabla libross
                    result libro = txn.exec_params("SELECT titulo, autor, estado FROM libross WHERE isbn = $1", std::string(isbn));

                    if (libro.empty()) {
                        MessageBox(hwnd, "El libro no existe en la base de datos.", "Error", MB_OK | MB_ICONERROR);
                        break;
                    }

                    std::string estadoActual = libro[0]["estado"].as<std::string>();

                    if (estadoActual == "prestado") {
                        MessageBox(hwnd, "El libro ya está prestado.", "No disponible", MB_OK | MB_ICONWARNING);
                        break;
                    }

                    // Si el libro está disponible, se puede prestar
                    std::string fecha = obtenerFechaActual();
                    txn.exec_params(
                        "UPDATE libross SET estado = 'prestado', fecha_movimiento = $1 WHERE isbn = $2",
                        fecha, std::string(isbn)
                    );

                    txn.commit();

                    std::string titulo = libro[0]["titulo"].as<std::string>();
                    std::string autor = libro[0]["autor"].as<std::string>();
                    std::string mensaje = "Préstamo realizado:\n\nTítulo: " + titulo + "\nAutor: " + autor;

                    MessageBox(hwnd, mensaje.c_str(), "Éxito", MB_OK | MB_ICONINFORMATION);
                    SetWindowText(txtISBNPrestamo, "");

                } catch (const std::exception &e) {
                    MessageBoxA(hwnd, e.what(), "Error", MB_OK | MB_ICONERROR);
                }
            } else if (LOWORD(wParam) == 2) { // Regresar
                DestroyWindow(hwnd);
            }
            break;

        case WM_DESTROY:
            hwndPrestarLibro = NULL;
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CrearVentanaPrestarLibro(HWND hwndPadre) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = PrestarLibroProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "PrestarLibroWindow";

    RegisterClass(&wc);

    hwndPrestarLibro = CreateWindow("PrestarLibroWindow", "Prestar Libro",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        150, 150, 400, 200, hwndPadre, NULL, NULL, NULL);

    CreateWindow("STATIC", "ISBN del libro:", WS_VISIBLE | WS_CHILD,
        30, 40, 120, 20, hwndPrestarLibro, NULL, NULL, NULL);
    txtISBNPrestamo = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
        160, 40, 180, 20, hwndPrestarLibro, NULL, NULL, NULL);

    CreateWindow("BUTTON", "Prestar", WS_VISIBLE | WS_CHILD,
        80, 100, 100, 30, hwndPrestarLibro, (HMENU)1, NULL, NULL);

    CreateWindow("BUTTON", "Regresar", WS_VISIBLE | WS_CHILD,
        200, 100, 100, 30, hwndPrestarLibro, (HMENU)2, NULL, NULL);
}
