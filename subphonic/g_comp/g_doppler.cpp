#include "g_doppler.h"

GDoppler::GDoppler(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    doppler = new ValueDoppler(NUM-1, &empty);
    doppler->setInput(&empty);
     
    Pixmap* back_pix = new Pixmap(pix["128x38"]);  
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("doppler");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    uint yoff=20;
   
    //inputs
    it = new InputTaker(pix["in"], this,0);
    addInputTaker(it);
    add(it,5, yoff);
   
    //output
    for(uint i=0;i<NUM;i++)
    {
        og[i] = new OutputGiver(pix["out"],this, i);
        addOutputGiver(og[i]);
        add(og[i], 30+16*i, yoff);
    }
      
    l = new Label(f);
    l->printF(">");
    add(l,20,yoff+2);
}


Value** GDoppler::getOutput(unsigned int id)
{
    if(id==0)return (Value**)&doppler;
   
    DASSERT(id-1 >= 0 && id-1 < NUM-1);
    return doppler->getSlave(id-1);
   

}

void GDoppler::addInput(Value** out, unsigned int fromid)
{
    //in=out;
    doppler->setInput(out);
   
}

void GDoppler::remInput(Value** out, unsigned int fromid)
{
    doppler->setInput(&empty);
}
