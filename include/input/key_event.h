#pragma once

#include <string>
#include <iostream>

struct KeyEvent
{
    enum Status
    {
        PRESSED,
        RELEASED
    };

    KeyEvent(const char symbol, Status status) : symbol(symbol), status(status){};

    char symbol;
    Status status;
};
