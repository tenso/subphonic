#include "g_altzeroctrl.h" 
#include <time.h>

GAltZeroCtrl::GAltZeroCtrl(BitmapFont* f, GroundMixer& g) : SoundComp(g)
{
    serial = new Serial();
    if(!serial->open("/dev/ttyUSB0", /*B38400*/38400))
    {
        cout << "serial open failed" << endl;
        delete serial;
        serial = NULL;
    }
    else
    {
        cout << "serial open" << endl;
    }
    sig = new AltZeroCtrl();
    sig->setSerial(serial);
    

    Pixmap* back_pix = new Pixmap(pix["320x480"]);
    add(back_pix, 0,0);
    add(new MoveBorder(this,back_pix->pos.w, back_pix->pos.h), 0,0);
    Label* l = new Label(fnt["label"]);
    l->printF("alt0 ctrl");
    add(l,GCOMP_LABEL_XOFF, GCOMP_LABEL_YOFF);
      
    //signal in/out
    //add(addInputTaker(new InputTaker(pix["in"], this, 0)), GCOMP_STDIN_XOFF, GCOMP_STDIN_YOFF);
    //add(addOutputGiver(new OutputGiver(pix["out"], this, 0)), GCOMP_STDOUT_XOFF, GCOMP_STDOUT_YOFF);
   
    int xoff=10;
    int yoff=30;
   
    //MAX LEN

    for(uint i=0;i<NUM_GEN;i++)
    {
        l = new Label(f);
        l->printF("gen %d", i);
        add(l, xoff, yoff);
        xoff+=10;
        yoff+=20;
        
        l = new Label(f);
        l->printF("hz");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, OSC_HZ + i)), xoff, yoff);
        
        yoff+=20;        
        l = new Label(f);
        l->printF("env on");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, ENV_ON + i)), xoff, yoff);

        yoff+=30;        
        l = new Label(f);
        l->printF("A");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, ENV_A + i)), xoff, yoff);
        yoff+=20;        
        l = new Label(f);
        l->printF("D");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, ENV_D + i)), xoff, yoff);
        yoff+=20;        
        l = new Label(f);
        l->printF("S");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, ENV_S + i)), xoff, yoff);
        yoff+=20;        
        l = new Label(f);
        l->printF("R");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, ENV_R + i)), xoff, yoff);
        yoff+=30;        
        l = new Label(f);
        l->printF("wave");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, OSC_WAVE + i)), xoff, yoff);
        yoff+=20;        
        l = new Label(f);
        l->printF("amp");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, OSC_AMP + i)), xoff, yoff);
        yoff+=20;        
        l = new Label(f);
        l->printF("ws");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, OSC_WS + i)), xoff, yoff);
        yoff+=30;        
        l = new Label(f);
        l->printF("lfo0 amp");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, LFO_AMP_0 + i)), xoff, yoff);
        yoff+=20;        
        l = new Label(f);
        l->printF("lfo0 hz");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, LFO_HZ_0 + i)), xoff, yoff);
        yoff+=20;        
        l = new Label(f);
        l->printF("lfo0 wave");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, LFO_WAVE_0 + i)), xoff, yoff);
        yoff+=20;        
        l = new Label(f);
        l->printF("lfo1 amp");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, LFO_AMP_1 + i)), xoff, yoff);
        yoff+=20;        
        l = new Label(f);
        l->printF("lfo1 hz");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, LFO_HZ_1 + i)), xoff, yoff);
        yoff+=20;        
        l = new Label(f);
        l->printF("lfo1 wave");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, LFO_WAVE_1 + i)), xoff, yoff);
        
        yoff+=30;        
        l = new Label(f);
        l->printF("glide");
        add(l, xoff+15, yoff);
        add(addInputTaker(new InputTaker(pix["in"], this, OSC_GLIDE + i)), xoff, yoff);

        xoff += 80;
        yoff = 30;
    }
    ground.addInput((Value**)&sig);
}

GAltZeroCtrl::~GAltZeroCtrl()
{
    delete sig;
    
    //make sure its quiet
    cout << "alt0 notes off..." << endl;

    for(uint i=0;i<NUM_GEN;i++)
    {
        char altZeroCmd[CMD_BYTES];
        cmdMake(CMD_SET, CMD_ENV, CMD_ENV_ON, i, 0, altZeroCmd);
        serial->write(altZeroCmd, CMD_BYTES);
    }
    
    serial->drain();

    serial->close();
    cout << "done" << endl;
}


