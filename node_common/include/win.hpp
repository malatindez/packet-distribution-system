#pragma once
#if defined (_WIN32)

#include "win-def.hpp"
#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "win-undef.hpp"

#endif