#ifndef PCH_H
#define PCH_H

#include <string>
#include <sstream>
#include <iostream>

#include <functional>

#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include <utility>
#include <algorithm>

#include <filesystem>

#include "core/logger.h"
#include "core/buffer.h"

#include "debug/instrumentor.h"

#ifdef DN_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#endif