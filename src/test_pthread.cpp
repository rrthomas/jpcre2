/**@file test_pthread.cpp
 * A multi threaded example with POSIX pthread
 * @include test_pthread.cpp
 * @author [Md Jahidul Hamid](https://github.com/neurobin)
 * */

#include <iostream>
#include <pthread.h>
#include "jpcre2.hpp"

typedef jpcre2::select<char> jp;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;

pthread_attr_t thattr;

//this is an example how you can use pre-defined data objects in multithreaded program.
//The logic is to wrap your objects inside another class and initialize them with constructor.
//Thus when you create objects of this wrapper class, they will have identical member objects.
//see the task() function that is using this class as a wrapper class to save 5 Regex objects.
//This has the overhead of creating new objects instead of using the same ones.
struct MyRegex{
    jp::Regex re[5];
    MyRegex(){
        re[0].compile("\\w","i");
        re[1].compile("\\d","i");
        re[2].compile("\\d\\w+","i");
        re[3].compile("\\d\\w\\s","m");
        re[4].compile("[\\w\\s]+","m");
    }
};

void *task(void *arg){
    MyRegex re;
    std::string sub[5] = {"subject1", "123456789", "1a2b3c", "1a 2b 3c ", "I am a string"};
    for(int i = 0;i<5; ++i){
        re.re[i].match(sub[i], "g");
    }
    return 0;
}

void *thread_safe_fun1(void *arg){ //uses no global or static variable, thus thread safe.
	jp::Regex re("\\w", "i"); 
	re.getMatchObject().setSubject("fdsf").setModifier("g").match();
    return 0;
}

void* thread_safe_fun2(void* arg){//uses no global or static variable, thus thread safe.
    jp::Regex re("\\w", "g");
    jp::RegexMatch rm(&re);
    
    rm.setSubject("fdsf").setModifier("g").match();
    return 0;
}

jp::Regex rec("\\w", "g");

void *thread_unsafe_fun1(void *arg){ //uses global variable 'rec', thus thread unsafe.
    //this mutex lock will not make it thread safe
    pthread_mutex_lock( &mutex2 );
    jp::RegexMatch rm(&rec);
    rm.setSubject("fdsf").setModifier("g").match();
    pthread_mutex_unlock( &mutex2);
    return 0;
}


int main(){
    pthread_t thread[20];
    pthread_attr_init(&thattr);
    pthread_attr_setdetachstate(&thattr, PTHREAD_CREATE_DETACHED);
    
    
    for(size_t i = 0;i<5;++i){
        if(pthread_create( &thread[i], &thattr, task, 0));
        //~ //else pthread_detach(thread[i]);
    }
    
    for(size_t i=5;i<10;++i){
        if(pthread_create( &thread[i], &thattr, thread_safe_fun1, 0));
        //~ else pthread_detach(thread[i]);
    }
    
    for(size_t i=10;i<15;++i){
        if(pthread_create( &thread[i], &thattr, &thread_safe_fun2, 0));
        //~ else pthread_detach(thread[i]);
    }
    pthread_attr_destroy(&thattr);
    pthread_exit((void*) 0);
    return 0;

}
