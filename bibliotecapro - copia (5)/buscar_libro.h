#ifndef BUSCAR_LIBRO_H
#define BUSCAR_LIBRO_H

#include <windows.h>

extern HWND txtBuscar;
extern HWND hwndBuscarLibro;

LRESULT CALLBACK BuscarLibroProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void CrearVentanaBuscarLibro(HWND hwndPadre);

#endif
