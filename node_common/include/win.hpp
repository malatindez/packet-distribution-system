#pragma once
#if defined (__WIN32__)

#include "win-def.hpp"
#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "win-undef.hpp"

#endif