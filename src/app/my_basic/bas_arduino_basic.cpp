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

#include "Arduino.h"
#include "bas_arduino.h"

#define dbg(x) Serial.println(x)
#define dbg 

//The high level OOP API
_MyBasic MyBasic;

//Wait 10 million ticks which is probably days, but still assert it if it fails
#define MB_LOCK assert(xSemaphoreTake(bas_gil,10000000))
#define MB_UNLOCK xSemaphoreGive(bas_gil)

//This is a set of queues
static QueueHandle_t request_queue;

//Allow up to 8 tasks
static TaskHandle_t basTasks[8];

//This is the queue used to request that a basic interpreter thread does something
static QueueHandle_t requestqueue;

//List of the interpreters for all loaded programs
static mb_interpreter_t *loadedPrograms[16];

//The root basic interpreter, passing a null pointer when getting by ID returns this
mb_interpreter_t* bas_parent = NULL;

//The global interpreter lock. Almost any messing
//with of interpreters uses this.
SemaphoreHandle_t bas_gil;


//The userdata struct for each loadedProgram interpreter
struct loadedProgram
{
  //This is how we can know which program to replace when updating with a new version
  char programID[16];
  //The first 30 bytes of a file identify its "version" so we don't
  //replace things that don't need replacing.
  char hash[30];


  //This is the input buffer that gives us an easy way to send things to a program
  //in excess of the 1500 byte limit for UDP. We might also use it for other stuff later.
  char * inputBuffer;

  //How many bytes are in the input buffer.
  int inputBufferLen;

  //1  or above if the program is busy, don't mess with it in any way except setting/getting vars and making sub-programs.
  //0 means you can delete, replace, etc

  //When a child interpreter runs, it increments all parents and itself.
  //In this way it is kind of like a reference count.
  char busy; 
};





//Given a string program ID, return the mb interpreter for it, or 0,
//If it's not loaded.
static mb_interpreter_t** _programForId(const char * id)
{
  struct loadedProgram * ud;
  if (id == 0)
  {
    if (bas_parent)
    {
      return &bas_parent;
    }
    else
    {
      return 0;
    }
  }
  for (char i = 0; i < 16; i++)
  {
    if (loadedPrograms[i])
    {
      mb_get_userdata(loadedPrograms[i], (void**)&ud);
      if (strcmp(ud->programID, id) == 0)
      { dbg("request for");
        dbg(id);
        dbg("Got interpreter for");
        dbg(ud->programID);
        return &loadedPrograms[i];
      }
    }
  }
  return 0;
}


///This must be called under lock. We can't manage that because we don't
///Know what you plan to do with it.
mb_interpreter_t * _MyBasic::getInterpreter(const char * id)
{
  return *_programForId(id);
}
//Is an interpreter busy, regardless of if the parents are busy
static char _mbisdirectlybusy(mb_interpreter_t * i)
{
  struct loadedProgram * ud;
  mb_get_userdata(i, (void**)&ud);
  if(ud->busy)
  {
    return 1;
  }

  return 0; 
}

//Return true if the interpreter, any parent, or any child is busy.
//We don't explicitly check children(we don't even have refs to them)
//but the children increment all ancestor busy counts when they are busy.
static char _mbisbusy(mb_interpreter_t * i)
{
  struct loadedProgram * ud;
  while(i)
  {
    mb_get_userdata(i, (void**)&ud);
    if(ud->busy)
    {
      return 1;
    }
    i=mb_get_parent(i);
  }
  return 0; 
}



//Mark a program as busy by incrementing the reference count
static void _mbsetbusy(mb_interpreter_t * i)
{
  struct loadedProgram * ud;

  while(i)
  {
    mb_get_userdata(i, (void**)&ud);
    ud->busy +=1;
    i=mb_get_parent(i);
  }
  
}

static void _mbsetfree(mb_interpreter_t * i)
{
  struct loadedProgram * ud;

  while(i)
  {
    mb_get_userdata(i, (void**)&ud);
    ud->busy -=1;
    i=mb_get_parent(i);
  } 
}


//Represents a request to do something with
struct BasRequest
{
  //Pointer tp the target of the request
  mb_interpreter_t * interpreter;
  //Object that represents what the interpreter should do.
  //If it === interpreter, it means run loaded code
  mb_value_t * object;
  void * arg;
};


static void makeRequest(mb_interpreter_t * interpreter, mb_value_t * object)
{
  struct BasRequest br;
  br.interpreter = interpreter;
  br.object = object;
  xQueueSend(request_queue, &br, portMAX_DELAY);
}


//Close a program by ID
static void _closeProgram(const char * id)
{
  mb_interpreter_t ** old = _programForId(id);


  struct loadedProgram * ud;
  if (*old == 0)
  {
    return;
  }

  //This stops the program no matter what it's doing;
  mb_schedule_suspend(*old, MB_FUNC_END);

  ///Something can be "busy" without holding the lock if it yields.
 while(_mbisdirectlybusy(*old))
  {
    MB_UNLOCK;
    delay(2500);
    MB_LOCK;
  }

  mb_get_userdata(*old, (void **)&ud);

  free(ud);
  mb_close(old);
}




