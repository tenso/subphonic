#include "g_common.h"

unsigned int SoundComp::st_seq=0;

/**********/
/*DISKCOMP*/
/**********/

DiskComponent::DiskComponent()
{
    empty=true;
}

DiskComponent::DiskComponent(SCType id, unsigned int s, SDL_Rect p)
{
    data.sc_type=id;
    data.seq=s;
    data.pos=p;
    data.appendix_len=0;
    empty=false;
    appendix=NULL;
}

DiskComponent::~DiskComponent()
{
    if(appendix!=NULL)
    {
        delete[] appendix;
    }
}

DiskComponent::DiskComponent(const DiskComponent& c)
{
    empty=c.empty; 
    data=c.data;
   
    appendix=NULL;
    if(c.data.appendix_len!=0)
        setAppendix(c.appendix,c.data.appendix_len); //might be saved on disk
}

DiskComponent& DiskComponent::operator=(const DiskComponent& c)
{
    if(&c==this)return *this;
   
    empty=c.empty;
    data=c.data;
   
    appendix=NULL;
    if(c.data.appendix_len!=0)
        setAppendix(c.appendix,c.data.appendix_len); //might be saved on disk
   
    return *this;
}


void DiskComponent::offsetSeq(int i)
{
    DASSERT(!empty);
   
    DASSERT(data.seq+i>=0);
    data.seq+=i;
}

bool DiskComponent::haveAppendix() const
{
    DASSERT(!empty);
    return data.appendix_len!=0;
}

void DiskComponent::setAppendix(const Uint8* raw, unsigned int len)
{
    if(appendix!=NULL)delete[] appendix;
   
    DASSERT(raw!=NULL);
    data.appendix_len=len;
    appendix = new Uint8[len];
    memcpy(appendix,raw,len);
}

void DiskComponent::setAppendix(const Uint8* raw)
{
    setAppendix(raw,data.appendix_len);
}

const Uint8* DiskComponent::getAppendixData() const
{
    return appendix;
}

DiskComponent::Data DiskComponent::getData() const
{
    return data;
}
void DiskComponent::setData(SCType id, unsigned int seq, SDL_Rect pos)
{
    data.sc_type=id;
    data.seq=seq;
    data.pos=pos;
}

void DiskComponent::setData(const Data& d)
{
    data=d;
}


SCType DiskComponent::getSCType()
{
    return data.sc_type;
}

unsigned int DiskComponent::getSeqNum()
{
    return data.seq;
}
SDL_Rect DiskComponent::getPos()
{
    return data.pos;
}
unsigned int DiskComponent::getAppendixSize()
{
    return data.appendix_len;
}



/*************/
/*GIVER/TAKER*/
/*************/

OutputGiver::OutputGiver(SDL_Surface* pixmap, SoundComp* sc, unsigned int id)
{
    DASSERT(pixmap!=NULL);
   
    src = sc;
    back = pixmap;
   
    //important
    pos.w=back->w;
    pos.h=back->h;
   
    this->id=id;
    line=NULL;
}

OutputGiver::~OutputGiver()
{
}


int OutputGiver::compID()
{
    return OUTPUT_GIVER;
}

void OutputGiver::draw(Screen* screen)
{
    screen->blit(back, &pos);
   
    //FIXME: this is a DIRTY hack
    if(line!=NULL)
    {
        SurfaceHoldAutoBuild& pix = SurfaceHoldAutoBuild::instance();
        screen->blit(pix["inout_used"], &pos);
    }
}

SoundComp* OutputGiver::getSrcSC()
{
    return src;
}

void OutputGiver::relMove(int x, int y)
{
    pos.x+=x;
    pos.y+=y;
    if(line!=NULL)
    {
        line->line[0].x+=x;
        line->line[0].y+=y;
    }
}

void OutputGiver::setLine(LineData* d)
{
    line=d;
}

void OutputGiver::remLine()
{
    line=NULL;
}

void OutputGiver::remOutput()
{
    src->remOutput(NULL, id);
    //remLine();
}

uint OutputGiver::getId()
{
    return id;
}

/************/
/*InputTaker*/
/************/


InputTaker::InputTaker(SDL_Surface* pixmap, SoundComp* src, unsigned int id)
{
    DASSERT(pixmap!=NULL);
   
    this->id=id;
    this->src=src;
    back = pixmap;
   
    //important
    pos.w=back->w;
    pos.h=back->h;
   
    line=NULL;
    og=NULL;
}

