
 tcgetattr(0, &term);
 tcgetattr(0, &restore);
 term.c_lflag &= ~(ICANON | ECHO);
 tcsetattr(0, TCSANOW, &term);
