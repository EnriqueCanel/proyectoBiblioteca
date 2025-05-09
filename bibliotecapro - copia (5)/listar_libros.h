#ifndef LISTAR_LIBROS_H
#define LISTAR_LIBROS_H

#include <windows.h>
#include <string>
#include <vector>

struct Libro {
    std::string isbn;
    std::string titulo;
    std::string autor;
    std::string estado;
};

std::vector<Libro> obtenerTodosLosLibros();
void CrearVentanaListarLibros(HWND hwndPadre);

#endif // LISTAR_LIBROS_H
