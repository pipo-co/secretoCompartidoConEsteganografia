#include "args/args.h"
#include "log/log.h"

#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <limits.h>    /* LONG_MIN et al */
#include <string.h>    /* memset */
#include <errno.h>
#include <getopt.h>

#define MIN_ARG_COUNT  5 // 1 + 4

static void version(void);
static void usage(const char *progname);
static int get_non_option_args(const int argc, char **argv, Args *args);


bool args_parse(const int argc, char **argv, Args *args) {
    
    if(args == NULL) {
        LOG_FATAL("Recieved args NULL.");
        return false;
    }

    memset(args, 0, sizeof(*args)); // sobre todo para setear en null los punteros de users

    // Default values
    args->shadesOutputDir   = NULL;
    args->padding           = false;

    args->loglevel          = LOG_LEVEL_FATAL;
    args->logQuiet          = false;
    args->logVerbose        = false;
    args->galoisGen         = 0;

    int c;
    
    while (true) {

        int option_index = 0;
        static struct option long_options[] = {
            { "version",   no_argument,         0, 0xD001 },
            { 0,           0,                   0, 0 }
        };

        c = getopt_long(argc, argv, "-ho:pvdg:", long_options, &option_index);
        
        if (c == -1)
            break;

        switch (c) {
            case 'h':
                usage(argv[0]);
                exit(EXIT_SUCCESS);
            case 'o':
                args->shadesOutputDir = optarg;
                break;
            case 'p':
                args->padding = true;
                break;
            case 'v':
                if(args->loglevel > LOG_LEVEL_INFO) {
                    args->loglevel = LOG_LEVEL_INFO;
                }
                break;
            case 'd':
                if(args->loglevel > LOG_LEVEL_DEBUG) {
                    args->loglevel = LOG_LEVEL_DEBUG;
                }
                args->logVerbose = true;
                break;
            case 'g':
                args->galoisGen = atoi(optarg);
                break;
            case 0xD001:
                version();
                exit(EXIT_SUCCESS);
            case 1:
                // non-option
                break;
            
            default:
                LOG_FATAL("Unknown argument %d.\n", c);
                return false;
        }
    }

    get_non_option_args(argc, argv, args);

    if(args->shadesOutputDir == NULL) {
        args->shadesOutputDir = args->shadowsDir;
    }

    log_set_level(args->loglevel);
    log_set_quiet(args->logQuiet);
    log_set_verbose(args->logVerbose);

    return true;
}

static int get_non_option_args(const int argc, char **argv, Args *args) {

    if (argc < MIN_ARG_COUNT) {
        usage(argv[0]);
        return -1;
    }

    args->action = argv[1][0];

    if (args->action != DISTRIBUTE && args->action != RECOVER) {
        LOG_FATAL("Invalid first param. Available options are [%c] for distributing the secret and [%c] to recover it.", DISTRIBUTE, RECOVER);
        return -1;
    }

    args->secretImage = argv[2];
    
    int tmpK = atoi(argv[3]);

    if(tmpK > UINT8_MAX) {
        LOG_FATAL("Invalid third param. K value should be lower or equal than %d.", UINT8_MAX);
        return -1;
    }

    args->k = tmpK;

    args->shadowsDir = argv[4];

    return MIN_ARG_COUNT - 1;
}

static void version(void) {
    LOG_INFO(
        "\n"
        "   Shared secret utility\n"
        "   ITBA Criptografia y Seguridad 2021/1 -- Grupo 18\n"
        "   Alumnos:\n"
        "       - Brandy, Tobias\n"
        "       - Pannunzio, Faustino\n"
        "       - Sagues, Ignacio\n");
}

static void usage(const char *progname) {
    LOG_INFO(
        "\n"
        "Usage: %s d|r secret_image_relative_path k_value shadows_dir_path [OPTION]...\n"
        "\n"
        "   -h                          Show usage.\n"
        "   -o <shades output dir>      Only available on distribute mode.\n"
        "   -p                          Enable secret padding. Use padding if the secret is not a divisible by `k' instead of failing\n"
        "   -v                          Verbose. Logs about program status.\n"
        "   -d                          Debug. Detailed error logs.\n"
        "   -g <generator poly>         Define a custom generator poly for galois operations.\n"
        "\n"
        "   --version                   Show version and info.\n"
        "\n",
        progname);
}
