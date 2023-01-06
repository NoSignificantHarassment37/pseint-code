#ifndef LOAD_H
#define LOAD_H
#include <cstddef>
#include <string>
using namespace std;
class Entity;
bool Save(std::string filename = "");
bool Load(std::string filename = "");
void SetProc(Entity *proc);
void CreateEmptyProc(string type);
void New();
#endif

