#ifndef PGMLINK_EXPORT_H
#define PGMLINK_EXPORT_H

#ifdef _WIN32
	#ifdef pgmlink_EXPORTS
		#define PGMLINK_EXPORT __declspec(dllexport)
	#elif defined(PGMLINK_STATIC_LIB)
		#define PGMLINK_EXPORT
	#else
		#define PGMLINK_EXPORT __declspec(dllimport)
	#endif
#else
	#define PGMLINK_EXPORT
#endif

#endif /* PGMLINK_EXPORT_H */

