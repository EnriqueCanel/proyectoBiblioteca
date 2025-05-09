#include "agregar_libro.h"
#include <windows.h>
#include <pqxx/pqxx>
#include <string>
#include <ctime>


using namespace pqxx;
HWND txtISBN, txtTitulo, txtAutor, txtEditorial, txtAnio, txtMateria;
HWND hwndAgregarLibro;

LRESULT CALLBACK AgregarLibroProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Botón Guardar
                char isbn[20], titulo[100], autor[100], editorial[100], anio[20], materia[100];
                GetWindowText(txtISBN, isbn, 20);
                GetWindowText(txtTitulo, titulo, 100);
                GetWindowText(txtAutor, autor, 100);
                GetWindowText(txtEditorial, editorial, 100);
                GetWindowText(txtAnio, anio, 20);
                GetWindowText(txtMateria, materia, 100);

                if (strlen(isbn) == 0 || strlen(titulo) == 0 || strlen(autor) == 0 ||
                    strlen(editorial) == 0 || strlen(anio) == 0 || strlen(materia) == 0) {
                    MessageBox(hwnd, "Todos los campos son obligatorios.", "Error", MB_OK | MB_ICONWARNING);
                    break;
                }

                try {
                    connection conn("dbname=biblioteca user=postgres password=1992 hostaddr=127.0.0.1 port=5432");
                    work txn(conn);

                    txn.exec_params(
                        "INSERT INTO libross (isbn, titulo, autor, editorial, anio_publicacion, materia) "
                        "VALUES ($1, $2, $3, $4, $5, $6)",
                        std::string(isbn),
                        std::string(titulo),
                        std::string(autor),
                        std::string(editorial),
                        std::string(anio),
                        std::string(materia)
                    );

                    txn.commit();

                    MessageBox(hwnd, "Libro agregado correctamente.", "Éxito", MB_OK | MB_ICONINFORMATION);

                    SetWindowText(txtISBN, "");
                    SetWindowText(txtTitulo, "");
                    SetWindowText(txtAutor, "");
                    SetWindowText(txtEditorial, "");
                    SetWindowText(txtAnio, "");
                    SetWindowText(txtMateria, "");

                } catch (const std::exception &e) {
                    MessageBoxA(hwnd, e.what(), "Error", MB_OK | MB_ICONERROR);
                }
            } else if (LOWORD(wParam) == 2) { // Cancelar
                DestroyWindow(hwnd);
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
    wc.lpfnWndProc = AgregarLibroProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "AgregarLibroWindow";
    RegisterClass(&wc);

    hwndAgregarLibro = CreateWindow("AgregarLibroWindow", "Agregar Libro",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        200, 200, 460, 400, hwndPadre, NULL, NULL, NULL);

    CreateWindow("STATIC", "ISBN:", WS_VISIBLE | WS_CHILD, 30, 30, 100, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtISBN = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 30, 250, 20, hwndAgregarLibro, NULL, NULL, NULL);

    CreateWindow("STATIC", "Título:", WS_VISIBLE | WS_CHILD, 30, 70, 100, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtTitulo = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 70, 250, 20, hwndAgregarLibro, NULL, NULL, NULL);

    CreateWindow("STATIC", "Autor:", WS_VISIBLE | WS_CHILD, 30, 110, 100, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtAutor = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 110, 250, 20, hwndAgregarLibro, NULL, NULL, NULL);

    CreateWindow("STATIC", "Editorial:", WS_VISIBLE | WS_CHILD, 30, 150, 100, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtEditorial = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 150, 250, 20, hwndAgregarLibro, NULL, NULL, NULL);

    CreateWindow("STATIC", "Año:", WS_VISIBLE | WS_CHILD, 30, 190, 100, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtAnio = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 190, 250, 20, hwndAgregarLibro, NULL, NULL, NULL);

    CreateWindow("STATIC", "Materia:", WS_VISIBLE | WS_CHILD, 30, 230, 100, 20, hwndAgregarLibro, NULL, NULL, NULL);
    txtMateria = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 140, 230, 250, 20, hwndAgregarLibro, NULL, NULL, NULL);

    CreateWindow("BUTTON", "Guardar", WS_VISIBLE | WS_CHILD,
        90, 300, 100, 30, hwndAgregarLibro, (HMENU)1, NULL, NULL);

    CreateWindow("BUTTON", "Cancelar", WS_VISIBLE | WS_CHILD,
        210, 300, 100, 30, hwndAgregarLibro, (HMENU)2, NULL, NULL);
}
