#ifndef OBJECTINTOEXE_H
#define OBJECTINTOEXE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Link an object file into an executable.
 *
 * @param obj_path Path to the object file (e.g. "out.o").
 * @param exe_path Path to the output executable (e.g. "a.exe").
 * @return 0 on success, nonzero on failure.
 */
int object_to_exe(const char *obj_path, const char *exe_path);

#ifdef __cplusplus
}
#endif

#endif // OBJECTINTOEXE_H
