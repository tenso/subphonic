
/**************/
/*NumberSelect*/
/**************/
template<class T>
NumberSelect<T>::NumberSelect(BitmapFont* font, unsigned int num_whole, unsigned int num_frac, T* data
, bool use_sign, SIGN_PLACEMENT place)
{
    //FIXMENOW: not depend on surfhold
    SurfaceHoldAutoBuild& surf = SurfaceHoldAutoBuild::instance();
   
    this->use_sign=use_sign;
   
    //DASSERT(data!=NULL);
   
    //something will add this container to itself let it desicde pos do with this for now
    setPos(0,0);
   
    this->nw=num_whole;
    this->nf=num_frac;
    n=nw+nf;
   
    internal_data=0;
   
    if(data != NULL)
        this->data=data;
    else this->data = &internal_data;
   
    //so delete is ok...
    digits_w=NULL;
    digits_f=NULL;
   
    if(nw>0)digits_w = new DigitSelect*[nw];
    if(nf>0)digits_f = new DigitSelect*[nf];
   
    int neg_off=0;
    sign_char=NULL;
    uint ch = /*surf["uparrow_up"]->h + surf["uparrow_down"]->h + */surf["digsel_back"]->h;
     
    if(use_sign)
    {
        sign_char = new SignSelect(surf["b_plus_up"], surf["b_plus_down"],
        surf["b_minus_up"],surf["b_minus_down"],
        surf["digsel_back"],
        font, new C_Sign(this));
	
        if(place==LEFT)
        {
            add(sign_char, 0, 0);
            neg_off=8;
        }
        if(place==OVER)
        {
            add(sign_char, 0, -ch);
        }
        if(place==UNDER)
        {
            add(sign_char, 0, ch);
        }
	
    }
      
    //WARNING:
    //digit select is a container so because of setPos not being recursive in Container
    //the final pos is needed in the constructor to DigitSelect, a Container::add wouldnt
    //update the things in DigitSelct Properly
    for(unsigned int i=0;i<nw;i++)
    {
        //must make new C_Fire for each... deleted in
        digits_w[i] = new DigitSelect(surf["uparrow_up"], surf["uparrow_down"],
        surf["downarrow_up"],surf["downarrow_down"],
        surf["digsel_back"],
        font, new C_Fire(this));
	
        add(digits_w[i],neg_off+i*8, 0);
    }
   
    int frac_off=8+nw*8;
    if(nf>0)
    {
        Label* l = new Label(font, string("."));
        add(l, neg_off+nw*8+1, 8);
    }
   
   
    for(unsigned int i=0;i<nf;i++)
    {
        digits_f[i] = new DigitSelect(surf["uparrow_up"], surf["uparrow_down"],
        surf["downarrow_up"],surf["downarrow_down"],
        surf["digsel_back"],
        font, new C_Fire(this) );
	
        add(digits_f[i], neg_off+i*8+frac_off, 0);
    }
   
    coord_t max;
    max=powf(10,nw);
    max-=(1/powf(10,nf));
   
    setMax((T)max);
    if(!use_sign)setMin(0);
    else setMin((T)-max);
   
    setValue(*this->data);
}

template<class T>
NumberSelect<T>::~NumberSelect()
{
    //this is OK: add() have pointers will delete them so only delete these:
    delete[] digits_w;
    delete[] digits_f;
}

template<class T>
T NumberSelect<T>::clip(T v)
{
    if(v<min)v=min;
    if(v>max)v=max;
    return v;
}

template<class T>
T NumberSelect<T>::getValue()
{
    return *data;
}

template<class T>
void NumberSelect<T>::setValue(T val)
{
    stringstream str;
    str.precision(nf);
    str << fixed <<  val;

    setValue(str.str());
}