static void BasicInterpreterTask(void *)
{
  struct BasRequest br;
  struct loadedProgram * ud;

  while (1)
  {
    xQueueReceive(request_queue, &br, portMAX_DELAY);
    MB_LOCK;
    dbg("Got task and lock,");
    //The only supported request right now is to run the loaded app,
    //marked by the object being the interpreter.
    if(!(br.interpreter==(mb_interpreter_t*)br.object))
    {
      dbg("unsupported request");
      return;
    }


    //We yield for 2500ms while that particular program is busy.
    //Response time isn't an issue, programs should try to avoid
    //Requesting busy apps anyway.
    while(_mbisdirectlybusy(br.interpreter))
    {
      dbg("interpreter is busy, waiting");
      MB_UNLOCK;
      vTaskDelay(10);
      MB_LOCK;
    }

    _mbsetbusy(br.interpreter);
    dbg("running");
     mb_run(br.interpreter, true);
     dbg("exited");
    _mbsetfree(br.interpreter);
    MB_UNLOCK;
  }
}

///Implement delay() without jamming up all the other programs
int bas_delay_rtos(struct mb_interpreter_t* s, void** l) {
  int result = MB_FUNC_OK;
  int64_t n = 0;
  int r = 0;
  mb_assert(s && l);
  mb_check(mb_attempt_open_bracket(s, l));
  mb_check(mb_pop_int(s, l, &n));
  mb_check(mb_attempt_close_bracket(s, l));
  
  //Don't hold the GIL while delaying
  MB_UNLOCK;
  vTaskDelay(n/portTICK_PERIOD_MS);
  MB_LOCK;
  return result;
}


void _MyBasic::begin(char numThreads)
{

  for (char i = 0; i < 16; i++)
  {
    loadedPrograms[i] == 0;
  }
  mb_init();


  //Start the root interpreter
  const char * code =  "'The only line in this root program currently is this comment";
  mb_open(&bas_parent);
  enableArduinoBindings(bas_parent);
  mb_remove_func(bas_parent,"DELAY");
  mb_register_func(bas_parent, "DELAY", bas_delay_rtos);
  mb_load_string(bas_parent,code, true);

  struct loadedProgram * loadedprg = (struct loadedProgram *)malloc(sizeof(struct loadedProgram));
  memcpy(loadedprg->hash, code, 30);
  loadedprg->busy = 0;
  mb_set_userdata(bas_parent, loadedprg);
  rootInterpreter = bas_parent;

  bas_gil = xSemaphoreCreateBinary( );
  xSemaphoreGive(bas_gil);
  request_queue = xQueueCreate( 25, sizeof(struct BasRequest));

  for(char i =0; i<numThreads; i++)
  {
    xTaskCreatePinnedToCore(BasicInterpreterTask,
                "MyBasic",
                stackSize,
                0,
                1,
                &basTasks[i],
                1
              );
  }
}



void _MyBasic::lock()
{
  MB_LOCK;
}

void _MyBasic::unlock()
{
  MB_UNLOCK;
}
//Allow someone else to use the my-basic GIL, and don't return until we have the lock again.
static void basyield(mb_interpreter_t *)
{
  MB_UNLOCK;
  vTaskDelay(2);
  MB_LOCK;
}


//Polls the interpreter until it's free. waiting sleepfor in between.
//Should force_close be true, it will wait a maximum of retries before
//Closing everything.
void mb_wait_directly_free(mb_interpreter_t *s,int sleepfor, char force_close, int retries)
{
  
      ///Something can be "busy" without holding the lock if it yields.
     while(_mbisdirectlybusy(s))
      {
        MB_UNLOCK;

        delay(sleepfor);
        MB_LOCK;

        if (force_close)
        {
          retries -=1;
          if (retries ==-1)
          {
            dbg("Taking too long, stopping");
            mb_schedule_suspend(s, MB_FUNC_END);
            retries = 5;
          }
        }
    }
}





