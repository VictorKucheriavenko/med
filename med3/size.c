    #include <stdio.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <unistd.h>
    int main () {
        struct winsize ws;
        ioctl (STDOUT_FILENO, TIOCGWINSZ, &ws);
        printf ("Rows: %d, Cols: %d\n", ws.ws_row, ws.ws_col);
        return 0;
    }
