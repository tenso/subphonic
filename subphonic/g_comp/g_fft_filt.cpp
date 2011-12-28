#include "g_fft_filt.h"


GFFTFilt::GFFTFilt(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    tran = new Transform(FFT_LEN);
    bwindow = new Window<smp_t>(Window<smp_t>::BLACKMAN, FFT_LEN);
   
    sig = new St_fft(FFT_LEN, FFT_LEN/4, bwindow);
    sig->setProcess(tran);
   
    h_fft_len = FFT_LEN/2;
   
    //add eveything
    Pixmap* back_pix = new Pixmap(pix["740x300"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
    Label* l = new Label(fnt["label"], string("fft eq"));
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
   
    //input
    in = new InputTaker(pix["in"], this,0);
    addInputTaker(in);
    add(in,5, 3);
   
    //output
    og = new OutputGiver(pix["out"],this, 0);
    addOutputGiver(og);
    add(og, 19, 3);
   
   
    //make sliders
   
    /*bins_per_slider = h_fft_len/NUM_SLIDERS;
      hz_per_slider = tran->getHzPerBin()*bins_per_slider;*/
   
   
    /*hz_per_slider = 100;
      bins_per_slider = hz_per_slider/tran->getHzPerBin();*/
   
   
   
    int xoff=50;
    int yoff=80;
    int xinc= pix["slide_underv_128_6"]->w+10;
    int slider_h = pix["slide_underv_128_6"]->h;
   
    bool over=true;
   
    uint start_bin=0;
    double num_bins=1;
   
    //number of sliders created: as many as are needed to fill upp all bins in 'tran'
    //or max, whatever comes first
    uint max=MAX_SLIDERS;
    Slider* nslider;
   
    for(uint i=0;i<max;i++)
    {
        if(i==max-1)num_bins = h_fft_len-1-start_bin; //put all remainging bins in last slider
        else
        {
            num_bins += 0.05*powf(1.2,i);
        }
	
        if(num_bins+start_bin >= h_fft_len)
        {
            if(start_bin>=h_fft_len)break;
	     
            num_bins=h_fft_len-1-start_bin;
            if(num_bins==0)break;
        }
	
        DASSERT(start_bin+num_bins < h_fft_len);
        DASSERT(num_bins>0);
	
        double slider_hz = (start_bin+num_bins)*tran->getHzPerBin();
	
        nslider = new Slider(pix["slide_knobv_14_12"],pix["slide_underv_128_6"], true);
        nslider->centerKnob(true);
        nslider->setMouseMoveAction(new C_SliderUpdate(this, i, start_bin, (uint)num_bins)); 
        nslider->setLimits(0,1);
        nslider->setValue(0);
	
        add(nslider, xoff, yoff);
	
	
        sliders.push_back(nslider);
        sliders[i]->mouseMoveAction();
	
        start_bin+=(uint)(num_bins)+1;
	
        //HZ labels
        if(i%2==0)
        {
            l = new Label(f);
	     
            //if(!over)l->setColor(128,0,0);
	     
            ostringstream ss;
	     
	     
            if(over)
            {
                ss << " |\n";
            }
            else
            {
                ss << "  \n";
		  
            }
	     
            if(slider_hz<1)ss << " ";
	     
            if(slider_hz>=1000)
            {
                ss.precision(1);
                ss << fixed << slider_hz/1000.0 << "k";
            }
            else 
            {
                ss.precision(0);
                ss << fixed << slider_hz;
            }
	     
            /*int xp=0;
              if(slider_hz>=10000)xp-=8;*/
	     
            l->printF(ss.str().c_str());
	     
            if(over)
                add(l, xoff-8, yoff+slider_h+1);
            else
            {
		  
                add(l, xoff-8, yoff+slider_h+1+10);
		  
                l = new Label(f);
                //if(!over)l->setColor(128,0,0);
		  
                l->printF(" |");
                add(l, xoff-8, yoff+slider_h+1);
            }
            over=!over;
        }
        else
        {
            l = new Label(f);
	     
            //if(!over)l->setColor(128,0,0);
	     
            ostringstream ss;
	     
            if(slider_hz>=1000)
            {
                ss.precision(1);
                ss << fixed << slider_hz/1000.0 << "k";
            }
            else 
            {
                ss.precision(0);
                ss << fixed << slider_hz;
            }
	     
	     

	     
            if(!over)
            {
                ss << "\n |";
		  
                l->printF(ss.str().c_str());
		  
                add(l, xoff-8, yoff-1-8*2);
            }
            else
            {
                l->printF(ss.str().c_str());
		  
                add(l, xoff-8, yoff+-1-10-8*2);
		  
                l = new Label(f);
                //if(!over)l->setColor(128,0,0);
                l->printF(" |");
                add(l, xoff-8, yoff-1-8);
            }
	     

	     

	     
        }
	
	
        xoff+=xinc;
	
    }
   
    /*BUTTONS*/
   
    b_zero = new Button(pix["up_ns"],pix["down_ns"]);
    b_zero->stayPressed(false);
    b_zero->setAction(new C_Change(this, C_Change::ZERO));
   
    xoff=10;
    yoff+=190;
   
    l = new Label(f, "zero");
    add(l, xoff, yoff);
   
    add(b_zero,xoff+45, yoff);
   
   
    b_one = new Button(pix["up_ns"],pix["down_ns"]);
    b_one->stayPressed(false);
    b_one->setAction(new C_Change(this, C_Change::ONE));
   
    xoff+=80;
   
    l = new Label(f, "one");
    add(l, xoff, yoff);
   
    add(b_one,xoff+35, yoff);
   
   
    b_half = new Button(pix["up_ns"],pix["down_ns"]);
    b_half->stayPressed(false);
    b_half->setAction(new C_Change(this, C_Change::HALF));
   
    xoff+=80;
   
    l = new Label(f, "half");
    add(l, xoff, yoff);
   
    add(b_half,xoff+35, yoff);
   
    xoff=280;
   
    l = new Label(f);
    l->printF( "first channel: 0 to %.0fhz (i.e mark is chan max)", tran->getHzPerBin());
    add(l, xoff, yoff);
}

GFFTFilt::~GFFTFilt()
{
    delete sig;
    delete tran;
}

Value** GFFTFilt::getOutput(unsigned int id)
{
    return (Value**)&sig;
}

void GFFTFilt::addInput(Value** out, unsigned int fromid)
{
    sig->setInput(out);
}

void GFFTFilt::remInput(Value** out, unsigned int fromid)
{
    sig->setInput(&empty);
}

/**********/
/*CALLBACK*/
/**********/

GFFTFilt::C_SliderUpdate::C_SliderUpdate(GFFTFilt* src, uint id, uint start_bin, uint num_bins)
{
    this->src=src;
    this->id=id;
   
    this->start_bin=start_bin;
    this->num_bins=num_bins;
}

void GFFTFilt::C_SliderUpdate::action(Component* c)
{
    /*uint start = id*src->bins_per_slider;
      uint stop = (id+1)*src->bins_per_slider;*/
   
    for(uint i=0;i<num_bins;i++)
    {
        DASSERT(start_bin+i<src->tran->getNumBins());
        src->tran->setCoeffSym(start_bin+i, src->sliders[id]->getValue());
    }
}



GFFTFilt::C_Change::C_Change(GFFTFilt* src, MODE mode)
{
    this->src=src;
    this->mode=mode;
}

void GFFTFilt::C_Change::action(Component* c)
{
    for(uint i=0;i<src->sliders.size();i++)
    {
        switch(mode)
        {
            case ZERO:
                src->sliders[i]->setValue(0);
                break;
	     
            case HALF:
                src->sliders[i]->setValue(0.5);
                break;
	     
            case ONE:
                src->sliders[i]->setValue(1.0);
                break;
        }
	
        src->sliders[i]->mouseMoveAction();
    }
}


/***********/
/*TRANSFORM*/
/***********/


GFFTFilt::Transform::Transform(uint len)
{
    this->len=len;
    data = new smp_t[len];
   
    for(uint i=0;i<len;i++)
    {
        data[i]=0;
    }
}

GFFTFilt::Transform::~Transform()
{
    delete[] data;
}

void GFFTFilt::Transform::setCoeffSym(uint bin_i, double c)
{
    uint sym = len-bin_i-1;
    DASSERT(bin_i < len/2);
    DASSERT( sym < len);
   
    data[bin_i]=c;
    data[sym]=c;
}

void GFFTFilt::Transform::setCoeff(uint bin_i, double c)
{
    DASSERT(bin_i < len);
   
    data[bin_i]=c;
}


double GFFTFilt::Transform::getHzPerBin() const
{
    return getBinHz(1);
}

double GFFTFilt::Transform::getBinHz(uint bin_i) const
{
    DASSERT(bin_i<len);
   
    double bin_freq=0;
    if(bin_i<len)
    {
        bin_freq = progstate.getSampleRate()*bin_i/(double)len;
    }
   
    return bin_freq;
}


uint GFFTFilt::Transform::getBinIndex(double freq) const
{
    uint bin_i = (uint)(freq*len/(double)progstate.getSampleRate());
   
    if(bin_i>=len)bin_i=len-1;
   
    return bin_i;
}

uint GFFTFilt::Transform::getNumBins() const
{
    return len;
}


bool GFFTFilt::Transform::operator()(spl::Complex* fft_out, uint n, bool shifted)
{
    DASSERT(n>=len);
   
   
    for(uint i=0;i<n;i++)
    {
        //bin_freq = 44100*i/(double)n;
	
        fft_out[i] *= data[i];
    }
   
    return true; 
}






/*state*/

void GFFTFilt::getState(SoundCompEvent* e)
{
    State s(sliders);
   
    e->copyData((char*)&s, sizeof(State));
}

void GFFTFilt::setState(SoundCompEvent* e)
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
   
    uint min = minOf<uint>(s->num, sliders.size());
   
    for(uint i=0;i<min;i++)
    {
        sliders[i]->setValue(s->coeff[i]);
        sliders[i]->mouseMoveAction();
    }
}
