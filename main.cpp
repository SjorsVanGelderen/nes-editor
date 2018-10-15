// Copyright 2018, Sjors van Gelderen

#include "app.h"
#include "debug.h"

int main()
{    
    auto status = App::Start();

    Debug::LogStatus(status);
    
    return status;
}
