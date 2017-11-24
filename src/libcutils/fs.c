/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "cutils"

/* These defines are only needed because prebuilt headers are out of date */
#define __USE_XOPEN2K8 1
#define _ATFILE_SOURCE 1
#define _GNU_SOURCE 1

//#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <io.h>
#include <direct.h>

#include <cutils/fs.h>
//#include <log/log.h>
#define AT_SYMLINK_NOFOLLOW 0x100
#define S_ISDIR(m) (((m) & 0170000) == (0040000)) 
#define S_ISREG(m) (((m) & 0170000) == (0100000))


#define ALL_PERMS 0xFFFFFFFF//(S_ISUID | S_ISGID | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO)
#define BUF_SIZE 64

static int fs_prepare_path_impl(const char* path, unsigned int mode, int uid, int gid,
        int allow_fixup, int prepare_as_dir) {
    // Check if path needs to be created
    struct stat sb;
    int create_result = -1;
    if (stat(path, &sb) == -1) {
        if (errno == ENOENT) {
            goto create;
        } else {
            return -1;
        }
    }

    // Exists, verify status
    int type_ok = prepare_as_dir ? S_ISDIR(sb.st_mode) : S_ISREG(sb.st_mode);
    if (!type_ok) {
        return -1;
    }

    int owner_match = ((sb.st_uid == uid) && (sb.st_gid == gid));
    int mode_match = ((sb.st_mode & ALL_PERMS) == mode);
    if (owner_match && mode_match) {
        return 0;
    } else if (allow_fixup) {
        goto fixup;
    } else {
        if (!owner_match) {
            return -1;
        } else {
            return 0;
        }
    }

create:
    create_result = prepare_as_dir
        ? _mkdir(path)
        : open(path, O_CREAT | O_RDONLY, 0644);
    if (create_result == -1) {
        if (errno != EEXIST) {
            return -1;
        }
    } else if (!prepare_as_dir) {
        // For regular files we need to make sure we close the descriptor
        if (close(create_result) == -1) {
        }
    }
fixup:
    if (chmod(path, mode) == -1) {
        return -1;
    }
	/*
    if (chown(path, uid, gid) == -1) {
        return -1;
    }
	*/
    return 0;
}

int fs_prepare_dir(const char* path, unsigned int mode, int uid, int gid) {
    return fs_prepare_path_impl(path, mode, uid, gid, /*allow_fixup*/ 1, /*prepare_as_dir*/ 1);
}

int fs_prepare_dir_strict(const char* path, unsigned int mode, int uid, int gid) {
    return fs_prepare_path_impl(path, mode, uid, gid, /*allow_fixup*/ 0, /*prepare_as_dir*/ 1);
}

int fs_prepare_file_strict(const char* path, int mode, int uid, int gid) {
    return fs_prepare_path_impl(path, mode, uid, gid, /*allow_fixup*/ 0, /*prepare_as_dir*/ 0);
}

int fs_read_atomic_int(const char* path, int* out_value) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return -1;
    }

    char buf[BUF_SIZE];
    if (read(fd, buf, BUF_SIZE) == -1) {
        goto fail;
    }
    if (sscanf(buf, "%d", out_value) != 1) {
        goto fail;
    }
    close(fd);
    return 0;

fail:
    close(fd);
    *out_value = -1;
    return -1;
}

int fs_write_atomic_int(const char* path, int value) {
    char temp[256];
    if (snprintf(temp, 256, "%s.XXXXXX", path) >= 256) {
        return -1;
    }

    int fd = mkstemp(temp);
    if (fd == -1) {
        return -1;
    }

    char buf[BUF_SIZE];
    int len = snprintf(buf, BUF_SIZE, "%d", value) + 1;
    if (len > BUF_SIZE) {
        goto fail;
    }
    if (write(fd, buf, len) < len) {
        goto fail;
    }
    if (close(fd) == -1) {
        ALOGE("Failed to close %s: %s", temp, strerror(errno));
        goto fail_closed;
    }

    if (rename(temp, path) == -1) {
        ALOGE("Failed to rename %s to %s: %s", temp, path, strerror(errno));
        goto fail_closed;
    }

    return 0;

fail:
    close(fd);
fail_closed:
    unlink(temp);
    return -1;
}

#ifndef __APPLE__
#include <windows.h>

int createDirectory(const char *path) {
	char *tmp = (char *)path;
	while (*tmp) {
		if (*tmp == '\\') {
			*tmp = '\0';
			 unsigned int fileAttr = GetFileAttributesA((const char *)path);
			 if (fileAttr == 0xFFFFFFFF || !(fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
				 if (!CreateDirectoryA((const char *)path, NULL)) {
					 return -1;
				 }
			 }
			 *tmp = '\\';
		}
		++tmp;
	}
	return 0;
}


int fs_mkdirs(const char* path, unsigned int mode) {
	return createDirectory(path);
}

#endif