//Load a new program with the given ID, replacing any with the same ID if the
//first 30 bytes are different.
int _loadProgram(const char * code, const char * id)
{
  mb_interpreter_t ** old = _programForId(id);
  struct loadedProgram * ud = 0;
  //Check if programs are the same

  if (old)
  {
    mb_get_userdata(*old, (void **)&ud);
    //Check if the versions are the same
    if (memcmp(ud->hash, code, 30) == 0)
    {
      return 0;
    }

    
    ///Something can be "busy" without holding the lock if it yields.
     while(_mbisdirectlybusy(*old))
      {
        MB_UNLOCK;
        delay(2500);
        MB_LOCK;
      }

    _closeProgram(id);
  }

     //passing a null pointer tells it to use the input buffer
    if(code == 0)
    {
      if(ud)
      {
        if(ud->inputBuffer)
        {
         code = ud->inputBuffer;
        }
        else
        {
          code = "";
        }
      }
      else
      {
          code="";
      }
    }



  //This is a request to open the root interpreter
  if (id == 0)
  {
    mb_open(&bas_parent);
    enableArduinoBindings(bas_parent);
    mb_load_string(bas_parent, code, true);

    struct loadedProgram * loadedprg = (struct loadedProgram *)malloc(sizeof(struct loadedProgram));
    memcpy(loadedprg->hash, code, 30);
    loadedprg->busy = 0;
    mb_set_userdata(bas_parent, loadedprg);
    return 0;
  }

  //Find a free interpreter slot
  for (char i = 0; i < 16; i++)
  {
    if (loadedPrograms[i] == 0)
    {
      mb_interpreter_t * n = 0;
      mb_open_child(&n, &bas_parent);
      mb_load_string(n, code, true);
      mb_set_yield(n, basyield);

      struct loadedProgram * loadedprg = (struct loadedProgram *) malloc(sizeof(struct loadedProgram));
      memcpy(loadedprg->hash, code, 30);
      strcpy(loadedprg->programID, id);
      loadedprg->programID[strlen(id)] = 0;
      loadedprg->busy = 0;

      mb_set_userdata(n, loadedprg);
      loadedPrograms[i] = n;
      return 0;
    }
  }

  //err, could not find free slot for program
  return 1;
}



//If a program with that ID exists, replace the code
int _MyBasic::appendInput(const char * data, int len,const char * id)
{
  MB_LOCK;
  dbg("got lock");
  mb_interpreter_t ** old = _programForId(id);
  struct loadedProgram * ud = 0;

    //If we are trying to append to a program that doesn't exist, we need to first create it.
    //The main reason for the input buffers is so we can transfer code by ID.

    //But it's also just a handy STDIN like feature.
    if (old==0)
    {
      //Load an empty program with that ID
      _loadProgram("", id);
    }


    mb_get_userdata(*old, (void **)&ud);
    //Check if the versions are the same
    if(ud->inputBuffer)
    {
      ud->inputBuffer=(char *)realloc(ud->inputBuffer, ud->inputBufferLen+len+1);
    }
    else
    {
      ud->inputBuffer = (char *)malloc(len+1);
    }
    memcpy(ud->inputBuffer, data, len);
    ud->inputBufferLen += len;
  MB_UNLOCK;
}



//If a program with that ID exists, replace the code
int _MyBasic::updateProgram(const char * code, const char * id)
{
  MB_LOCK;
  dbg("got lock");
  mb_interpreter_t ** old = _programForId(id);
  struct loadedProgram * ud = 0;
  //Check if programs are the same

  if (old)
  {
    mb_get_userdata(*old, (void **)&ud);
    //Check if the versions are the same
    if (memcmp(ud->hash, code, 30) == 0)
    {
      dbg("Doing nothing, hash same");
      MB_UNLOCK;
      return 0;
    }

    //passing a null pointer tells it to use the input buffer
    if(code == 0)
    {
      if(ud)
      {
        if(ud->inputBuffer)
        {
         code = ud->inputBuffer;
        }
        else
        {
          code = "";
        }
      }
      else
      {
          code="";
      }
    }


   dbg("wait free");
    //Wait up to ten seconds, then tell the interpreter to stop whatever its doing.
    mb_wait_directly_free(*old, 100, 1, 20);

    mb_reset_preserve(old, 0);
    mb_load_string(*old, code, true);
  }
  else
  {
    _loadProgram(code, id);
  }
  MB_UNLOCK;
}

//Allow someone else to use the my-basic GIL, and don't return until we have the lock again.
void _MyBasic::yield()
{
  MB_UNLOCK;
  MB_LOCK;
}







int _MyBasic::loadProgram(const char * code, const char * id)
{

  MB_LOCK;
  int ret = _loadProgram(code, id);
  MB_UNLOCK;
  return ret;
}




void _MyBasic::runLoaded(const char * id)
{
   makeRequest(*_programForId(id),(mb_value_t *)*_programForId(id));
}


void mbRunInProgram(char * source, const char * id)
{
  MB_LOCK;
  struct mb_interpreter_t** program = _programForId(id);
  mb_load_string(*program, source, true);
  mb_run(*program, true);
  mb_reset_preserve(program, 0);
  MB_UNLOCK;
}
void mb_subshell(char * source, char * id)
{
  MB_LOCK;

  struct mb_interpreter_t** parent = _programForId(id);
  struct mb_interpreter_t* child = NULL;

  mb_open_child(&child, parent);


  //mb_set_printer(bas, xprintf);
  mb_load_string(child, source, true);
  mb_run(child, true);


  mb_close(&child);
  MB_UNLOCK;
}


