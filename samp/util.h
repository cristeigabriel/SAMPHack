#pragma once

#include <utility>
#include <cstdint>

#define FWD(x) std::forward<decltype(x)>(x)
#ifdef _DEBUG
#define LOG(x) std::wcout << x
#else
#define LOG(x)
#endif
