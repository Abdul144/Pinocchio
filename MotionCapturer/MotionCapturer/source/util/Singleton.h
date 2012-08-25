#pragma once

#include "defines.h"

#define USE_SINGLETON(CLASS)	friend class Singleton<CLASS>;


template <class CLASS>
class Singleton
{
protected:
    static CLASS *instance;
    
protected:
    Singleton()
    {
    }

    virtual ~Singleton()
    {
    }

    void free()
    {
    	gjDelete(instance);
    }

public:
    static CLASS& getInstance()
    {
        if (instance == null)
            instance = new CLASS;
        
        return *instance;
    }
    
    static void releaseInstance()
    {
        gjDelete(instance);
    }
    
    static bool isAllocated()
    {
    	if (instance)
    		return true;
    	else
    		return false;
    }
    
};

template <class CLASS>
CLASS *Singleton<CLASS>::instance = null;