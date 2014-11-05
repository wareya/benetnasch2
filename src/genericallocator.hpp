#include <nall/set.hpp>

#include <iostream>

template <typename T>
struct GenericAllocator
{
    nall::set<entityid_t> Used; // IDs which are currently being used
    nall::set<entityid_t> Freed; // IDs which were used at some point, but no longer are
    entityid_t LowestUnused = 0; // The lowest ID which has never been used yet
    
    T New()
    {
        if (Freed.size() == 0) // No freed IDs, use lowest unused
        {
            auto id = LowestUnused;
            Used.insert(id); //logn
            
            // We need to figure out what the next lowest unused will be
            // There aren't any freed ones to use, so we have to do it hard
            if (LowestUnused >= *Used.end()) // lowest unused is outside of used range, free to increment LowestUnused
                LowestUnused++;
            else // This shouldn't ever happen; no freed IDs, lowest unused being inside Used's bounds -- implies earlier error setting LowestUnused
            {
                std::cout << "Error: Unknown cause with LowestUnused being invalid 1!";
                return -1;
            }
            return id;
        }
        else // use an ID from the freed list
        {
            auto id = *Freed.begin();
            Freed.remove(id); //logn
            Used.insert(id); //logn
            
            if(Freed.size() > 0) // We have an ID to use
            { // (Freed.size == 0 block won't be the next to run if we're here, so...)
                LowestUnused = *Freed.begin();
                return id;
            }
            else // We just exhausted the last previously freed ID
            {
                if (LowestUnused >= *Used.end()) // lowest unused is outside of used range, free to increment LowestUnused
                    LowestUnused++;
                else // This shouldn't ever happen; no freed IDs, lowest unused being inside Used's bounds -- implies earlier error setting LowestUnused
                {
                    std::cout << "Error: Unknown cause with LowestUnused being invalid 2!";
                    return -1;
                }
                return id;
            }
        }
    }
    // attempt to forcefully allocate a specific ID
    // returns whether successful
    bool ForceNew(T id)
    {
        if(Exists(id))
            return false;
        else
        {
            Used.insert(id); //logn
            if (Freed.size() == 0 and LowestUnused == id)
            {
                LowestUnused = *Used.end() + 1;
            }
            else if (Freed.find(id))
            {
                Freed.remove(id);
            }
            else
            {
                puts("Critical error synchronizing internal state in ForceNew!");
            }
            return true;
        }
    }
    // free a given ID -- returns false if id doesn't exist
    bool Free(T id)
    {
        if(Used.find(id))
            return false;
        Freed.insert(id);
        Used.remove(id);
        if(id < LowestUnused)
            LowestUnused = id;
        return true;
    }
    bool Exists(T id)
    {
        return Used.find(id);
    }
};
