/*
** Copyright (C) 2018 Daniel Dunn
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of
** this software and associated documentation files (the "Software"), to deal in
** the Software without restriction, including without limitation the rights to
** use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
** the Software, and to permit persons to whom the Software is furnished to do so,
** subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
** FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
** COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
** IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once
extern "C"
{
#include "my_basic.h"
}

class _MyBasic
{

    public:
    void begin(char);
    void yield();
    int updateProgram(const char *, const char *);
    void runLoaded(const char *);
    int loadProgram(const char *, const char *);

    int appendInput(const char * data, int len,const char * id);

    mb_interpreter_t * rootInterpreter;
    
    mb_interpreter_t * getInterpreter(const char * id);
    void lock();
    void unlock();

    int stackSize = 2048;

};

extern _MyBasic MyBasic;
void enableArduinoBindings(struct mb_interpreter_t* bas);