InputTaker::~InputTaker()
{
}


int InputTaker::compID()
{
    return INPUT_TAKER;
}

void InputTaker::draw(Screen* screen)
{
    screen->blit(back, &pos);
   
    //FIXME: this is a DIRTY hack
    if(line!=NULL)
    {
        SurfaceHoldAutoBuild& pix = SurfaceHoldAutoBuild::instance();	
        screen->blit(pix["inout_used"], &pos);
    }
}

void InputTaker::setInput(OutputGiver* og)
{
    this->og=og;
    src->addInput(og->getSrcSC()->getOutput(og->getId()), id);
}

void InputTaker::remInput()
{
    src->remInput(og->getSrcSC()->getOutput(og->getId()), id);
    //remLine();
}

void InputTaker::relMove(int x, int y)
{
    pos.x+=x;
    pos.y+=y;
    if(line!=NULL)
    {
        line->line[1].x+=x;
        line->line[1].y+=y;
    }
}

void InputTaker::setLine(LineData* d)
{
    line=d;
}

void InputTaker::remLine()
{
    line=NULL;
}

SoundComp* InputTaker::getSrcSC()
{
    return src;
}

uint InputTaker::getId()
{
    return id;
}

/**********/
/*LineData*/
/**********/

LineData::LineData(SDL_Rect s, SDL_Rect e, OutputGiver* out, InputTaker* in)
{
    line[0]=s;
    line[1]=e;
    this->out=out;
    this->in=in;
   
    tmp_settle=0;
    tmp_count=0;
   
    color_v=0;
    have_color=false;
    color[0]=0; color[1]=0xef; color[2]=0;
}

ConPair LineData::getConPair()
{
    ConPair cp;
   
    cp.seq_out=out->getSrcSC()->getSeqNum();
    cp.seq_out_id=out->getId();
    cp.seq_in=in->getSrcSC()->getSeqNum();
    cp.seq_in_id=in->getId();
   
    return cp;
}


/****************/
/*SoundCompEvent*/
/****************/

SoundCompEvent::SoundCompEvent()
{
    data=NULL;
    size=0;
   
    want_partial=false;
    is_partial=false;
    partial_no=0;
}
SoundCompEvent::~SoundCompEvent()
{
    deallocate();
}


SoundCompEvent::SoundCompEvent(const SoundCompEvent& from)
{
    //cout << "SoundCompEvent::SoundCompEvent(const SoundCompEvent& from)" << endl;
   
    copyData(from.getData(), from.getSize());
}

SoundCompEvent& SoundCompEvent::operator=(const SoundCompEvent& from)
{
    //cout << "SoundCompEvent& SoundCompEvent::operator=(const SoundCompEvent& from)" << endl;
   
    if(this==&from)return *this;
   
    copyData(from.getData(), from.getSize());
   
    return *this;
}


bool SoundCompEvent::empty() const
{
    return data==NULL;
}

void SoundCompEvent::copyData(const char* d, uint s)
{
    if(s==0)
    {
        deallocate();
        return;
    }
   
    allocate(s);
    memcpy(data, d ,size);
}
void SoundCompEvent::free()
{
    deallocate();
}
const char* SoundCompEvent::getData() const
{
    return data;
}
uint SoundCompEvent::getSize() const
{
    return size;
}

void SoundCompEvent::allocate(uint size)
{
    if(size==0)
    {
        deallocate();
        return;
    }
   
    if(size==this->size)return;
   
    if(data!=NULL)deallocate();
   
    this->size=size;
    data = new char[size];
}

void SoundCompEvent::deallocate()
{
    if(data==NULL)return;
   
    delete[] data;
    data=NULL;
    size=0;
}



void SoundCompEvent::setWantPartial(bool v)
{
    want_partial=v;
}

bool SoundCompEvent::getWantPartial() const
{
    return want_partial;
}

void SoundCompEvent::setIsPartial(bool v)
{
    is_partial=v;
}

bool SoundCompEvent::getIsPartial() const
{
    return is_partial;
}

int SoundCompEvent::getPartialNo() const
{
    return partial_no;
}

void SoundCompEvent::setPartialNo(int no)
{
    partial_no=no;
}



/***********/
/*SoundComp*/
/***********/

SurfaceHoldAutoBuild& SoundComp::pix = SurfaceHoldAutoBuild::instance();
const ProgramState& SoundComp::progstate = ProgramState::instance();
//SingleGround& SoundComp::ground = SingleGround::instance();

