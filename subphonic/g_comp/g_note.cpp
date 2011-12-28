#include "g_note.h"

char GNote::key_str[12][3]= {"A ","A#","B ","C ","C#","D ","D#","E ","F ","F#","G ","G#"};

GNote::GNote(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    octave = 4;
    ison=0;
    lastkey=1;
   
    hz=noteMap(lastkey, (int)octave);
   
    sig0 = new SmpPoint(&hz);
   
    Pixmap* back_pix = new Pixmap(pix["96x96"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    og = new OutputGiver(pix["out"], this, 0);
    addOutputGiver(og);
   
    add(og, 19, 3);
   
    ns0 = new NumberSelect<smp_t>(f, 2,0, &octave);
    ns0->setAction(new Update(this));
    ns0->setValue(octave);
    add(ns0,70,64);
   
    l = new Label(f);
    l->printF(key_str[0]);
    add(l,6,30);
   
    Label* ll = new Label(fnt["label"]);
    ll->printF("note");
    add(ll,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    ll = new Label(f);
    ll->printF("oct");
    add(ll,40,72);
   
    //key buttons
    for(int i=0;i<12;i++)
    {
        key[i] = new Button(pix["button6x12u"],pix["button6x12d"]);
        key[i]->stayPressed(true);
        key[i]->setAction(new PMODE(this,i+1));
        add(key[i],10+i*6,45);
    }
    key[lastkey-1]->action();

}

Value** GNote::getOutput(unsigned int id)
{
    return (Value**)&sig0;
}

void GNote::setHz(int key)
{
    hz = noteHz((int)key, (int)octave);
    string k = noteStr((int)key, (int)octave);
    l->printF("%s",k.c_str());
}

void GNote::getState(SoundCompEvent* e)
{
    State s(lastkey, (int)ns0->getValue());
   
    e->copyData((char*)&s, sizeof(State));
}

void GNote::setState(SoundCompEvent* e)
{
    DASSERT(e!=NULL);
    if(e->empty())return;
    if(e->getSize()!=sizeof(State))
    {
        DERROR("size missmatch");
        if(e->getSize() < sizeof(State))return;
	
        DERROR("trying to read...");
    }
    const State* s = (const State*)e->getData();
   
    lastkey = s->key;
   
    ns0->setValue(s->oct);
    key[lastkey-1]->action();
   
}

GNote::PMODE::PMODE(GNote* s, int i)
{
    this->src = s;
    key=i;
}

void GNote::PMODE::action(Component* comp)
{
    //unpress all
    for(int i=0;i<12;i++)
    {
        src->key[i]->setPressed(false);
    }
   
    //-1 because buttons start at0 not 1 as key does
    src->key[key-1]->setPressed(true);
   

    src->lastkey=key;
    src->setHz(key);
}


GNote::Update::Update(GNote* s)
{
    src=s;
}

void GNote::Update::action(Component* comp)
{
    src->setHz(src->lastkey);
}
