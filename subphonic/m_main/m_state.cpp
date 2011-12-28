#include "m_state.h"

/*********/
/*SCState*/
/*********/

SCState::SCState(SoundComp* sc)
{
    event = new SoundCompEvent();
    parent=NULL;
   
    if(sc!=NULL)setup(sc);
}

SCState::~SCState()
{
    delete event;
}

SCState::SCState(const SCState& from)
{
    event = new SoundCompEvent();
    *event = *from.event;
   
    parent = from.parent;
}

SCState& SCState::operator=(const SCState& from)
{
    if(this==&from)return *this;
   
    *event = *from.event;
    parent = from.parent;
   
    return *this;
}




bool SCState::reset()
{
    if(parent==NULL)return false;
   
    event->free(); //not really needed
    parent=NULL;
   
    return true;
}

bool SCState::setup(SoundComp* sc)
{
    if(sc==NULL)return false;
    reset();
    parent= sc;
    parent->getState(event);
    return true;
}

SoundComp* SCState::applyState()
{
    if(parent==NULL)return NULL;
   
    parent->setState(event);
   
    return parent;
}

bool SCState::isSetup()
{
    return parent!=NULL;
}

SoundComp* SCState::getParent()
{
    DASSERT(parent!=NULL);
    return parent;
}

void SCState::setParent(SoundComp* s)
{
    parent=s;
}


SoundCompEvent* SCState::getEvent()
{
    return event;
}


//make new event with this as base going towards next...
/*int SCState::makeInterpolated(SoundCompEvent* dst, SoundCompEvent* next, double pos)
  {
  DERROR("interpolated should not be used now, does not WORK");
  return parent->getInterpolatedState(dst,event,next,pos);
  }*/


/************/
/*CompStates*/
/************/


CompStates::CompStates(SoundComp* parent)
{
    this->parent=parent;
}

CompStates::~CompStates()
{
    //frees all states:
    reset();
}

bool CompStates::saveIndex(int i)
{
    //if(parent->getStateSize()==0)return false;
   
    //if it exist overwrite
    if(indexInUse(i))
    {
        return getState(i)->setup(parent);
    }
   
    //otherwise make new
    states.insert(make_pair(i, new SCState(parent)));
    return true;
}

bool CompStates::applyIndex(int i)
{
    //currenti=i;
    if(!indexInUse(i))return false;

    SCState* state = getState(i);
    DASSERT(state!=NULL);
   
    SoundComp* par = state->applyState();
   
    return (par!=NULL);
   
}

bool CompStates::delIndex(int i)
{
    if(!indexInUse(i))return false;
   
    s_it it = states.find(i);
    DASSERT(it!=states.end());
   
    SCState* state = it->second;
   
    states.erase(it);
   
    delete state;
   
    return true;
}

bool CompStates::delAll()
{
    int done=0;

    for(s_it it = states.begin();it!=states.end();it++)
    {
        SCState* state = it->second;
	
        delete state;
	
        done++;
    }
   
    states.clear();
   
    return done;
}

int CompStates::lastUsedIndex(int beg)
{
    if(states.size()==0)return -1;
   
    //lower_bound is: > beg
    s_it it= states.upper_bound(beg);
   
    if(it==states.begin())
    {
        //there are no
        return -1;
    }
   
    it--;
    return it->first;
}

int CompStates::nextUsedIndex(int index, int stop)
{
    if(states.size()==0)return -1;
   
    //find first greater than index
    s_it it = states.upper_bound(index);
   
    //if found and less than 'stop' return; otherwise wrap around
    if(it!=states.end() && it->first<stop)return it->first;
   
    if(states.begin()->first==stop)return -1;
   
    return states.begin()->first;
}


/*void CompStates::setState(int index, int stop)
  {
  if(inUse(index))
  {
  applyIndex(index);
	
  //nexti = nextUsed(index+1,stop);
  }
  else
  {
  //DERROR("interpolated should not be used now, does not WORK as expected");
  //setInterpolated(currenti,nexti,index,stop);
  }
  }*/

