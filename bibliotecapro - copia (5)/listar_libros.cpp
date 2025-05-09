#include "listar_libros.h"
#include <windows.h>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <commctrl.h>
#include <algorithm>

using namespace pqxx;

#define ID_BOTON_CERRAR 1001
#define ID_LISTVIEW 1002

std::vector<Libro> librosGlobal;
int columnaOrdenActual = -1;
bool ordenAscendente = true;

std::vector<Libro> obtenerTodosLosLibros() {
    std::vector<Libro> libros;
    try {
        connection conn("dbname=biblioteca user=postgres password=1992 hostaddr=127.0.0.1 port=5432");
        work txn(conn);
        result r = txn.exec("SELECT isbn, titulo, autor, estado FROM libross ORDER BY titulo ASC");

        for (auto row : r) {
            Libro libro;
            libro.isbn = row["isbn"].as<std::string>();
            libro.titulo = row["titulo"].as<std::string>();
            libro.autor = row["autor"].as<std::string>();
            libro.estado = row["estado"].as<std::string>();
            libros.push_back(libro);
        }
        txn.commit();
    } catch (const std::exception &e) {
        MessageBoxA(NULL, e.what(), "Error", MB_OK | MB_ICONERROR);
    }
    return libros;
}

void llenarListView(HWND hListView) {
    ListView_DeleteAllItems(hListView);

    for (size_t i = 0; i < librosGlobal.size(); ++i) {
        LVITEM item = {};
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.pszText = (LPSTR)librosGlobal[i].isbn.c_str();
        ListView_InsertItem(hListView, &item);
        ListView_SetItemText(hListView, i, 1, (LPSTR)librosGlobal[i].titulo.c_str());
        ListView_SetItemText(hListView, i, 2, (LPSTR)librosGlobal[i].autor.c_str());
        ListView_SetItemText(hListView, i, 3, (LPSTR)librosGlobal[i].estado.c_str());
    }
}

void ordenarPorColumna(int columna) {
    auto cmp = [columna](const Libro& a, const Libro& b) {
        const std::string* campoA = nullptr;
        const std::string* campoB = nullptr;

        switch (columna) {
            case 0: campoA = &a.isbn; campoB = &b.isbn; break;
            case 1: campoA = &a.titulo; campoB = &b.titulo; break;
            case 2: campoA = &a.autor; campoB = &b.autor; break;
            case 3: campoA = &a.estado; campoB = &b.estado; break;
        }

        if (campoA && campoB) {
            return ordenAscendente ? *campoA < *campoB : *campoA > *campoB;
        }
        return false;
    };
    std::sort(librosGlobal.begin(), librosGlobal.end(), cmp);
}

LRESULT CALLBACK ListarLibrosWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hListView;

    switch (msg) {
    case WM_CREATE: {
        InitCommonControls();

        hListView = CreateWindow(WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
                                 10, 10, 860, 380, hwnd, (HMENU)ID_LISTVIEW, GetModuleHandle(NULL), NULL);

        LVCOLUMN lvc = {};
        lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

        const char* headers[] = {"ISBN", "Título", "Autor", "Estado"};
        int widths[] = {100, 150, 120, 80};

        for (int i = 0; i < 4; ++i) {
            lvc.pszText = (LPSTR)headers[i];
            lvc.cx = widths[i];
            lvc.iSubItem = i;
            ListView_InsertColumn(hListView, i, &lvc);
        }

        librosGlobal = obtenerTodosLosLibros();
        llenarListView(hListView);

        CreateWindow("BUTTON", "Cerrar", WS_VISIBLE | WS_CHILD, 400, 400, 100, 30,
                     hwnd, (HMENU)ID_BOTON_CERRAR, GetModuleHandle(NULL), NULL);
        break;
    }

    case WM_NOTIFY: {
        LPNMHDR lpnmh = (LPNMHDR)lParam;

        if (lpnmh->idFrom == ID_LISTVIEW) {
            if (lpnmh->code == LVN_COLUMNCLICK) {
                NMLISTVIEW* pnmv = (NMLISTVIEW*)lParam;
                int col = pnmv->iSubItem;

                // Verifica si la columna clickeada es la misma que la última columna ordenada
                if (col == columnaOrdenActual) {
                    ordenAscendente = !ordenAscendente; // Invertir orden si la misma columna es clickeada
                } else {
                    columnaOrdenActual = col;
                    ordenAscendente = true; // Por defecto, ordenar de forma ascendente
                }

                ordenarPorColumna(col);
                llenarListView(GetDlgItem(hwnd, ID_LISTVIEW));
            }
            else if (lpnmh->code == NM_DBLCLK) {
                // Ignorar doble clic (no hacer nada)
                return 0;
            }
        }
        break;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BOTON_CERRAR) {
            // Solo cierra la ventana actual, no el programa completo
            DestroyWindow(hwnd);
        }
        break;

    case WM_DESTROY:
        // No cerrar todo el programa, solo la ventana de listado
        PostMessage(hwnd, WM_CLOSE, 0, 0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CrearVentanaListarLibros(HWND hwndPadre) {
    static bool registrado = false;
    if (!registrado) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = ListarLibrosWndProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = "ListarLibrosWindow";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        RegisterClass(&wc);
        registrado = true;
    }

    RECT rect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
    int width = 900, height = 500;
    int x = (rect.right - rect.left - width) / 2;
    int y = (rect.bottom - rect.top - height) / 2;

    CreateWindow("ListarLibrosWindow", "Listado de Libros", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                 x, y, width, height, hwndPadre, NULL, GetModuleHandle(NULL), NULL);
}
