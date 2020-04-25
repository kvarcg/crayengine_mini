/*
File:  Global.h

Description: Global Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

#if defined (_WIN32) && defined (_MSC_VER)
#define WINMSVC
#endif

#if defined (_WIN32) && defined (_MSC_VER) && !defined(NDEBUG)
#define WINMSVCDEBUG
#endif

// this breaks Qt.Regardless, it is not needed anymore - To remove
/*
#ifdef WINMSVCDEBUG
#define my_malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define my_new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define _CRTDBG_MAP_ALLOC // this breaks Qt
#else
#define my_malloc malloc
#define my_new new
#endif // NDEBUG
*/