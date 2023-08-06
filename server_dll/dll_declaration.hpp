#pragma once
#ifdef SERVER_DLL_EXPORTS
#define SERVER_DLL_API __declspec(dllexport) 
#else
#define SERVER_DLL_API __declspec(dllimport) 
#endif