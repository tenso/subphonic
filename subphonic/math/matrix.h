/*"GRAPHICS MATRIX" fixed size(4x4), fixed type: float*/

/*COLUMN MAJOR AS OPENGL this means that in machine memory the complete first column comes first
 * OR; that row's count first.
 *
 * Linear memory 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15' map to "mathematical" matrix:
 *
 * 0  4  8  12
 * 1  5  9  13
 * 2  6  10 14
 * 3  7  11 15
 *
 * where it is seen for example that memory slot 1 is the first column of the second row
 * so to set col 2 row 2 elm 10 should be set in mem.
 * 
 * for same LinMem:
 * c++  indexing [r][c] => [0][0..3] =0  1  2  3
 *                         [1][0..3] =4  5  6  7
 *                         [2][0..3] =8  9  10 11
 *                         [3][0..3] =12 13 14 15
 * 
 * it is concluded that data[col][row] will be correct
 */

#ifndef SPL_MATRIX_H
# define SPL_MATRIX_H

# include <ostream>
# include <string.h>

# include "vec3.h"
# include "vec4.h"
# include "mathutil.h"

# include "../util/rand.h"

namespace spl{

# define USE_GL

# ifdef USE_GL
#include <GL/glew.h>
#include <GL/gl.h>
# endif

class LUPdata;
class Matrix;

ostream& operator<<(ostream& out, const Matrix& mat);
Matrix operator*(const float& rh, const Matrix& mat);

class Matrix
{
    friend ostream& spl::operator<<(ostream& out, const Matrix& mat);
    friend Matrix spl::operator*(const float& rh, const Matrix& mat);
   
  public:
   
# ifdef USE_GL
    //default is current matrix mode
    void loadGL(GLenum mode=GL_FALSE);
    void multGL(GLenum mode=GL_FALSE);
# endif
   
    Matrix()
    {
    }
   
    //set as it looks, i.e a, b, c, d is 1st row etc
    void set(float a,float b,float c,float d,
             float e,float f,float g, float h,
             float i, float j, float k, float l,
             float m, float n, float o, float p);
   
   
    //because of column-major order this is row first i.e i=0,1,2,3 => row's 0,1,2,3 etc
    float& operator[](int i);
    float  operator[](int i) const;
   
    //notice argument order
    float& operator()(int row, int col);
    float  operator()(int row, int col) const;
    float* ptr();
    Vec4   col(int i) const;
    Vec4   row(int i) const;
    Vec3   col3(int i) const;
    Vec3   row3(int i) const;
    void   setCol(int i, const Vec4& vec);
    void   setRow(int i, const Vec4& vec);
   
    bool operator==(const Matrix& m);
   
    Matrix operator+(const Matrix& rh) const;
    Matrix operator-(const Matrix& rh) const;
    void   operator+=(const Matrix& rh);
    void   operator-=(const Matrix& rh);
    Matrix operator*(const Matrix& rh) const;
    void   operator*=(const Matrix& rh);
    Matrix operator*(const float& rh);
    void   operator*=(const float& rh);
    Vec4   operator*(const Vec4& rh) const;
   
//#warning "what do to with Matrix4x4 * Vec3, breaks distlaw"
    //WARNING: this breaks for instance distributive law:if A,B matrix and v vector:
    //(A+B)*v != A*v+B*v
    //because of "normalization" by 'd' below
   
    //IN:  Vec3(x,y,z)
    //OUT: Vec3(a/d,b/d,c/d)
    //WHERE: Vec4[x y z 1] produces  Vec4(a,b,c,d) when multiplyed with matrix
    Vec3   operator*(const Vec3& rh) const;
   
    void   setZero();
    static Matrix zero();
    Matrix transpose() const;
    void setTranspose();
    Matrix abs() const;
    void setAbs();
    static Matrix rand(float min=0.0f, float max=1.0f); //uses rand.h
    void setRand(float min=0.0f, float max=1.0f);
    Matrix truncate(unsigned int decimals);
    void setTruncate(unsigned int decimals);
   
    //make a matrix A out of v so that A*v' = v cross v' 
    static Matrix crossMatrix(const Vec3& v);
   
    void setIdentity();
    void setTranslate(float x, float y, float z);
    void setScale(float x, float y, float z);
    void setRotate(float rad, const Vec3& axis);
    void setRotate(float rad, float x, float y, float z);
   
    static Matrix identity();
    static Matrix translate(float x, float y, float z);
    static Matrix translate(const Vec3& vec);
    static Matrix scale(float x, float y, float z);
    static Matrix scale(float u)
    {
        return scale(u,u,u);
    }
    static Matrix scale(const Vec3& vec);
    static Matrix rotate(float rad, const Vec3& axis);
    static Matrix rotate(float rad, float x, float y, float z);
   
    /*GRAPHICS: OPENGL-standars follows, floats instead of doubles*/
    static Matrix lookAt(const Vec3& from, const Vec3& at, const Vec3& up);
    static Matrix frustum(float l, float r, float b, float t, float n, float f);
    static Matrix perspective(float v_fov, float aspect /*w/h*/, float znear /*>0*/, float zfar /*>0*/);
    static Matrix ortho(float l, float r, float b, float t, float n, float f);
    static Matrix ortho2D(float left, float right, float bottom, float top);
   
   
    /********/
    /*EXTRAS*/
    /********/
   
    //might be moved
   
   
    //textbook standard
    /*(B1) BEZIER CURVE
      USAGE, the point will be: p = C*bezier()*t 
      where C is the matrix with the four control points i.e:
      C : = [C0 C1 C2 C3] where Ci=[x y z 1]~ and t := [t^3 t^2 t 1]~ where t[0,1] is the interpolation vector
      and ~ is transpose*/
    static Matrix bezier();
   
   
    //WARNING:
    //own derivation unknown acceptance, do not know what it "is", it is NOT a B-SPLINE
    //Reason for only 2 new CP when adding bezier segment:
    //restricion on first two CP from derivation, i.e c0' must be same as c0 and c1' must be set so that first
    //derivative matches, so only c2',c3' can be set by user
   
    /*(B2) PICEWISE BEZIER CURVE
      USAGE: to add 2 new control-points to an already existing bezier curve use 'old' control points - C, to get 
      * addition to curve as follows: C' = C*bezierSpline() with the two new control point n0,n1 added to C' after transformation
      * as: NC =  [c0' c1' n0 n1]. Then next curve will be as usual but with NC instead of C in (B1)
      * 
      * it will be continous and have first derivatives the same at splice point(s)
      */
    static Matrix picewiseBezier();
   
    //interface strange due to fail possibility
    static bool inverse(const Matrix& A, Matrix& ans);
   
    //solves Ax=b, i.e saves x in ans
    static bool solve(const Matrix& A, const Vec4& b, Vec4& ans);
   
    //decomposes A to PA=LU and returns P,L,U
    static LUPdata LUPdecompose(const Matrix& mat);
    static Vec4 LUPsolve(const LUPdata& data, Vec4 b);
   
  private:
    float data[4][4]; //column_major, as opengl so: data[COLUMN][ROW]
};

class LUPdata
{
  public:
    Matrix L,U, P; //lower/upper, permutation matrices
    int   p[4];    //permuitation, p[i] is column of '1' in p-matrix, row i
    bool   ok;     
};

}

#endif
