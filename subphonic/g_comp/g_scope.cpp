#include "g_scope.h" 

GScope::GScope(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    sig = new ValueScope(48000/10);
    sig->addInput(&empty);
   

    Pixmap* pmap = new Pixmap(pix["scope_back"]);
    add(pmap,0,0);
    add(new MoveBorder(this,pmap->pos.w,pmap->pos.h),0,0);
   
/*
  SDL_Color color;
  color.r=47; 
  color.g=243;
  color.b=255;
*/

    view = new DataPlotRing<smp_t>(pix["scope_back"]->w-7,pix["scope_back"]->h-10);
    view->setData(sig->getBuffer(), SMP_MAX);
    view->setReversePlot(true); 
     
    ProgramState& ps = ProgramState::instance();
    view->setColor(ps.getGraphColor());
    //view->setColor(color);
    add(view,4,5);
   
    it = new InputTaker(pix["in"], this,0);
    addInputTaker(it);
    add(it,5,5);
   
    og = new OutputGiver(pix["out"],this,0);
    addOutputGiver(og);
    add(og,19,5);
   
    KnobSlider* slide = new KnobSlider(pix["knob16x16"]);
    slide->setMouseMoveAction(new C_Amp(this));
    add(slide,34,5);
    slide->setValue(1.0/BOOST);
      
   
}


Value** GScope::getOutput(unsigned int id)
{
    return (Value**)(&sig);
}

void GScope::addInput(Value** out, unsigned int fromid)
{
    sig->addInput(out);
}

void GScope::remInput(Value** out, unsigned int fromid)
{
    sig->addInput(&empty);
    sig->clear();
}

GScope::C_Amp::C_Amp(GScope* src)
{
    this->src=src;
}

void GScope::C_Amp::action(Component* co)
{
    KnobSlider* s = (KnobSlider*)co;
    src->sig->setBoost(s->getValue()*BOOST);
}
