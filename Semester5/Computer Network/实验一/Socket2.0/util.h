#pragma once
#include "struct.h"
#include <iostream>
#include <fstream>
#include <sstream>
#define err(errMsg) printf("[line:%d]%s failed code %d\n",__LINE__,errMsg,WSAGetLastError())
#define ok(Msg) printf("%s Ok!\n\n",Msg)

Config read_conf(string config_path);
string dataHead(string contentType, int size);