/*
  void CompStates::setInterpolated(int first, int next, int current, int stop)
  {
  DERROR("interpolated should not be used now, does not WORK as expected");
   
  double pos;
  if(first<next)pos = (current-first)/(double)(next-first);
  else 
  {
  if(current>first)pos = (current-first)/(double)(next+stop-first);
  else pos = (stop-first+current)/(double)(first+stop-next);
  }
   
  SoundCompEvent* e = new SoundCompEvent();
  if(getState(first)->makeInterpolated(e,getState(next)->getEvent(), pos)==-1)
  {
  parent->deleteState(e);
  return;
  }
  parent->setState(e);
  parent->deleteState(e);
  }
*/


void CompStates::copyIndex(int from, int to)
{
    if(states.size()==0)return;
    if(from==to)return;
   
    SCState* fromstate=NULL;
    if(indexInUse(from))fromstate = getState(from);
   
    if(fromstate==NULL) //this means a clean state at 'to'
    {
        if(indexInUse(to))
        {
            bool ok = delIndex(to);
            DASSERT(ok);
        }
        else
        {
            //nothing
        }
    }
    else
    {
        //beacuse getState() creates new if it does not exist:
        SCState* ts = getState(to);
        DASSERT(ts!=NULL);
        *ts = *fromstate;
    }
}

int CompStates::pushStatesFromIndex(int index, int push, int min_i, int max_i)
{
    int done=0;
   
    if(push==0)return done;
   
    if(index<min_i || index>max_i)
    {
        DERROR("try to push from outside range");
        return 0;
    }
      
    if(push>0)
    {
        if( index+push > max_i)
        {
            push=max_i-index;
        }
	
        done=push;
        if(push==0)return done;
	
        s_map new_map;
        if(push==0)return 0;
	
        for(s_it it = states.begin();it!=states.end();it++)
        {
            int i = it->first+push;
            SCState* data = it->second;
	     
            if(it->first>=index)
            {
                if(i<=max_i)
                {
                    new_map.insert(make_pair(i, data));
                }
                else
                {
                    //got pushed out of bound
                    delete it->second;
                }
		  
            }
            else
            {
                //keep as is
                new_map.insert(make_pair(it->first, data));
            }
	     
        }

        states=new_map;
	
    }
    else if(push<0)
    {
        if(index+push<min_i)push=min_i-index;
        done=push;
        if(push==0)return done;
	
        s_map new_map;
	
        for(s_it it = states.begin();it!=states.end();it++)
        {
            int i = it->first+push;
            SCState* data = it->second;
	     
            if(it->first>=index)
            {
                new_map.insert(make_pair(i, data));
            }
            else
            {
                if(it->first>=index+push)
                {
                    //will be overpushed
                    delete data;
                }
                else
                {
                    //keep
                    new_map.insert(make_pair(it->first, data));
                }
            }
	     
        }

        states=new_map;
    }
    return done;
}


bool CompStates::indexInUse(int i)
{
    return states.find(i)!=states.end();
}

void CompStates::reset()
{
    for(s_it it = states.begin();it != states.end();it++)
    {
        delete it->second;
    }
   
    states.clear();
}

SoundComp* CompStates::getParent()
{
    DASSERT(parent!=NULL);
   
    return parent;
}

SCState* CompStates::getState(int index)
{
    s_it it = states.find(index);
    if(it!=states.end())
    {
        return it->second;
    }
    else
    {
        pair<s_it, bool> ret = states.insert(pair<int, SCState*>(index, new SCState(parent)));
        DASSERT(ret.second);
        return ret.first->second;
    }
   
   
    return NULL;
}

unsigned int CompStates::getNumUsed() const
{
    return states.size();
}

CompStates::s_map& CompStates::getStates()
{
    return states;
}



//private

void CompStates::setIndexTo(int i, SCState* state)
{
    DASSERT(state!=NULL);
   
    //if it exist overwrite
    if(indexInUse(i))
    {
        SCState* f = getState(i);
        DASSERT(f!=NULL);
	
        *f = *state;
	
        return;
    }
   
    //otherwise make new 
    SCState* copy = new SCState(*state);
    states.insert(make_pair(i, copy));
}
