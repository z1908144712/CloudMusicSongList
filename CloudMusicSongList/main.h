#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <direct.h>
#include <io.h>
#include "sqlite3.h"
#include "jsonxx/json.hpp"

sqlite3* db = nullptr;
std::vector<std::string> uidList;
std::vector<std::string> pidList;
std::vector<std::string> tidList;
std::map<std::string, std::vector<std::string>> uidListInfo;
std::map<std::string, std::vector<std::string>> pidListInfo;