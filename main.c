#include <stdio.h>
#include <stdlib.h>

#include "Window.h"

int main(int argc, char *argv[])
{
    Window_Init(1280, 720);
    Window_Loop();

    return 0;
}
