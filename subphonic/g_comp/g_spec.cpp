#include "g_spec.h"
#define LEN 1024*2

//FIXMENOW: needs alot of things:
//labels, zoom etc etc

GSpec::GSpec(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    viz=NULL;
   
      
    bwindow = new Window<smp_t>(Window<smp_t>::HANN, LEN); 
    sig = new St_fft(LEN, LEN/4/*hopsize*/, bwindow);
    sig->setDebug(true);
   
    sig->setInput(&empty);
   
   
    //add eveything

    Pixmap* back_pix = new Pixmap(pix["620x268"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
    Label* l = new Label(fnt["label"], string("frequency spectrum"));
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    int xoff=15;
    int yoff=25;
   
    add(new Pixmap(pix["vback96x96"]), xoff, yoff);
    add(new Pixmap(pix["vback96x96"]), xoff+96, yoff+96);
    add(new Pixmap(pix["vback96x96"]), xoff, yoff+96);
    add(new Pixmap(pix["vback96x96"]), xoff+96, yoff);
    add(new Pixmap(pix["vback96x96"]), xoff+96*2, yoff);
    add(new Pixmap(pix["vback96x96"]), xoff+96*2, yoff+96);
   
    xoff=15+300;
    yoff=25;
   
    add(new Pixmap(pix["vback96x96"]), xoff, yoff);
    add(new Pixmap(pix["vback96x96"]), xoff+96, yoff+96);
    add(new Pixmap(pix["vback96x96"]), xoff, yoff+96);
    add(new Pixmap(pix["vback96x96"]), xoff+96, yoff);
    add(new Pixmap(pix["vback96x96"]), xoff+96*2, yoff);
    add(new Pixmap(pix["vback96x96"]), xoff+96*2, yoff+96);
   
   
    //input
    in = new InputTaker(pix["in"], this,0);
    addInputTaker(in);
    add(in,5, 3);
   
    //output
    og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, 19, 3);
   
   
    xoff=20;
    yoff=230;
    s_vquota = new Slider(pix["slide_knob"],pix["slide_under"]);
    s_vquota->setMouseMoveAction(new C_VQuota(this));
    add(s_vquota, xoff,yoff);
    s_vquota->setLimits(0, 0.5);
    s_vquota->setValue(0.5);
   
    l_vquota = new Label(fnt["label"]);
    l_vquota->printF("max freq: %dhz", (int)(progstate.getSampleRate()*s_vquota->getValue()));
    add(l_vquota, xoff+100,yoff);
   
    xoff=15;
    yoff=25;
    //viz
    viz = new Visualize(LEN);
    viz->setPlotQuota(s_vquota->getValue());
    //viz->setMode(Visualize::PHASE);
    sig->setProcess(viz);
   
    p_spec = new Histogram<smp_t>(288, 192);
    p_spec->setData(viz->getMagData(), viz->getPlotDataLen(), false);
    add(p_spec, xoff,yoff);
   
    p_plot = new DataPlot<smp_t>(288, 192);
    p_plot->setData(viz->getPhaseData(), viz->getPlotDataLen(), M_PI);
    add(p_plot, xoff+300,yoff);
}

GSpec::~GSpec()
{
    delete sig;
    delete viz;
}

Value** GSpec::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GSpec::addInput(Value** out, unsigned int fromid)
{
    sig->setInput(out);
}

void GSpec::remInput(Value** out, unsigned int fromid)
{
    sig->setInput(&empty);
}


/***********/
/*VISUALIZE*/
/***********/

GSpec::Visualize::Visualize(uint len)
{
    this->len=len;
    p_len=len/2;
   
    mag_data = new smp_t[len];
    phase_data = new smp_t[len];
   
    for(uint i=0;i<len;i++)
    {
        mag_data[i]=0;
        phase_data[i]=0;
    }
    //setMode(MAG);
}

GSpec::Visualize::~Visualize()
{
    delete[] mag_data;
    delete[] phase_data;
}


bool GSpec::Visualize::operator()(spl::Complex* fft_out, uint n, bool shifted)
{
    DASSERT(n>=len);

    smp_t max=100; //FIXME: problem: how to scale data for vis?
	
    for(uint i=0;i<p_len;i++)
    {
        mag_data[i] = 0.2*fft_out[i].length()/max + 0.8*mag_data[i]; //just a simple iir to control vis falloff
	
        //-pi,pi
        double ndata = fft_out[i].arg();
	
        phase_data[i] = 0.005*ndata + 0.995*phase_data[i]; //just a simple iir to control vis falloff
    }
   
    return true; 
}


const smp_t* GSpec::Visualize::getMagData()
{
    return mag_data;
}

const smp_t* GSpec::Visualize::getPhaseData()
{
    return phase_data;
}

uint GSpec::Visualize::getPlotDataLen()
{
    return p_len;
}

/*void GSpec::Visualize::setMode(MODE mode)
  {
  this->mode=mode;
  }
*/

void GSpec::Visualize::setPlotQuota(double p)
{
    if(p>1)
    {
        ERROR("p to big");
        p=1;
    }
    if(p<0)
    {
        ERROR("p to small");
        p=0;
    }
   
    p_len = (int)(len*p);
    DASSERT(p_len<=len);
}


GSpec::C_VQuota::C_VQuota(GSpec* src)
{
    this->src=src;
} 

void GSpec::C_VQuota::action(Component* c)
{
    src->viz->setPlotQuota(src->s_vquota->getValue());
    src->l_vquota->printF("max freq: %dhz", (int)(progstate.getSampleRate()*src->s_vquota->getValue()));
   
    src->p_spec->setData(src->viz->getMagData(), src->viz->getPlotDataLen(), false);
    src->p_plot->setData(src->viz->getPhaseData(), src->viz->getPlotDataLen(), M_PI);
}