/*Value** GAltZeroCtrl::getOutput(unsigned int id)
  {
  return (Value**)&sig;
  }
*/

void GAltZeroCtrl::addInput(Value** in, uint id)
{
    sig->setInput(in, id);
}

void GAltZeroCtrl::remInput(Value** out, uint id)
{
    sig->setInput(NULL, id);
}

/*******/
/*Value*/
/*******/

ProgramState& AltZeroCtrl::progstate = ProgramState::instance();

AltZeroCtrl::AltZeroCtrl()
{
    serial = NULL;
    
    for(uint i=0;i<NUM_INPUT;i++)inputs[i] = NULL;

    altZeroRespCount = 0;
    writeDelay = 0;

    struct timeval ta;
    gettimeofday(&ta, NULL);
    startSec = ta.tv_sec;
}

bool AltZeroCtrl::pushCmd(CmdTypeId type, CmdGroupId group, char param, char id, unsigned short data)
{
    if(writeDelay==0 && cmdsOut.size() < MAX_CMDS_OUT)
    {
        /*int haveToWaitIndex = (progstate.getSampleGenBuffer()+1) * progstate.getSampleGenLen() + 400;
        int haveToWaitTime = (haveToWaitIndex*1000000)/progstate.getSampleRate();
        */
        AltZeroCmd cmd;
        cmd.atIndex = index;
        
        /*struct timeval ta;
        gettimeofday(&ta, NULL);
        int us = (ta.tv_sec-startSec)*1000000+ta.tv_usec;
        cmd.atTime = us;
        cmd.trigAtTime = us + haveToWaitTime;
        */
        cmdMake(type, group, param, id, data, cmd.data);
        cmdsOut.push_back(cmd);
        //cout << cmdsOut.size() << endl;
        if(cmdsOut.size() >= MAX_CMDS_OUT)
            writeDelay = progstate.getSampleRate()/30;
        return true;
    }
    return false;
}

