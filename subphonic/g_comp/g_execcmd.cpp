#include "g_execcmd.h"

GExecCmd::GExecCmd(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    Pixmap* back_pix = new Pixmap(pix["128x48"]);
    add(back_pix);
    add(new MoveBorder(this,back_pix->pos.w,20),0,0);
   
    Label* l = new Label(fnt["label"]);
    l->printF("execcmd");
    add(l, GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
   
    //out
    /*OutputGiver* og = new OutputGiver(pix["out"],this, 0);
      addOutputGiver(og);
      add(og, GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);*/
      
    InputTaker* in;
  
    uint xoff=10;
    uint yoff=25;
   
    in = new InputTaker(pix["in"], this, 0);
    addInputTaker(in);
    add(in, xoff, yoff);
            
    cmd_l = new Label(f);
    cmd_l->printF("unused");
    add(cmd_l, xoff+20, yoff+2);
   
    sig = new GateExecCmd();
    ground.addInput((Value**)&sig);
   
    /*FIXME
      xoff=60;
      yoff=25;
      Button* rb = new Button(pix["up"],pix["down"]);
      rb->stayPressed(true);
      rb->setAction(new C_GATE(this));
      add(rb, xoff,yoff);*/
}

GExecCmd::~GExecCmd()
{
    ground.remInput((Value**)&sig);
    delete sig;
}

/*
  Value** GExecCmd::getOutput(unsigned int id)
  {
  return (Value**)&sig;
  }*/


void GExecCmd::addInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(out);
}

void GExecCmd::remInput(Value** out, unsigned int fromid)
{
    if(fromid==0)sig->setInput(NULL);
} 


/*
  void GExecCmd::getState(SoundCompEvent* e)
  {
  }

  void GExecCmd::setState(SoundCompEvent* e)
  {
  DASSERT(e!=NULL);
  }
*/

void GExecCmd::setCmdStr(const string& cmd)
{
    sig->setCmdStr(cmd);
    cmd_l->printF("%s", cmd.c_str());
}

void GExecCmd::setCmdQueue(std::queue<string>* cmd_q)
{
    sig->setCmdQueue(cmd_q);
}
