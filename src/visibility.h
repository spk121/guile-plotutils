#ifndef VISIBILITY_H
#define VISIBILITY_H

#if defined _WIN32 || defined __CYGWIN__
#define GUPL_HELPER_DLL_IMPORT __declspec(dllimport)
#define GUPL_HELPER_DLL_EXPORT __declspec(dllexport)
#define GUPL_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define GUPL_HELPER_DLL_IMPORT __attribute__ ((visibility("default")))
#define GUPL_HELPER_DLL_EXPORT __attribute__ ((visibility("default")))
#define GUPL_HELPER_DLL_LOCAL  __attribute__ ((visibility("hidden")))
#else
#define GUPL_HELPER_DLL_IMPORT
#define GUPL_HELPER_DLL_EXPORT
#define GUPL_HELPER_DLL_LOCAL
#endif
#endif

#ifdef GUPL_DLL			/* defined if GUPL is compiled as a DLL */
#ifdef GUPL_DLL_EXPORTS		/* defined if we are building the GUPL DLL */
#define GUPL_API GUPL_HELPER_DLL_EXPORT
#else
#define GUPL_API GUPL_HELPER_DLL_IMPORT
#endif /* GUPL_DLL_EXPORTS */
#define GUPL_LOCAL GUPL_HELPER_DLL_LOCAL
#else /* GUPL_DLL is not defined: this means GUPL is a static lib. */
#define GUPL_API
#define GUPL_LOCAL
#endif

#endif