smp_t AltZeroCtrl::nextValue()
{
    if(serial == NULL)return 0;
    
    if (index % POLL_DELAY == 0)
    {
        string data;
        if(serial->read(&data))
        {
            cout << data << endl;
        }
            
        //output raw data for now
        /* char data[CMD_BYTES];
        if(serial->read(data, CMD_BYTES))
        {
            char type;
            char group;
            char param;
            char id;
            unsigned short val;
            cmdDecode(data, &type, &group,&param, &id, &val);
            cout << "alt0 says:" << endl;
            cout << "type: " << (int)type << endl;
            cout << "group: " << (int)group << endl;
            cout << "param: " << (int)param << endl;
            cout << "id: " << (int)id << endl;
            cout << "data: " << (int)val << endl;
            cout << endl;
            }*/
    }
    /*
    int haveToWaitIndex = (progstate.getSampleGenBuffer()+1) * progstate.getSampleGenLen();// + 400;
    int haveToWaitTime = (haveToWaitIndex*1000000)/progstate.getSampleRate();*/

    for(uint i=0;i<NUM_GEN;i++)
    {
        //these if's are priority of commands sent
        if(inputs[ENV_ON+i] != NULL)
        {
            bool on = (bool)((*inputs[ENV_ON+i])->nextValue());
            if (gen[i].on != on)
            {
                if(pushCmd(CMD_SET, CMD_ENV, CMD_ENV_ON, i, on))
                {
                    gen[i].on = on;
                    
/*                    if(gen[i].on)
                    {
                        struct timeval ta;
                        gettimeofday(&ta, NULL);
                        int timeNow = (ta.tv_sec-startSec)*1000000+ta.tv_usec;

                        int timeSinceLastShould = (int)((index - gen[i].lastTrigIndex) * (1000000 / (double)progstate.getSampleRate()));
                        //cout << index - gen[i].lastTrigIndex << " samples" << endl;
                        //cout << "op should take:" << timeSinceLastShould << endl;
                        
                        //int timeSinceLastWas = timeNow - gen[i].lastTrigTime;
                        
                        //int dTrig = timeSinceLastShould - timeSinceLastWas;
                        //int dt =  haveToWaitTime - dTrig;
                        //if(dt < 0)
                        //{
                        //    cout << "<<<<< wont make it" << endl;
                        //    dt=0;
                        //}
                        //dt = haveToWaitTime;
                        //cmdsOut.back().trigAtTime = timeNow + haveToWaitTime;//timeSinceLastShould;//dt;
                        cmdsOut.back().trigAtTime = gen[i].lastTrigTime + timeSinceLastShould;//dt;
                        gen[i].dt = timeSinceLastShould;
                        gen[0].t = gen[i].lastTrigTime;
                        gen[i].lastTrigIndex = index;
                    }*/
                }
            }
        }
        if(inputs[OSC_HZ+i] != NULL)
        {
            uint hz = round( 10.0 * (*inputs[OSC_HZ+i])->nextValue());
            if (gen[i].hz != hz)
            {
                if(pushCmd(CMD_SET, CMD_OSC, CMD_OSC_HZ, i, hz))gen[i].hz = hz;
                //cout << hz << endl;
            }
        }
        if(inputs[ENV_A+i] != NULL)
        {
            uint val = (uint)((*inputs[ENV_A+i])->nextValue()*0xffff);
            if(val > 0xffff)val=0xffff;

            if (gen[i].a != val)
            {
                if(pushCmd(CMD_SET, CMD_ENV, CMD_ENV_A, i, val))gen[i].a = val;
            }
        }
        if(inputs[ENV_D+i] != NULL)
        {
            uint val = (uint)((*inputs[ENV_D+i])->nextValue()*0xffff);
            if(val > 0xffff)val=0xffff;
            if (gen[i].d != val)
            {
                if(pushCmd(CMD_SET, CMD_ENV, CMD_ENV_D, i, val))gen[i].d = val;
            }
        }
        if(inputs[ENV_S+i] != NULL)
        {
            uint val = (uint)((*inputs[ENV_S+i])->nextValue()*0xffff);
            if(val > 0xffff)val=0xffff;
            if (gen[i].s != val)
            {
                if(pushCmd(CMD_SET, CMD_ENV, CMD_ENV_S, i, val))gen[i].s = val;
            }
        }
        if(inputs[ENV_R+i] != NULL)
        {
            uint val = (uint)((*inputs[ENV_R+i])->nextValue()*0xffff);
            if(val > 0xffff)val=0xffff;
            if (gen[i].r != val)
            {
                if(pushCmd(CMD_SET, CMD_ENV, CMD_ENV_R, i, val))gen[i].r = val;
            }
        }
        if(inputs[OSC_WAVE+i] != NULL)
        {
            uint val = (uint)((*inputs[OSC_WAVE+i])->nextValue());
            if(val > 4)val=4;
            if (gen[i].wave != val)
            {
                if(pushCmd(CMD_SET, CMD_OSC, CMD_OSC_WAVE, i, val))gen[i].wave = val;
            }
        }
        if(inputs[OSC_AMP+i] != NULL)
        {
            uint val = (uint)((*inputs[OSC_AMP+i])->nextValue()*0xff);
            if(val > 0xff)val=0xff;
            if (gen[i].amp != val)
            {
                if(pushCmd(CMD_SET, CMD_OSC, CMD_OSC_AMP, i, val))gen[i].amp = val;
                //cout << "amp: " << gen[i].amp << endl;
            }
        }
        if(inputs[OSC_WS+i] != NULL)
        {
            uint val = (uint)((*inputs[OSC_WS+i])->nextValue());
            if(val > 4)val=4;
            if (gen[i].ws != val)
            {
                if(pushCmd(CMD_SET, CMD_OSC, CMD_OSC_WS, i, val))gen[i].ws = val;
            }
        }
        if(inputs[LFO_HZ_0+i] != NULL)
        {
            uint val = (uint)((*inputs[LFO_HZ_0+i])->nextValue());
            if (gen[i].lfoHz[0] != val)
            {
                //cout << i*2 << endl;
                if(pushCmd(CMD_SET, CMD_LFO, CMD_LFO_HZ, i*2, val))gen[i].lfoHz[0] = val;
            }
        }
        if(inputs[LFO_HZ_1+i] != NULL)
        {
            uint val = (uint)((*inputs[LFO_HZ_1+i])->nextValue());
            if (gen[i].lfoHz[1] != val)
            {
                if(pushCmd(CMD_SET, CMD_LFO, CMD_LFO_HZ, 1+i*2, val))gen[i].lfoHz[1] = val;
            }
        }
        if(inputs[LFO_AMP_0+i] != NULL)
        {
            uint val = (uint)((*inputs[LFO_AMP_0+i])->nextValue()*0xff);
            if (val>0xff)val=0xff;

            if (gen[i].lfoAmp[0] != val)
            {
                if(pushCmd(CMD_SET, CMD_LFO, CMD_LFO_AMP, i*2, val))gen[i].lfoAmp[0] = val;
            }
        }
        if(inputs[LFO_AMP_1+i] != NULL)
        {
            uint val = (uint)((*inputs[LFO_AMP_1+i])->nextValue()*0xff);
            if (val>0xff)val=0xff;
            if (gen[i].lfoAmp[1] != val)
            {
                if(pushCmd(CMD_SET, CMD_LFO, CMD_LFO_AMP, 1+i*2, val))gen[i].lfoAmp[1] = val;
            }
        }
        if(inputs[LFO_WAVE_0+i] != NULL)
        {
            uint val = (uint)((*inputs[LFO_WAVE_0+i])->nextValue());
            if(val>4)val=4;
            if (gen[i].lfoWave[0] != val)
            {
                if(pushCmd(CMD_SET, CMD_LFO, CMD_LFO_WAVE, i*2, val))gen[i].lfoWave[0] = val;
            }
        }
        if(inputs[LFO_WAVE_1+i] != NULL)
        {
            uint val = (uint)((*inputs[LFO_WAVE_1+i])->nextValue());
            if(val>4)val=4;
            if (gen[i].lfoWave[1] != val)
            {
                if(pushCmd(CMD_SET, CMD_LFO, CMD_LFO_WAVE, 1+i*2, val))gen[i].lfoWave[1] = val;
            }
        }
        if(inputs[OSC_GLIDE+i] != NULL)
        {
            uint val = (uint)((*inputs[OSC_GLIDE+i])->nextValue()*0xffff);
            if(val > 0xffff)val=0xffff;
            if (gen[i].glide != val)
            {
                if(pushCmd(CMD_SET, CMD_OSC, CMD_OSC_GLIDE, i, val))gen[i].glide = val;
            }
        }
    }
    
    //cant turn it on directly if we want to stay in sync with mw audio
    //it will take X samples before anything
    //gets through mw, make alt0 wait that long to...
    
    //this sample was just generated, how many buffers are in front of us?
    //1.Main::smpQueueSize()*sampleGenLen() [waiting for upload]  + this samples index in current sample +
    //2.uploaded[th]->size*sampleGenLen() //already uploaded and not played
    //3. adev.inQueue()*sampleGenLen() in audio pipe
    //4 - number of samples it takes to send data to alt0 and for it to respond
    //=to complicated, make something up(for now)
    //it is impossible to know number 4 anyways


    //front always has least index
    //while(cmdsOut.size() &&  (abs((time - cmdsOut.front().atTime) - haveToWait) < 2 || time - cmdsOut.front().atTime > haveToWait))
    //while(cmdsOut.size() &&   index - cmdsOut.front().atIndex > haveToWait)

    while( cmdsOut.size() )
    {
        /*struct timeval ta;
        gettimeofday(&ta, NULL);
        int time = (ta.tv_sec-startSec)*1000000+ta.tv_usec;*/
        
        //cout << "trig at: " << cmdsOut.front().trigAtTime << endl;
        //cout << "time: " << time << endl;
        
        //if(cmdsOut.front().trigAtTime-5 > time)break;
        //if( abs( (double)(cmdsOut.front().trigAtTime - time)) > 10)break;
        

        if(serial->pollWrite() > 0)
        {
/*
            if(cmdsOut.front().data[1] == CMD_ENV && cmdsOut.front().data[5] == 1)
            {
                struct timeval ta;
                gettimeofday(&ta, NULL);
                int us = (ta.tv_sec-startSec)*1000000+ta.tv_usec;

                static int oldus=0;
                
                //cout << "it took: " << (time-oldus)/1000 << " ms" << endl;
                //cout << "dTime: now-wanted: " << (time - cmdsOut.front().trigAtTime)/1000   << endl;
                cout << "error in time since last trig: " << abs( (double)(gen[0].dt - (time-gen[0].t))) / 1000 << " ms" << endl;
                
                oldus = time;

                gen[0].lastTrigTime = time;

            }*/
            serial->write(cmdsOut.front().data, CMD_BYTES);
            cmdsOut.pop_front();
            //break;
        }
        else break;
    }
   
    if(writeDelay > 0)
    {
        writeDelay--;
    }
    index++;
    return 0;
}

void AltZeroCtrl::setInput(Value** r, uint index)
{
    inputs[index] = r;
}

void AltZeroCtrl::setSerial(Serial* serial)
{
    this->serial = serial;
}

AltZeroCtrl::Generator::Generator()
{
    hz = 55;
    on = 0;
    wave = 1;
    lastTrigTime = 0;
    lastTrigIndex = 0;
}
