#include <stdio.h>
#include <stdlib.h>

#include "util.h"

#include "BasicTriangle.h"

int main(int argc, char *argv[])
{
    Window_Init(800, 800, BasicTriangle_Init, BasicTriangle_Update);
    Window_Loop();

    return 0;
}
