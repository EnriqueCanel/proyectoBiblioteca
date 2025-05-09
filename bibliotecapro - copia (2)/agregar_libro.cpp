#include "agregar_libro.h"
#include <pqxx/pqxx>
#include <windows.h>
#include <string>
#include <regex>

using namespace pqxx;

HWND txtISBN, txtTitulo, txtAutor, txtEditorial, txtAnio, txtMateria;
HWND hwndAgregarLibro;

// Simulación del menú principal
void mostrarMenu(HWND hwndPadre) {
    MessageBox(hwndPadre, "Volviendo al menú principal.", "Menú", MB_OK | MB_ICONINFORMATION);
}

LRESULT CALLBACK LibroProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Botón "Guardar"
                char isbn[20], titulo[255], autor[255], editorial[255], anio[20], materia[255];
                GetWindowText(txtISBN, isbn, 20);
                GetWindowText(txtTitulo, titulo, 255);
                GetWindowText(txtAutor, autor, 255);
                GetWindowText(txtEditorial, editorial, 255);
                GetWindowText(txtAnio, anio, 20);
                GetWindowText(txtMateria, materia, 255);

                // Verificar que todos los campos estén llenos
                if (strlen(isbn) == 0 || strlen(titulo) == 0 || strlen(autor) == 0 ||
                    strlen(editorial) == 0 || strlen(anio) == 0 || strlen(materia) == 0) {
                    MessageBox(hwnd, "Todos los campos son obligatorios.", "Validación", MB_OK | MB_ICONWARNING);
                    break;
                }

                // Validar formato de fecha YYYY-MM-DD
                if (!std::regex_match(anio, std::regex("^\\d{4}-\\d{2}-\\d{2}$"))) {
                    MessageBox(hwnd, "Fecha inválida. Usa formato YYYY-MM-DD.", "Error", MB_OK | MB_ICONERROR);
                    break;
                }

                try {
                    connection conn("dbname=biblioteca user=postgres password=1992 hostaddr=127.0.0.1 port=5432");
                    work txn(conn);
                    txn.exec_params(
                        "INSERT INTO libros (isbn, titulo, autor, editorial, anio_publicacion, materia) VALUES ($1, $2, $3, $4, $5, $6)",
                        std::string(isbn), std::string(titulo), std::string(autor),
                        std::string(editorial), std::string(anio), std::string(materia)
                    );
                    txn.commit();

                    MessageBox(hwnd, "Libro agregado correctamente.", "Éxito", MB_OK | MB_ICONINFORMATION);

                    // Limpiar campos
                    SetWindowText(txtISBN, "");
                    SetWindowText(txtTitulo, "");
                    SetWindowText(txtAutor, "");
                    SetWindowText(txtEditorial, "");
                    SetWindowText(txtAnio, "");
                    SetWindowText(txtMateria, "");

                } catch (const sql_error &e) {
                    MessageBoxA(hwnd, e.what(), "Error al agregar libro", MB_OK | MB_ICONERROR);
                } catch (const std::exception &e) {
                    MessageBoxA(hwnd, e.what(), "Error inesperado", MB_OK | MB_ICONERROR);
                }
            } else if (LOWORD(wParam) == 2) { // Botón "Regresar al Menú"
                DestroyWindow(hwnd);  // Cerrar ventana actual
                mostrarMenu(NULL);   // Simular regreso al menú
            }
            break;

        case WM_DESTROY:
            hwndAgregarLibro = NULL;
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CrearVentanaAgregarLibro(HWND hwndPadre) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = LibroProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "AgregarLibro";

    RegisterClass(&wc);

    hwndAgregarLibro = CreateWindow("AgregarLibro", "Agregar Libro",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        150, 150, 520, 420, hwndPadre, NULL, NULL, NULL);

    // Labels y campos
    CreateWindow("STATIC", "ISBN:", WS_VISIBLE | WS_CHILD, 30, 30, 100, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtISBN = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 30, 320, 20, hwndAgregarLibro, NULL, NULL, NULL);

    CreateWindow("STATIC", "Título:", WS_VISIBLE | WS_CHILD, 30, 60, 100, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtTitulo = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 60, 320, 20, hwndAgregarLibro, NULL, NULL, NULL);

    CreateWindow("STATIC", "Autor:", WS_VISIBLE | WS_CHILD, 30, 90, 100, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtAutor = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 90, 320, 20, hwndAgregarLibro, NULL, NULL, NULL);

    CreateWindow("STATIC", "Editorial:", WS_VISIBLE | WS_CHILD, 30, 120, 100, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtEditorial = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 120, 320, 20, hwndAgregarLibro, NULL, NULL, NULL);

    CreateWindow("STATIC", "Año Publicación (YYYY-MM-DD):", WS_VISIBLE | WS_CHILD, 30, 150, 200, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtAnio = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 240, 150, 220, 20, hwndAgregarLibro, NULL, NULL, NULL);

    CreateWindow("STATIC", "Materia:", WS_VISIBLE | WS_CHILD, 30, 180, 100, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtMateria = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 180, 320, 20, hwndAgregarLibro, NULL, NULL, NULL);

    // Botones
    CreateWindow("BUTTON", "Guardar", WS_VISIBLE | WS_CHILD,
        190, 240, 120, 30, hwndAgregarLibro, (HMENU)1, NULL, NULL);

    CreateWindow("BUTTON", "Regresar al Menú", WS_VISIBLE | WS_CHILD,
        190, 290, 120, 30, hwndAgregarLibro, (HMENU)2, NULL, NULL);
}