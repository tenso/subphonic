template<class T>
ResHold<T> ResHold<T>::me;

template<class T>
ResHold<T>& ResHold<T>::instance()
{
    return me;
}

template<class T>
ResHold<T>::ResHold()
{
    empty=NULL;
}

template<class T>
ResHold<T>::~ResHold()
{
    clear();
}

template<class T>
T* ResHold<T>::get(const std::string name, bool strict)
{

    typename Data::iterator it = data.find(name);
   
    if(it==data.end())
    {
        if(strict)
        {
            //ERROR(cout << "resource not found:" + name);
            return NULL;
        }
        else return empty;
    }
   
    return it->second;
}

template<class T>
T* ResHold<T>::operator[](const std::string k)
{
    return get(k);
}


template<class T>
bool ResHold<T>::add(const std::string name, T* s)
{
    empty = s;
   
    typename Data::iterator it = data.find(name);
    if(it!=data.end())
    {
        DERROR("resource with same name already exist");
        return false;
    }
   
    data.insert(make_pair(name, s));
   
    return true;
}

template<class T>
void ResHold<T>::clear()
{
    list<T*> rem;
   
    //to prevent double free if same pointer added multiple times(diff names)
    typename Data::iterator it = data.begin();
    while(it!=data.end())
    {
        typename list<T*>::iterator r_it = find(rem.begin(), rem.end(), it->second);
        if(r_it==rem.end())
        {
            rem.push_back(it->second);
            delete it->second;
        }
        else
        {
            //do nothing, already deleted
        }
        it++;
    }
    data.clear();
}
