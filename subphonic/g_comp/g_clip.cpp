#include "g_clip.h"

GClip::GClip(BitmapFont* f, GroundMixer& g) : SoundComp(g) 
{
    //doppler = new ValueDoppler();
    //mix = new Mixer();
   
    clip_od = new ClipOverdrive();
    //input=NULL;
   
    //clip_od->setInput(doppler->getSlave());   
      
    depth_dry = new Gain(1.0);
   
    clip_od->setInput((Value**)&depth_dry);
   
    //depth_dry->setInput((Value**)&clip_od);
   
    /*depth_wet = new Gain(1.0);
      depth_wet->setInput((Value**)&clip_od);
    */
   
    Pixmap* back_pix = new Pixmap(pix["96x128"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
      
    Label* l = new Label(fnt["label"]);
    l->printF("clip");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //input
    InputTaker* in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
   
    //output
    OutputGiver* og = new OutputGiver(pix["out"], this, 0);
    addOutputGiver(og);
    add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
   
   
    int xoff=10;
    int yoff=90;
   
    /*in = new InputTaker(pix["in"], this, 1);
      addInputTaker(in);
      add(in, xoff, yoff);*/
   
    l = new Label(f);
    l->printF("in boost");
    add(l, xoff, yoff);
   
    yoff+=10;
   
    /*mix_slide = new Slider(pix["slide_knob"],pix["slide_under"]);
      mix_slide->setMouseMoveAction(new C_Update(this));
      mix_slide->setValue(0.5);
      add(mix_slide, xoff+30,yoff);*/
   
    gain_knob = new KnobSlider(pix["knob16x16"]);
    gain_knob->setMouseMoveAction(new C_Update(this));
    gain_knob->setContinous(0, 10, 5);
    gain_knob->setValue(1.0);

    add(gain_knob, xoff+30,yoff);
   
    gain_reset = new Button(pix["up"],pix["down"]);
    gain_reset->stayPressed(false);
    gain_reset->setAction(new C_ResetGain(this));
    add(gain_reset,xoff+50, yoff);
   
    /*l = new Label(f);
      l->printF("dry");
      add(l,xoff,yoff);
   
      l = new Label(f);
      l->printF("wet");
      add(l,xoff+70,yoff);
    */
   
    xoff=10;
    yoff=20;
        
    graph_len = 64;
    graph = new smp_t[graph_len];
    makeGraph(graph, graph_len);
   
    Pixmap* gpix = new Pixmap(pix["vback64x64"]);
    add(gpix, xoff, yoff);
   
    plot = new DataPlot<smp_t>(64, 64);
   
    plot->setData(graph, graph_len, SMP_MAX);
   
    ProgramState& ps = ProgramState::instance();
    plot->setColor(ps.getGraphColor());
   
    add(plot, xoff, yoff);
      
    update();
   
   
    //DEBUG
    /*
      double xdata[] = {-1.0, 1.0, 0};
      double ydata[] = {0, 1.0, -1};
   
      uint dl=sizeof(xdata)/sizeof(double);
      cout << dl << endl;
      double* xd = new double[dl];
      double* yd = new double[dl];
   
      memcpy(xd, xdata, dl*sizeof(double));
      memcpy(yd, ydata, dl*sizeof(double));
   
      Plot<double>* p = new Plot<double>(96,96);
      p->setData(xd, yd, dl);

   
      gpix = new Pixmap(pix["vback96x96"]);
      add(gpix, 10,96); 
      add(p, 10,96); */
}

GClip::~GClip()
{
      
    delete depth_dry;
    //delete depth_wet;
    //delete doppler;
   
    //delete mix;
    delete clip_od;
   
    delete[] graph;
}

void GClip::makeGraph(smp_t* graph, uint len)
{
    LinearSweep* ls = new LinearSweep(-1.0, 1.0, len);
    ls->reset();
   
    clip_od->setInput((Value**)&ls);
   
    clip_od->make(graph, len, 1.0);
   
    //reset input
    //clip_od->setInput(doppler->getSlave());
    //clip_od->setInput(input);
    clip_od->setInput((Value**)&depth_dry);
   
    delete ls;
}


Value** GClip::getOutput(unsigned int id)
{
    return (Value**)&clip_od;
}

void GClip::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)
    {
        depth_dry->setInput(out);
	
        //doppler->setInput(out);
        //clip_od->setInput(doppler->getSlave());
	
        //depth_dry->setInput((Value**)&clip_od);
        //depth_wet->setInput((Value**)&doppler);
	
        //mix->addInput((Value**)&depth_dry);
        //mix->addInput((Value**)&depth_wet);
    }
}

void GClip::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)
    {
        depth_dry->setInput(NULL);
        //input=NULL;
        //clip_od->setInput(NULL);
        //mix->remAllInput();
    }
}


void GClip::update()
{
    double sv = gain_knob->getValue();
   
    depth_dry->setGain(sv);
    //depth_wet->setGain(sv);
}


GClip::C_Update::C_Update(GClip* d)
{
    this->src=d;
}

void GClip::C_Update::action(Component* co)
{
    src->update();
}


GClip::C_ResetGain::C_ResetGain(GClip* d)
{
    this->src=d;
}

void GClip::C_ResetGain::action(Component* co)
{
    src->gain_knob->setValue(1.0);
    src->update();
}

/*state*/

void GClip::getState(SoundCompEvent* e)
{
    State s((double)gain_knob->getValue());
   
    e->copyData((char*)&s, sizeof(State));
}

void GClip::setState(SoundCompEvent* e)
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
   
    gain_knob->setValue((smp_t)s->mix);
    update();
   
}

