// src/main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encoder.h"
#include "utils.h"
#include "keccak.h"
#include "json_input.h"
#include "cli.h"
#include "gui.h"

/*
 * Mini ABI Encoder – CLI + GUI launcher
 */
int main(int argc, char *argv[])
{
    int only_selector = 0;
    int pretty        = 0;
    int json_out      = 0;
    int interactive   = 0;

    /* 👇 اگر --gui دیده شد، قبل از تحویل argv به GTK آن را حذف می‌کنیم
     *    تا خطای «Unknown option --gui» رخ ندهد.  */
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--gui") == 0) {
            for (int k = i; k < argc - 1; ++k)
                argv[k] = argv[k + 1];   // شیفت به چپ
            --argc;
            return launch_gui(argc, argv);
        }
    }

    /* --help */
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_help();
            return 0;
        }
    }

    /* … باقی کد CLI شما همانی است که قبلاً داشتید …
       شامل parse ورودی‌ها، فراخوانی encode()‎ و چاپ خروجی.
       هیچ تغییری در منطقش ندادم، فقط بلوک --gui را اصلاح کردیم. */

    return 0;
}
