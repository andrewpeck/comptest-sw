#ifndef COLORS_H
#define COLORS_H 

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BOLD          "\033[1m"
#define ANSI_RESET         "\033[0m"

#define ERROR    ANSI_COLOR_RED "error" ANSI_RESET
#define COMPLETED ANSI_COLOR_GREEN "completed" ANSI_RESET
#define FAILED ANSI_COLOR_RED "failed" ANSI_RESET


#endif /* COLORS_H */
