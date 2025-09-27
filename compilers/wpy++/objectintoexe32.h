#ifndef OBJECTINTOEXE32_H
#define OBJECTINTOEXE32_H

// Link a 32-bit object file into a 32-bit Windows executable.
// Returns 0 on success, nonzero on failure.
int object_to_exe32(const char *obj_path, const char *exe_path);

#endif // OBJECTINTOEXE32_H