template<class T>void NumberSelect<T>::setValue(const string& str)
{

    istringstream in(str);
    in.precision(nf);
    T val=0;
   
    in >> val;
    val = clip(val);
      
    //make this so that string value is also clipped
    str_rep.clear();
   
    stringstream ss;
    ss.precision(nf);
   
    ss << fixed << val;
    ss >> str_rep;
   
    if(str[0]=='-')
    {
        if(str_rep[0]!='-')
        {
            //this happens if -0; want to keep -
            str_rep = "-" + str_rep;
        }
    }
   
    *data=val;
   
   
    //clear digits
    for(unsigned int i=0;i<nw;i++)
    {
        digits_w[i]->setChar('0');
    }
    for(unsigned int i=0;i<nf;i++)
    {
        digits_f[i]->setChar('0');
    }
   
    int len = str_rep.length();
    size_t to_point = str_rep.find_first_of(".");
   
   
    //get number of whole and frac's
    unsigned int numwhole;
    unsigned int numfrac;
   
    int negoff=0;
    if(use_sign)
    {
        if(str_rep[0]=='-')
        {
            sign_char->setSign(-1);
            negoff=1;
        }
        else
        {
            sign_char->setSign(1);
        }
    }
    else if(str_rep[0]=='-')
    {
        DERROR("use of negative numbers not set");
    }
   
    if(to_point==string::npos)
    {
        numwhole=len-negoff;
        to_point=len;
	
        numfrac=0;
    }
    else if(to_point==0)
    {
        numwhole=0;
        numfrac=len-1;
    }
    else
    {
        numwhole=to_point-negoff;
        numfrac=len-numwhole-1-negoff;
    }
   
    DASSERTP(numwhole<=nw, "str whole part to big");
    DASSERTP(numfrac<=nf, "str frac part to big");
   
    for(unsigned int i=0;i<numwhole;i++)
    {
        //set digits in rev order, so to preserve '0' on top digits
        unsigned int index = nw-1-i;
        DASSERT(index<nw);
	
        digits_w[index]->setChar(str_rep[to_point-i-1]);
    }
   
    //do frac
    for(unsigned int i=0;i<numfrac;i++)
    {
        unsigned int index = i;
        DASSERT(index<nf);
	
        digits_f[index]->setChar(str_rep[to_point+1+i]);
    }
   
}

template<class T>
string NumberSelect<T>::getStr()
{
    return str_rep;
}

template<class T>
NumberSelect<T>::C_Fire::C_Fire(NumberSelect* s)
{
    src=s;
}

template<class T>
void NumberSelect<T>::C_Fire::action(Component* co)
{
    stringstream sstr;
    sstr.precision(src->nf/*+src->nw*/);
   
    if(src->use_sign)
    {
        sstr << src->sign_char->getChar();
    }
   
   
    for(unsigned int i=0;i<src->nw;i++)
    {
        sstr << fixed << src->digits_w[i]->getChar();
    }
   
    sstr << ".";
   
    for(unsigned int i=0;i<src->nf;i++)
    {
        sstr << fixed << src->digits_f[i]->getChar();
    }

    src->setValue(sstr.str());
   
    src->action();
}



template<class T>
NumberSelect<T>::C_Sign::C_Sign(NumberSelect* s)
{
    src=s;
}

template<class T>
void NumberSelect<T>::C_Sign::action(Component* co)
{
    string val = src->getStr();
   
    SignSelect* sign = (SignSelect*)co;
    int pos = sign->getSign();
   
    if(pos==1)
    {
        if(val[0]=='-')val=val.substr(1);
    }
    else
    {
        if(val[0]!='-')val= '-' + val;
    }
   
    src->setValue(val);
   
    src->action();
}

template<class T>
void NumberSelect<T>::setMin(T m)
{
    min=m;
}

template<class T>
T NumberSelect<T>::getMin()
{
    return min;
}

template<class T>
void NumberSelect<T>::setMax(T m)
{
    max=m;
}

template<class T>
T NumberSelect<T>::getMax()
{
    return max;
}

