/* C wrapper for statx - get file status (extended)) */

#define _GNU_SOURCE /* See feature_test_macros(7) */
#include <argp.h>
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

/* Program documentation. */
static char doc[] = "C wrapper for statx - get file status (extended)";

/* A description of the arguments we accept. */
static char args_doc[] = "file";

/* The options we understand. */
static struct argp_option options[] = {
    {"atime", 'a', 0, OPTION_ARG_OPTIONAL,
     "Display file access time only.\nFormat: sec.nsec (Epoch)"},
    {"btime", 'b', 0, OPTION_ARG_OPTIONAL,
     "Display file birth time only.\nFormat: sec.nsec (Epoch)"},
    {"ctime", 'c', 0, OPTION_ARG_OPTIONAL,
     "Display file change time only.\nFormat: sec.nsec (Epoch)"},
    {"mtime", 'm', 0, OPTION_ARG_OPTIONAL,
     "Display file modification time only.\nFormat: sec.nsec (Epoch)"},
    {0}};

/* Used by main to communicate with parse_opt. */
struct arguments {
    char *args[1]; /* file */
    char atime;    /* atime */
    char btime;    /* btime */
    char ctime;    /* ctime */
    char mtime;    /* mtime */
};

/* Parse a single option. */
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;
    int nargs = sizeof(arguments->args) / sizeof(arguments->args[0]);
    int opts;

    switch (key) {
    case 'a':
        arguments->atime = 1;
        break;
    case 'b':
        arguments->btime = 1;
        break;
    case 'c':
        arguments->ctime = 1;
        break;
    case 'm':
        arguments->mtime = 1;
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num >= nargs) {
            fprintf(stderr, "Error: Too many arguments.\n");
            argp_usage(state);
        }
        arguments->args[state->arg_num] = arg;
        break;
    case ARGP_KEY_END:
        if (state->arg_num < nargs) {
            fprintf(stderr, "Error: Not enough arguments.\n");
            argp_usage(state);
        }
        opts = arguments->atime + arguments->btime + arguments->ctime +
               arguments->mtime;
        if (opts > 1) {
            fprintf(stderr, "Error: --atime, --btime, --ctime and --mtime are "
                            "mutually exclusive.\n");
            argp_usage(state);
        }

        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Our argp parser. */
static struct argp argp = {options, parse_opt, args_doc, doc};

void _print_stx_time(struct statx_timestamp stx_time) {
    printf("%" PRIu64 ".%" PRIu32 "\n", stx_time.tv_sec, stx_time.tv_nsec);
}

int main(int argc, char **argv) {
    struct arguments arguments;
    struct statx statxbuf;
    struct tm *tmtime;
    char tmbuf[_STATX_C_MAX_STRFTIME];
    long ret;

    /* Default values. */
    arguments.args[0] = 0;
    arguments.atime = 0;
    arguments.btime = 0;
    arguments.ctime = 0;
    arguments.mtime = 0;

    /* Parse our arguments; every option seen by parse_opt will
       be reflected in arguments. */
    ret = argp_parse(&argp, argc, argv, 0, 0, &arguments);
    if (ret) {
        perror("Error");
        return ret;
    }

    /* Syscall to statx */
    ret = syscall(SYS_statx, AT_FDCWD, arguments.args[0],
                  AT_STATX_SYNC_AS_STAT | AT_EMPTY_PATH, STATX_ALL, &statxbuf);
    if (ret) {
        perror(arguments.args[0]);
        return ret;
    }

    /* Display modes */
    if (arguments.atime) {
        _print_stx_time(statxbuf.stx_atime);
    } else if (arguments.btime) {
        _print_stx_time(statxbuf.stx_btime);
    } else if (arguments.ctime) {
        _print_stx_time(statxbuf.stx_ctime);
    } else if (arguments.mtime) {
        _print_stx_time(statxbuf.stx_mtime);
    } else {
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
        printf("stx_attributes_mask: %" PRIu64 "\n",
               statxbuf.stx_attributes_mask);

        printf("stx_atime: %");
        tmtime = localtime(&(statxbuf.stx_atime.tv_sec));
        strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", tmtime);
        if (statxbuf.stx_atime.tv_sec == 0) {
            printf("-\n");
        } else {
            printf("%s.%" PRIu32 "\n", tmbuf, statxbuf.stx_atime.tv_nsec);
        }

        printf("stx_btime: %");
        tmtime = localtime(&(statxbuf.stx_btime.tv_sec));
        strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", tmtime);
        if (statxbuf.stx_btime.tv_sec == 0) {
            printf("-\n");
        } else {
            printf("%s.%" PRIu32 "\n", tmbuf, statxbuf.stx_btime.tv_nsec);
        }

        printf("stx_ctime: %");
        tmtime = localtime(&(statxbuf.stx_ctime.tv_sec));
        strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", tmtime);
        if (statxbuf.stx_ctime.tv_sec == 0) {
            printf("-\n");
        } else {
            printf("%s.%" PRIu32 "\n", tmbuf, statxbuf.stx_ctime.tv_nsec);
        }

        printf("stx_mtime: %");
        tmtime = localtime(&(statxbuf.stx_mtime.tv_sec));
        strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", tmtime);
        if (statxbuf.stx_mtime.tv_sec == 0) {
            printf("-\n");
        } else {
            printf("%s.%" PRIu32 "\n", tmbuf, statxbuf.stx_mtime.tv_nsec);
        }

        printf("stx_rdev_major: %" PRIu32 "\n", statxbuf.stx_rdev_major);
        printf("stx_rdev_minor: %" PRIu32 "\n", statxbuf.stx_rdev_minor);

        printf("stx_dev_major: %" PRIu32 "\n", statxbuf.stx_dev_major);
        printf("stx_dev_minor: %" PRIu32 "\n", statxbuf.stx_dev_minor);
    }

    return 0;
}
