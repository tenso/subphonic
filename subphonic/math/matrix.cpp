#include "matrix.h"

#include "mathutil.h"
#include "../util/debug.h"

namespace spl{

#ifdef USE_GL
void Matrix::loadGL(GLenum mode)
{
    if(mode!=GL_FALSE)
    {
        D_ASSERT(mode==GL_MODELVIEW || mode==GL_PROJECTION
        || mode==GL_TEXTURE || mode==GL_COLOR);
	
        glPushAttrib(GL_TRANSFORM_BIT);
	
        glMatrixMode(mode);
        glLoadMatrixf(ptr());
	
        glPopAttrib();
    }
    else  glLoadMatrixf(ptr());
}
void Matrix::multGL(GLenum mode)
{
    if(mode!=GL_FALSE)
    {
        D_ASSERT(mode==GL_MODELVIEW || mode==GL_PROJECTION
        || mode==GL_TEXTURE || mode==GL_COLOR);
	
        glPushAttrib(GL_TRANSFORM_BIT);
	
        glMatrixMode(mode);
        glMultMatrixf(ptr());
	
        glPopAttrib();
    }
    else  glMultMatrixf(ptr());
}
#endif

ostream& operator<<(ostream& out, const Matrix& mat)
{
    out << mat.data[0][0] << " " << mat.data[1][0] << " " << mat.data[2][0] << " " << mat.data[3][0] <<endl;
    out << mat.data[0][1] << " " << mat.data[1][1] << " " << mat.data[2][1] << " " << mat.data[3][1] <<endl;
    out << mat.data[0][2] << " " << mat.data[1][2] << " " << mat.data[2][2] << " " << mat.data[3][2] <<endl;
    out << mat.data[0][3] << " " << mat.data[1][3] << " " << mat.data[2][3] << " " << mat.data[3][3];
    return out;
}

Matrix operator*(const float& rh, const Matrix& mat)
{
    Matrix ret;
   
    float* s = &ret.data[0][0];
    const float* d = &mat.data[0][0];
    for(int i=0;i<16;i++)s[i]=d[i]*rh;
   
    return ret;
}

void Matrix::set(float a,float b,float c,float d,
float e,float f,float g, float h,
float i, float j, float k, float l,
float m, float n, float o, float p)
{
    data[0][0]=a; data[1][0]=b; data[2][0]=c; data[3][0]=d;
    data[0][1]=e; data[1][1]=f; data[2][1]=g; data[3][1]=h;
    data[0][2]=i; data[1][2]=j; data[2][2]=k; data[3][2]=l;
    data[0][3]=m; data[1][3]=n; data[2][3]=o; data[3][3]=p;
}

bool Matrix::operator==(const Matrix& m)
{
    return (memcmp(data, m.data, sizeof(float)*16)==0);
}

Matrix Matrix::abs() const
{
    Matrix ret;
   
    ret=*this;
    ret.setAbs();
   
    return ret;
}

Matrix Matrix::truncate(unsigned int decimals)
{
    D_ERROR("dont use me");
    return Matrix::zero();
    /*Matrix ret;
     * 
     float* d = &data[0][0];
     for(int i=0;i<16;i++)ret[i]=trunc(d[i], decimals);
     return ret;*/
}

void Matrix::setTruncate(unsigned int decimals)
{
    D_ERROR("dont use me");
    /*float* d = &data[0][0];
      for(int i=0;i<16;i++)d[i]=trunc(d[i], decimals);*/
}

void Matrix::setAbs()
{
    float* d = &data[0][0];
    for(int i=0;i<16;i++)d[i]=fabs(d[i]);
}

Matrix Matrix::rand(float min, float max)
{
    Matrix ret;
    ret.setRand(min,max);
    return ret;
}

void Matrix::setRand(float min, float max)
{
    for(int i=0;i<16;i++)(*this)[i]=min+(max-min)*Rand::next();
}


float& Matrix::operator[](int i)
{
    return (&data[0][0])[i];
}

float Matrix::operator[](int i) const
{
    return (&data[0][0])[i];
}


float& Matrix::operator()(int row, int col)
{
    return data[col][row];
}
float Matrix::operator()(int row, int col) const
{
    return data[col][row];
}
float* Matrix::ptr()
{
    return &data[0][0];
}

Vec4 Matrix::col(int i) const
{
    return Vec4(data[i][0], data[i][1], data[i][2], data[i][3]);
}
Vec4 Matrix::row(int i) const
{
    return Vec4(data[0][i], data[1][i], data[2][i], data[3][i]);
}

Vec3 Matrix::col3(int i) const
{
    return Vec3(data[i][0], data[i][1], data[i][2] );
}

Vec3 Matrix::row3(int i) const
{
    return Vec3(data[0][i], data[1][i], data[2][i]);
}

void Matrix::setCol(int i, const Vec4& vec)
{
    data[i][0]=vec[0];
    data[i][1]=vec[1];
    data[i][2]=vec[2];
    data[i][3]=vec[3];
}
void Matrix::setRow(int i, const Vec4& vec)
{
    data[0][i]=vec[0];
    data[1][i]=vec[1];
    data[2][i]=vec[2];
    data[3][i]=vec[3];
}

Matrix Matrix::crossMatrix(const Vec3& v)
{
    Matrix ret;
    ret.data[0][0]=0; ret.data[1][0]=-v[2]; ret.data[2][0]=v[1]; ret.data[3][0]=0;
    ret.data[0][1]=v[2]; ret.data[1][1]=0; ret.data[2][1]=-v[0]; ret.data[3][1]=0;
    ret.data[0][2]=-v[1]; ret.data[1][2]=v[0]; ret.data[2][2]=0; ret.data[3][2]=0;
    ret.data[0][3]=0; ret.data[1][3]=0; ret.data[2][3]=0; ret.data[3][3]=1;
   
    return ret;
}


Matrix Matrix::operator*(const Matrix& rh) const
{
    Matrix ret;
   
    for(int i=0;i<4;i++)
    {
        ret.data[0][i]=(data[0][i]*rh.data[0][0]+data[1][i]*rh.data[0][1]
        +data[2][i]*rh.data[0][2]+data[3][i]*rh.data[0][3]);
	
        ret.data[1][i]=(data[0][i]*rh.data[1][0]+data[1][i]*rh.data[1][1]
        +data[2][i]*rh.data[1][2]+data[3][i]*rh.data[1][3]);
	
        ret.data[2][i]=(data[0][i]*rh.data[2][0]+data[1][i]*rh.data[2][1]
        +data[2][i]*rh.data[2][2]+data[3][i]*rh.data[2][3]);
	
        ret.data[3][i]=(data[0][i]*rh.data[3][0]+data[1][i]*rh.data[3][1]
        +data[2][i]*rh.data[3][2]+data[3][i]*rh.data[3][3]);
	
    }
    return ret;
}

void Matrix::operator*=(const Matrix& rh)
{
    (*this)=(*this)*rh;
}

Matrix Matrix::operator-(const Matrix& rh) const
{
    Matrix ret;
    for(int i=0;i<16;i++)ret[i]=(*this)[i]-rh[i];
    return ret;
}

void Matrix::operator-=(const Matrix& rh)
{
    (*this)=(*this)-rh;
}

Matrix Matrix::operator+(const Matrix& rh) const
{
    Matrix ret;
    for(int i=0;i<16;i++)ret[i]=(*this)[i]+rh[i];
    return ret;
}

void Matrix::operator+=(const Matrix& rh)
{
    (*this)=(*this)+rh;
}

Matrix Matrix::operator*(const float& rh)
{
    Matrix mat = *this;
    float* s = &data[0][0];
    float* d = &mat.data[0][0]; 
    for(int i=0;i<16;i++)d[i]=s[i]*rh;
   
    return mat;
}

void Matrix::operator*=(const float& rh)
{
    float* s = &data[0][0];
    for(int i=0;i<16;i++)s[i]*=rh;
}


Vec3 Matrix::operator*(const Vec3& rh) const
{
    Vec3 ret;
   
    float w = 1.0/(data[0][3]*rh[0]+data[1][3]*rh[1]+data[2][3]*rh[2]+data[3][3]);
   
    ret[0] = w*(data[0][0]*rh[0]+data[1][0]*rh[1]+data[2][0]*rh[2]+data[3][0]);
    ret[1] = w*(data[0][1]*rh[0]+data[1][1]*rh[1]+data[2][1]*rh[2]+data[3][1]);
    ret[2] = w*(data[0][2]*rh[0]+data[1][2]*rh[1]+data[2][2]*rh[2]+data[3][2]);
   
    return ret;
}
Vec4 Matrix::operator*(const Vec4& rh) const
{
    Vec4 ret;
   
    ret[0] = data[0][0]*rh[0]+data[1][0]*rh[1]+data[2][0]*rh[2]+data[3][0]*rh[3];
    ret[1] = data[0][1]*rh[0]+data[1][1]*rh[1]+data[2][1]*rh[2]+data[3][1]*rh[3];
    ret[2] = data[0][2]*rh[0]+data[1][2]*rh[1]+data[2][2]*rh[2]+data[3][2]*rh[3];
    ret[3] = data[0][3]*rh[0]+data[1][3]*rh[1]+data[2][3]*rh[2]+data[3][3]*rh[3];
   
    return ret;
}

void Matrix::setZero()
{
    memset(data, 0, sizeof(float)*16); //fixme: precalc
}

Matrix Matrix::zero()
{
    Matrix ret;
    ret.setZero();
    return ret;
}



void Matrix::setIdentity()
{
    setZero();
    data[0][0]=1.0f;
    data[1][1]=1.0f;
    data[2][2]=1.0f;
    data[3][3]=1.0f;
}

Matrix Matrix::identity()
{
    Matrix ret;
    ret.setIdentity();
    return ret;
}


Matrix Matrix::transpose() const
{
    Matrix ret;
   
    ret.setRow(0,col(0));
    ret.setRow(1,col(1));
    ret.setRow(2,col(2));
    ret.setRow(3,col(3));
   
    return ret;
}

void Matrix::setTranspose()
{
    Matrix tmp=*this;
    (*this)=tmp.transpose();
}


bool Matrix::inverse(const Matrix& A, Matrix& ans)
{
    //solve A*x_i=e_i, for each base e
   
    LUPdata data = LUPdecompose(A);
    if(!data.ok)
    {
        //D_ERROR("failed to invert matrix");
        return false;
    }
   
    ans.setIdentity();
   
    for(int i=0;i<4;i++)ans.setCol(i, LUPsolve(data, ans.col(i)));
   
    return true;
}


//solves Ax=b, i.e saves x in ans
bool Matrix::solve(const Matrix& A, const Vec4& b, Vec4& ans)
{
    LUPdata data = LUPdecompose(A);
    if(data.ok)
    {
        ans = LUPsolve(data, b);
        return true;
    }
   
    D_ERROR("failed to solve system");
    return false;
}

void Matrix::setTranslate(float x, float y, float z)
{
    setIdentity();
    data[3][0]=x;
    data[3][1]=y;
    data[3][2]=z;
}
void Matrix::setScale(float x, float y, float z)
{
    setIdentity();
    data[0][0]=x;
    data[1][1]=y;
    data[2][2]=z;
}
void Matrix::setRotate(float rad, const Vec3& axis)
{
    setRotate(rad, axis.x(), axis.y(), axis.z());
}
void Matrix::setRotate(float rad, float x, float y, float z)
{
    float l = 1.0f/sqrt(x*x+y*y+z*z);
    x*=l; y*=l; z*=l;
   
   
    float s=sin(rad);
    float c=cos(rad);
    setIdentity();
    data[0][0]=c+(1.0f-c)*x*x;   data[1][0]=(1.0f-c)*x*y-s*z; data[2][0]=(1.0f-c)*x*z+s*y;
    data[0][1]=(1.0f-c)*x*y+s*z; data[1][1]=c+(1.0f-c)*y*y;   data[2][1]=(1.0f-c)*y*z-s*x;
    data[0][2]=(1.0f-c)*x*z-s*y; data[1][2]=(1.0f-c)*y*z+s*x; data[2][2]=c+(1.0f-c)*z*z;
   
}

Matrix Matrix::translate(float x, float y, float z)
{
    Matrix ret;
    ret.setTranslate(x,y,z);
    return ret;
}
Matrix Matrix::translate(const Vec3& vec)
{
    Matrix ret;
    ret.setTranslate(vec[0],vec[1],vec[2]);
    return ret;
}
Matrix Matrix::scale(float x, float y, float z)
{
    Matrix ret;
    ret.setScale(x,y,z);
    return ret;
}
Matrix Matrix::scale(const Vec3& vec)
{
    Matrix ret;
    ret.setScale(vec[0],vec[1],vec[2]);
    return ret;
}
Matrix Matrix::rotate(float rad, const Vec3& axis)
{
    return rotate(rad, axis.x(), axis.y(), axis.z());
}
Matrix Matrix::rotate(float rad, float x, float y, float z)
{
    Matrix rot;
    rot.setRotate(rad,x,y,z);
    return rot;
}


/*OPENGL follows, floats instead of doubles*/
Matrix Matrix::lookAt(const Vec3& from, const Vec3& at, const Vec3& up)
{
    Matrix look;
    look.setIdentity();
   
    Vec3 v = at-from; //z
    Vec3 u = v^up;    //x
    Vec3 w = u^v;     //y
    v.normalize(); u.normalize(); w.normalize();
   
    look.data[0][0]=u[0]; look.data[1][0]=u[1]; look.data[2][0]=u[2];
    look.data[0][1]=w[0]; look.data[1][1]=w[1]; look.data[2][1]=w[2];
    look.data[0][2]=-v[0]; look.data[1][2]=-v[1]; look.data[2][2]=-v[2];
   
    look*=translate(-from.x(),-from.y(),-from.z());
   
    return look;
}

Matrix Matrix::frustum(float l, float r, float b, float t, float n, float f)
{	
    Matrix frus;
    frus.setZero();
   
    frus.data[0][0]=2.0f*n/(r-l); /*0*/  frus.data[2][0]= (r+l)/(r-l); /*0*/
    /*0*/  frus.data[1][1]=2.0f*n/(t-b); frus.data[2][1]=(t+b)/(t-b);  /*0*/
    /*0*/  /*0*/  frus.data[2][2]=-1.0f;    frus.data[3][2]=-2.0f*n;
    /*0*/  /*0*/  frus.data[2][3]=-1.0f;    /*0*/
   
    return frus;
}

Matrix Matrix::perspective(float v_fov, float aspect /*w/h*/, float znear /*>0*/, float zfar /*>0*/)
{
    float l,r,t,b,n,f;
   
    n=znear;
    f=zfar;
    t=n*tan((M_PI*v_fov/180.0f)/2.0f);
    b=-t;
    l=b*aspect;
    r=t*aspect;
   
    return frustum(l,r,b,t,n,f);
}

Matrix Matrix::ortho(float l, float r, float b, float t, float n, float f)
{
    Matrix ret;
    ret.setZero();
    ret.data[0][0] = 2.0f/(r-l); ret.data[3][0]=-(r+l)/(r-l);
    ret.data[1][1] = 2.0f/(t-b); ret.data[3][1]=-(t+b)/(t-b);
    ret.data[2][2] = -2.0f/(f-n); ret.data[3][2] = -(f+n)/(f-n);
    ret.data[3][3]=1.0f;
   
    return ret;
}
Matrix Matrix::ortho2D(float left, float right, float bottom, float top)
{
    return ortho(left, right, bottom, top, -1.0f, 1.0f);
}



Vec4 Matrix::LUPsolve(const LUPdata& data, Vec4 b)
{
    Vec4 ret;
   
    int size=4; //FIXME: could solve any square matrix, just change size
    //first solve Ly = Pb where y=Ux by forward subst
   
    for(int i=0;i<size;i++)
    {
        float sum=0;
        for(int j=0;j<i;j++)sum+=ret[j]*data.L(i,j);
        ret[i]=b[data.p[i]]-sum;
    }
   
    //now solve Ux = y by back-subst
   
    for(int i=size-1;i>=0;i--)
    {
        float sum=0;
        for(int j=i+1;j<size;j++)sum+=ret[j]*data.U(i,j);
        ret[i]=(ret[i]-sum)/data.U(i,i);
    }
   
    return ret;
}

#define SWAPF(x, y)                             \
    {                                           \
        float tmp=*x;                           \
        *x=*y;                                  \
        *y=tmp;                                 \
    }

#define SWAPI(x, y)                             \
    {                                           \
        int tmp=*x;                             \
        *x=*y;                                  \
        *y=tmp;                                 \
    }


//decomposes A to PA=LU and returns P,L,U
//FIXME: solving inline is faster...
LUPdata Matrix::LUPdecompose(const Matrix& mat)
{
    LUPdata ret;
    ret.ok=false;
    ret.L.setIdentity();
    ret.U.setZero();
    Matrix work=mat;
    Matrix work2=mat;
   
    int size=4; //FIXME: could decompose any square matrix, just change size
   
    //identity permutation
    for(int i=0;i<size;i++)ret.p[i]=i;
   
    for(int i=0;i<size;i++)
    {
        //find pivot;
        float p=0;
        int pr=0;
	
        for(int j=i;j<size;j++)
        {
            float fval = fabs(work(j,i));
            if(fval>p)
            {
                p=fval;
                pr=j;
            }
        }
        if(p==0)
        {
            //D_ERROR("LUPdecomposition: singular matrix");
            return ret;
        }
	
        //change permut 'mat'
	
        SWAPI(&ret.p[i], &ret.p[pr]);
	
	
        //row exchange
        for(int j=0;j<size;j++)
        {
            SWAPF(&work(i, j), &work(pr, j));
        }
        for(int j=0;j<i;j++)
        {
            SWAPF(&ret.L(i,j), &ret.L(pr,j));
        }
	
	
        ret.U(i,i)=work(i,i);
	
        for(int k=i+1;k<size;k++)
        {
            ret.U(i,k)=work(i,k);
            ret.L(k,i)=work(k,i)/ret.U(i,i);
        }
	
        for(int k=i+1;k<size;k++)
        {
            for(int j=i+1;j<size;j++)
            {
                work(k,j) = work(k,j)-ret.L(k,i)*ret.U(i,j);
            }
	     
        }
    }
    ret.P.setZero();
    for(int i=0;i<4;i++)ret.P(ret.p[i],i)=1.0f;
    ret.ok=true;
   
    return ret;
}

/********/
/*BEZIER*/
/********/

Matrix Matrix::bezier()
{
    Matrix B;
    B.set(-1, 3,-3, 1,
    3,-6, 3, 0,
    -3, 3, 0, 0,
    1, 0, 0, 0);
    return B;
}

Matrix Matrix::picewiseBezier()
{
    Matrix S;
/*   S.set(0, 0, 0, 0,
     0, 0, 1, 0,
     0,-1,-4, 0,
     1, 2, 4, 0);*/
   
    //derivation follows from want of continous curve and same first derivative at splice point
    S.set(0, 0, 0, 0,
    0, 0, 0, 0,
    0,-1, 0, 0,
    1, 2, 0, 0);
   
    return S;
}

}
