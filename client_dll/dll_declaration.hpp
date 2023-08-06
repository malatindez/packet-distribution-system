#pragma once
#ifdef CLIENT_DLL_EXPORTS
#define CLIENT_DLL_API __declspec(dllexport) 
#else
#define CLIENT_DLL_API __declspec(dllimport) 
#endif