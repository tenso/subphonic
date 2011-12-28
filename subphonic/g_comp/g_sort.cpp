#include "g_sort.h"

GSort::GSort(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    sort = new Sort(NUM);
   
    Pixmap* back_pix = new Pixmap(pix["448x174"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("stable sort");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    OutputGiver* og;
    InputTaker* in;
   
    uint xoff=10;
    uint yoff=40;
    uint dy=30;
    int outdy=20;
   
    l = new Label(f);
    l->printF("sort\nweight");
    add(l, xoff, yoff);
   
    l = new Label(f);
    l->printF("override\nval");
    add(l, xoff, yoff+dy);
   
    l = new Label(f);
    l->printF("sorted\nval <=");
    add(l, xoff, yoff+dy*2+outdy);
   
    l = new Label(f);
    l->printF("from\ninput: 0-%d",NUM-1);
    add(l, xoff, yoff+dy*3+outdy);
   
    xoff+=100;
   
    //out
    for(uint i=0;i<NUM;i++)
    {
        l = new Label(f);
        if(i<10)l->printF("0%d", i);
        else l->printF("%d", i);
        add(l, xoff-2, yoff-15);
	
        //weight in
        in = new InputTaker(pix["in"], this, i);
        addInputTaker(in);
        add(in, xoff, yoff);
	
        //value in
        in = new InputTaker(pix["in"], this, NUM+i);
        addInputTaker(in);
        add(in, xoff, yoff+dy);
	
        //value out
        og = new OutputGiver(pix["out"],this, i);
        addOutputGiver(og);
        add(og, xoff, yoff+dy*2+outdy);
	
        //id out
        og = new OutputGiver(pix["out"],this, NUM+i);
        addOutputGiver(og);
        add(og, xoff, yoff+dy*3+outdy);
	
        xoff+=20;
    }
   
    ground.addInput((Value**)&sort);
}

GSort::~GSort()
{
    ground.remInput((Value**)&sort);
    delete sort;
}


Value** GSort::getOutput(unsigned int id)
{
    if(id<NUM)return sort->getSortedOutput(id);
    return sort->getSortedId(id-NUM);
}

void GSort::addInput(Value** out, unsigned int fromid)
{
    if(fromid<NUM)sort->addInputWeight(fromid, out);
    else sort->addInputValue(fromid-NUM, out);
}

void GSort::remInput(Value** out, unsigned int fromid)
{
    if(fromid<NUM)sort->addInputWeight(fromid, NULL);
    else sort->addInputValue(fromid-NUM, NULL);
} 


