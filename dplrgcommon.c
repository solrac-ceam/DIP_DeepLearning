#include "dplrgcommon.h"

int remove_directory(const char *path)
{
    DIR *dir = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;
    struct dirent *entry;
    int r2 = -1;
    char *buf;
    size_t len;
    struct stat statbuf;

    if (dir != NULL) {
        r = 0;

        while (!r && (entry=readdir(dir))) {
            r2 = -1;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            {
                continue;
            }

            len = path_len + strlen(entry->d_name) + 2;
            buf = malloc(len);

            if (buf != NULL) {

                snprintf(buf, len, "%s/%s", path, entry->d_name);

                if (stat(buf, &statbuf) == 0) {
                    if (S_ISDIR(statbuf.st_mode)) {
                        r2 = remove_directory(buf);
                    }
                    else {
                        r2 = unlink(buf);
                    }
                }

                free(buf);
            }

            r = r2;
        }

        closedir(dir);
    }

    if (!r)
    {
        r = rmdir(path);
    }

    return r;
}
