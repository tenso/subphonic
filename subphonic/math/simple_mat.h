#ifndef SPL_MAT_D_H
#define SPL_MAT_D_H

#include <iostream>

namespace spl{

using namespace std;

/*SIMPLE data storage*/

template<typename TYPE, int H, int W>
class SimpleMat;

typedef SimpleMat<int,4,1> sVec4i;
typedef SimpleMat<int,3,1> sVec3i;

template<typename TYPE, int H, int W>
class SimpleMat
{
    friend ostream& operator <<(ostream& out, const SimpleMat<TYPE,H,W>& mat)
    {
        for(int i=0;i<H;i++)
        {
            if(i!=0)out << endl;
            for(int j=0;j<W;j++)
            {
                out << mat.data[i][j] << " ";
            }
        }
		     
        return out;
    }
   
  public:
    SimpleMat()
    {
        //D_ASSERT_M( H>0 && W>0, "invalid size");
        memset(data,0,H*W*sizeof(TYPE));
    }
   
    TYPE& operator()(int y=0, int x=0)
    {
        //D_ASSERT_M(x>=0 && x<W && y>=0 && y<H, "domain");
        return data[y][x];
    }
    TYPE operator()(int y=0, int x=0) const
    {
        //D_ASSERT_M(x>=0 && x<W && y>=0 && y<H, "domain");
        return data[y][x];
    }
    TYPE& operator[](int i)
    {
        TYPE* p = (TYPE*)data;
        return *(p+i); 
    }
   
  private:
    TYPE data[H][W];
};

}

#endif
