#include "devolver_libro.h"
#include <windows.h>
#include <pqxx/pqxx>
#include <string>

using namespace pqxx;

HWND txtISBNDevolver;
HWND hwndDevolverLibro;

LRESULT CALLBACK DevolverLibroProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Botón Devolver
                char isbn[20];
                GetWindowText(txtISBNDevolver, isbn, 20);

                if (strlen(isbn) == 0) {
                    MessageBox(hwnd, "Debe ingresar el ISBN.", "Error", MB_OK | MB_ICONWARNING);
                    break;
                }

                try {
                    connection conn("dbname=biblioteca user=postgres password=1992 hostaddr=127.0.0.1 port=5432");
                    work txn(conn);

                    // Buscar si el libro está en préstamo en la tabla libross
                    result r = txn.exec_params("SELECT estado FROM libross WHERE isbn = $1", std::string(isbn));

                    if (r.empty()) {
                        MessageBox(hwnd, "No se encontró el libro con el ISBN ingresado.", "No encontrado", MB_OK | MB_ICONERROR);
                    } else {
                        std::string estado = r[0]["estado"].as<std::string>();

                        if (estado == "prestado") {
                            // Obtener información del libro
                            result libro = txn.exec_params("SELECT titulo, autor FROM libross WHERE isbn = $1", std::string(isbn));

                            if (!libro.empty()) {
                                std::string titulo = libro[0]["titulo"].as<std::string>();
                                std::string autor = libro[0]["autor"].as<std::string>();
                                std::string mensaje = "Libro a devolver:\n\nTítulo: " + titulo + "\nAutor: " + autor + "\n\n¿Deseas continuar?";

                                int respuesta = MessageBox(hwnd, mensaje.c_str(), "Confirmar devolución", MB_YESNO | MB_ICONQUESTION);
                                if (respuesta == IDYES) {
                                    // Actualizar estado del libro a "disponible" en la tabla libross
                                    txn.exec_params("UPDATE libross SET estado = 'disponible', fecha_movimiento = CURRENT_DATE WHERE isbn = $1 AND estado = 'prestado'", std::string(isbn));
                                    txn.commit();
                                    MessageBox(hwnd, "El libro ha sido devuelto con éxito.", "Éxito", MB_OK | MB_ICONINFORMATION);
                                    SetWindowText(txtISBNDevolver, "");
                                }
                            } else {
                                MessageBox(hwnd, "Información del libro no encontrada.", "Error", MB_OK | MB_ICONERROR);
                            }
                        } else {
                            MessageBox(hwnd, "El libro no está prestado y no se puede realizar la operación.", "Estado inválido", MB_OK | MB_ICONWARNING);
                        }
                    }

                } catch (const std::exception &e) {
                    MessageBoxA(hwnd, e.what(), "Error", MB_OK | MB_ICONERROR);
                }
            } else if (LOWORD(wParam) == 2) { // Regresar
                DestroyWindow(hwnd);
            }
            break;

        case WM_DESTROY:
            hwndDevolverLibro = NULL;
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CrearVentanaDevolverLibro(HWND hwndPadre) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = DevolverLibroProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "DevolverLibroWindow";

    RegisterClass(&wc);

    hwndDevolverLibro = CreateWindow("DevolverLibroWindow", "Devolver Libro",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        180, 180, 400, 200, hwndPadre, NULL, NULL, NULL);

    CreateWindow("STATIC", "ISBN del libro:", WS_VISIBLE | WS_CHILD,
        30, 40, 120, 20, hwndDevolverLibro, NULL, NULL, NULL);
    txtISBNDevolver = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
        160, 40, 180, 20, hwndDevolverLibro, NULL, NULL, NULL);

    CreateWindow("BUTTON", "Devolver", WS_VISIBLE | WS_CHILD,
        80, 100, 100, 30, hwndDevolverLibro, (HMENU)1, NULL, NULL);

    CreateWindow("BUTTON", "Regresar", WS_VISIBLE | WS_CHILD,
        200, 100, 100, 30, hwndDevolverLibro, (HMENU)2, NULL, NULL);
}