ResHold<BitmapFont>& SoundComp::fnt = ResHold<BitmapFont>::instance();

SoundComp::SoundComp(GroundMixer& g) : ground(g)
{
    setPos(0, 0);
   
    empty = new Const(0);
    seq=st_seq++;
}

SoundComp::~SoundComp()
{
    delete empty;
}

DiskComponent SoundComp::getDiskComponent()
{
    return DiskComponent(sc_type, seq, pos);
}

void SoundComp::setAppendix(const Uint8* data, unsigned int len)
{
   
}

int SoundComp::compID()
{
    return SOUND_COMP;
}

void SoundComp::setSCType(SCType id)
{
    sc_type=id;
}

SCType SoundComp::getSCType()
{
    return sc_type;
}


unsigned int SoundComp::getSeqNum()
{
    return seq;
}

void SoundComp::setSeqNum(unsigned int s)
{
    seq=s;
}

//output
Value** SoundComp::getOutput(unsigned int id)
{
    return NULL;
}

void SoundComp::remOutput(Value** o, unsigned int id)
{
}

//input
void SoundComp::addInput(Value** out, unsigned int id)
{
}

void SoundComp::remInput(Value** out, unsigned int id)
{
}



/*************/
/*TAKER/GIVER*/
/*************/

InputTaker* SoundComp::addInputTaker(InputTaker* in)
{
    pair<map<uint,InputTaker*>::iterator, bool> ret = intakers.insert(make_pair(in->getId(), in));
   
    if(!ret.second)
    {
        DERROR("intaker already present");
    }
   
    return ret.first->second;
}

void SoundComp::addInputTaker(InputTaker** in, int num)
{
    for(int i=0;i<num;i++)addInputTaker(in[i]);
}


bool SoundComp::remInputTaker(InputTaker* in) 
{
    map<uint,InputTaker*>::iterator it;
    it = intakers.find(in->getId());
   
    if(it==intakers.end())
    {
        DERROR("intaker not found");
        return false;
    }
   
    intakers.erase(it);
   
    return true;
}


InputTaker* SoundComp::getInputTaker(unsigned int id)
{
    map<uint,InputTaker*>::iterator it;
    it = intakers.find(id);
   
    if(it==intakers.end())
    {
        DERROR("intaker not found, wanted id:" << id);
        return NULL;
    }
   
    return it->second;
}

map<uint,InputTaker*>* SoundComp::getAllIntakers()
{
    return &intakers;
}

OutputGiver* SoundComp::addOutputGiver(OutputGiver* o)
{
    pair<map<uint,OutputGiver*>::iterator,bool> ret = outgivers.insert(make_pair(o->getId(), o));
   
    if(!ret.second)
    {
        DERROR("outgiver already present, ");
    }
   
    return ret.first->second;
}

void SoundComp::addOutputGiver(OutputGiver** in, int num)
{
    for(int i=0;i<num;i++)addOutputGiver(in[i]);
}

bool SoundComp::remOutputGiver(OutputGiver* o)
{
    map<uint,OutputGiver*>::iterator it;
    it = outgivers.find(o->getId());
   
    if(it==outgivers.end())
    {
        DERROR("outgiver not found");
        return false;
    }
   
    outgivers.erase(it);
   
    return true;
}

OutputGiver* SoundComp::getOutputGiver(unsigned int id)
{
    map<uint,OutputGiver*>::iterator it;
    it = outgivers.find(id);
   
    if(it==outgivers.end())
    {
        DERROR("outgiver not found");
        return NULL;
    }
   
    return it->second;
}

map<uint,OutputGiver*>* SoundComp::getAllOutgivers()
{
    return &outgivers;
}

int SoundComp::inputTakerId(const string& name, uint port)
{
    return -1;
}

int SoundComp::outputGiverId(const string& name, uint port)
{
    return -1;
}


/*******/
/*state*/
/*******/

void SoundComp::getState(SoundCompEvent* e)
{
}

void SoundComp::setState(SoundCompEvent* e)
{
}


/*int SoundComp::getInterpolatedState(SoundCompEvent* dst, SoundCompEvent* start, SoundCompEvent* end, double pos)
  {
  return -1;
  }*/

//some might want this to put all affected in some common state i.e g_punchpad etc..
int SoundComp::syncToIndex(unsigned int index)
{
    return 0;
}

