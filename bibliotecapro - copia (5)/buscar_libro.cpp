#include "buscar_libro.h"
#include <windows.h>
#include <pqxx/pqxx>
#include <string>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

using namespace pqxx;

HWND txtBuscar, cbxCriterio, listView;
HWND hwndBuscarLibro;

void MostrarResultadosEnTabla(result& r, HWND hwnd) {
    if (!listView) return;

    // Limpiar columnas y filas si ya existen
    ListView_DeleteAllItems(listView);
    while (ListView_DeleteColumn(listView, 0));

    LVCOLUMN lvc = {0};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    std::string columnas[] = { "ISBN", "Título", "Autor", "Editorial", "Año", "Materia", "Estado", "Fecha movimiento" };

    for (int i = 0; i < 8; ++i) {
        lvc.pszText = const_cast<char*>(columnas[i].c_str());
        lvc.cx = 100;
        ListView_InsertColumn(listView, i, &lvc);
    }

    int index = 0;
    for (auto row : r) {
        LVITEM lvi = {0};
        lvi.mask = LVIF_TEXT;
        lvi.iItem = index;

        lvi.pszText = const_cast<char*>(row["isbn"].c_str());
        ListView_InsertItem(listView, &lvi);

        ListView_SetItemText(listView, index, 1, const_cast<char*>(row["titulo"].c_str()));
        ListView_SetItemText(listView, index, 2, const_cast<char*>(row["autor"].c_str()));
        ListView_SetItemText(listView, index, 3, const_cast<char*>(row["editorial"].c_str()));
        ListView_SetItemText(listView, index, 4, const_cast<char*>(row["anio_publicacion"].c_str()));
        ListView_SetItemText(listView, index, 5, const_cast<char*>(row["materia"].c_str()));
        ListView_SetItemText(listView, index, 6, const_cast<char*>(row["estado"].c_str()));
        ListView_SetItemText(listView, index, 7, const_cast<char*>(row["fecha_movimiento"].c_str()));

        index++;
    }
}

LRESULT CALLBACK BuscarLibroProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == 1) { // Buscar
                char criterio[100], campo[10];
                GetWindowText(txtBuscar, criterio, 100);
                int index = SendMessage(cbxCriterio, CB_GETCURSEL, 0, 0);
                SendMessage(cbxCriterio, CB_GETLBTEXT, index, (LPARAM)campo);

                if (strlen(criterio) == 0) {
                    MessageBox(hwnd, "Por favor, introduzca un criterio de búsqueda.", "Error", MB_OK | MB_ICONWARNING);
                    break;
                }

                std::string columna = (strcmp(campo, "Título") == 0) ? "titulo" : "autor";

                try {
                    connection conn("dbname=biblioteca user=postgres password=1992 hostaddr=127.0.0.1 port=5432");
                    work txn(conn);
                    std::string query = "SELECT * FROM libross WHERE " + columna + " ILIKE $1";
                    result r = txn.exec_params(query, "%" + std::string(criterio) + "%");
                    txn.commit();

                    if (r.empty()) {
                        MessageBox(hwnd, "No se encontró información.", "Resultado", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MostrarResultadosEnTabla(r, hwnd);
                    }

                } catch (const std::exception& e) {
                    MessageBoxA(hwnd, e.what(), "Error", MB_OK | MB_ICONERROR);
                }

            } else if (LOWORD(wParam) == 2) { // Cancelar
                DestroyWindow(hwnd);
            }
            break;

        case WM_DESTROY:
            hwndBuscarLibro = NULL;
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CrearVentanaBuscarLibro(HWND hwndPadre) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = BuscarLibroProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "BuscarLibroWindow";
    RegisterClass(&wc);

    RECT rc;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
    int width = 900, height = 400;
    int x = (rc.right - width) / 2;
    int y = (rc.bottom - height) / 2;

    hwndBuscarLibro = CreateWindow("BuscarLibroWindow", "Buscar Libro",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        x, y, width, height, hwndPadre, NULL, NULL, NULL);

    CreateWindow("STATIC", "Buscar por:", WS_VISIBLE | WS_CHILD, 30, 20, 80, 20, hwndBuscarLibro, NULL, NULL, NULL);

    cbxCriterio = CreateWindow("COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
        120, 20, 120, 200, hwndBuscarLibro, NULL, NULL, NULL);
    SendMessage(cbxCriterio, CB_ADDSTRING, 0, (LPARAM)"Título");
    SendMessage(cbxCriterio, CB_ADDSTRING, 0, (LPARAM)"Autor");
    SendMessage(cbxCriterio, CB_SETCURSEL, 0, 0);

    txtBuscar = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
        260, 20, 250, 20, hwndBuscarLibro, NULL, NULL, NULL);

    CreateWindow("BUTTON", "Buscar", WS_VISIBLE | WS_CHILD,
        530, 18, 100, 25, hwndBuscarLibro, (HMENU)1, NULL, NULL);

    CreateWindow("BUTTON", "Cancelar", WS_VISIBLE | WS_CHILD,
        640, 18, 100, 25, hwndBuscarLibro, (HMENU)2, NULL, NULL);

    listView = CreateWindow(WC_LISTVIEW, "",
        WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL,
        30, 60, 820, 280, hwndBuscarLibro, NULL, NULL, NULL);
}
