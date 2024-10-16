// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include "vertex.hpp" // ToDO: define a vertex in a seperate file so it can be shared accross main.cpp and cow.cpp

extern std::vector<Vertex> vertices;
extern std::vector<unsigned int> indices;
extern unsigned int VAO, VBO, EBO;

#endif // GLOBALS_H