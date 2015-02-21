#ifndef INCLUDE_BCOMPONENTS
#define INCLUDE_BCOMPONENTS

#include "entity.hpp"
#include <nall/set.hpp>

namespace Sys
{
    struct Component
    {
        Component(entityid_t myEntity);
        Component();
        virtual ~Component();
        
        entityid_t entityID;
    };
    
    // define collection and interfaces for components of some arbitrary type
    template<typename CType>
    struct Collection
    {
        nall::set<CType*> List;
        
        typename nall::set<CType*>::iterator begin()
        {
            return List.begin();
        }
        typename nall::set<CType*>::iterator end()
        {
            return List.end();
        }
        void add(CType * new_instance)
        {
            List.insert(new_instance);
        }
        void remove(CType * dying_instance)
        {
            List.remove(dying_instance);
        }
        void killall()
        {
            while(List.size() > 0)
                delete *List.begin();
        }
    };
    
}

#endif
