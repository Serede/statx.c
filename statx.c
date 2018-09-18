/* statx - get file status (extended) */

#define _GNU_SOURCE /* See feature_test_macros(7) */
#include <fcntl.h>
#include <inttypes.h> /* For printf */
#include <linux/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/syscall.h> /* For SYS_xxx definitions */
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define _STATX_C_MAX_STRFTIME 128

int main(int argc, char const *argv[]) {
    struct statx statxbuf;
    struct tm *tmtime;
    char tmbuf[_STATX_C_MAX_STRFTIME];
    long ret;

    ret = syscall(SYS_statx, AT_FDCWD, argv[1],
                  AT_STATX_SYNC_AS_STAT | AT_EMPTY_PATH, STATX_ALL, &statxbuf);
    if (ret) {
        return ret;
    }

    printf("stx_mask: %" PRIu32 "\n", statxbuf.stx_mask);
    printf("stx_blksize: %" PRIu32 "\n", statxbuf.stx_blksize);
    printf("stx_attributes: %" PRIu64 "\n", statxbuf.stx_attributes);
    printf("stx_nlink: %" PRIu32 "\n", statxbuf.stx_nlink);
    printf("stx_uid: %" PRIu32 "\n", statxbuf.stx_uid);
    printf("stx_gid: %" PRIu32 "\n", statxbuf.stx_gid);
    printf("stx_mode: %" PRIu16 "\n", statxbuf.stx_mode);
    printf("stx_ino: %" PRIu64 "\n", statxbuf.stx_ino);
    printf("stx_size: %" PRIu64 "\n", statxbuf.stx_size);
    printf("stx_blocks: %" PRIu64 "\n", statxbuf.stx_blocks);
    printf("stx_attributes_mask: %" PRIu64 "\n", statxbuf.stx_attributes_mask);

    printf("stx_atime: %");
    tmtime = localtime(&(statxbuf.stx_atime.tv_sec));
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", tmtime);
    printf("%s.%" PRIu32 "\n", tmbuf, statxbuf.stx_atime.tv_nsec);

    printf("stx_btime: %");
    tmtime = localtime(&(statxbuf.stx_btime.tv_sec));
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", tmtime);
    printf("%s.%" PRIu32 "\n", tmbuf, statxbuf.stx_btime.tv_nsec);

    printf("stx_ctime: %");
    tmtime = localtime(&(statxbuf.stx_ctime.tv_sec));
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", tmtime);
    printf("%s.%" PRIu32 "\n", tmbuf, statxbuf.stx_ctime.tv_nsec);

    printf("stx_mtime: %");
    tmtime = localtime(&(statxbuf.stx_mtime.tv_sec));
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", tmtime);
    printf("%s.%" PRIu32 "\n", tmbuf, statxbuf.stx_mtime.tv_nsec);

    printf("stx_rdev_major: %" PRIu32 "\n", statxbuf.stx_rdev_major);
    printf("stx_rdev_minor: %" PRIu32 "\n", statxbuf.stx_rdev_minor);

    printf("stx_dev_major: %" PRIu32 "\n", statxbuf.stx_dev_major);
    printf("stx_dev_minor: %" PRIu32 "\n", statxbuf.stx_dev_minor);

    return 0;
}
