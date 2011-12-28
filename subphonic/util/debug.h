#ifndef SPL_DEBUG_H
# define SPL_DEBUG_H

# include <iostream>
//# include <time.h>

namespace spl{

#define DEBUG 1

//DEBUG needs to be defined for DEBUG_HIGH

#ifdef LINUX
#  define ERROR( str ) std::cerr << __PRETTY_FUNCTION__ << " in " << __FILE__ << "<" << __LINE__ << ">" << " : " << str << std::endl

#  define CHK_GL_ERROR(){                                               \
        GLenum err=glGetError();                                        \
        if(err!=GL_NO_ERROR){                                           \
            std::cerr << __FILE__ << "<" << __LINE__ << ">"             \
                      << " : " << "GL error: " << hex << err << "h" << std::endl; \
        }                                                               \
    }
#endif

# ifndef DEBUG
#  ifdef DEBUG_HIGH
#   error DEBUG_HIGH without DEBUG
#  endif

#  define D_MSG( str )
#  define D_ERROR( str ) 
#  define D_ENTER 
#  define D_EXIT 
#  define D_ASSERT( val )
#  define D_ASSERT_M( val, txt )
#  define D_OUT(str) 
#  define D_GL_ERROR()
#  define D_DEBUGCODE(x)
#  define D_ASSERT_HIGH(val)
#  define D_ASSERT_M_HIGH(val, txt)

# else
/*#  define D_ERROR( str ) std::cerr << __PRETTY_FUNCTION__ << " in " << __FILE__ << "<" << __LINE__ << ">" << " : " << str << std::endl
#  define D_ENTER std::cerr << "enter " << __FUNCTION__ << " in " << __FILE__ << "<" << __LINE__ << ">" <<  std::endl
#  define D_EXIT std::cerr << "exit " << __FUNCTION__ << " in " << __FILE__ << "<" << __LINE__ << ">" <<  std::endl*/
#  define D_ERROR( str ) std::cerr << str << std::endl
#  define D_ENTER
#  define D_EXIT
#  define D_ASSERT( val ) if(!(val)){D_ERROR("assert fail");/*exit(1)*/;}
#  define D_ASSERT_M( val, txt ) if(!(val)){D_ERROR(txt);/*exit(1)*/;}
#  define D_OUT(str) {std::cerr << str;std::cerr.flush();}
#  define D_DEBUGCODE(x)  x 

#  define D_GL_ERROR(){													\
        GLenum err=glGetError();                                        \
        if(err!=GL_NO_ERROR){                                           \
            std::cerr << __FILE__ << "<" << __LINE__ << ">"				\
                      << " : " << "GL error: " << hex << err << "h" << std::endl; \
        }                                                               \
    }
//used for very high overhead asserts that might not even be used in debug mode

#  ifdef DEBUG_HIGH
#   define D_ASSERT_HIGH( val ) D_ASSERT(val)
#   define D_ASSERT_M_HIGH(val, txt) D_ASSERT_M(val, txt)
#  else 
#   define  D_ASSERT_HIGH(val)
#   define D_ASSERT_M_HIGH(val, txt)
#  endif

# endif

#define D_MSG( str ) //std::cout << str << std::endl;
#define ERROR( str ) std::cerr << str << std::endl;
#define CHK_GL_ERROR(str) 
#define DASSERT(x) D_ASSERT(x)
#define DERROR(x) D_ERROR(x)
#define DASSERTP(val, msg) D_ASSERT_M(val, msg)
#define VERBOSE2(msg) //msg
#define VERBOSE1(msg) //msg
#define VERBOSE0(msg) //msg

/*TIMINGS*/
	/*
extern double __ta_usecs;
extern double __ta_usecs_avg;
extern double __ta_usecs_sum;
extern int __ta_timings;
extern timeval __ta_tim;

# define D_TICK()                                               \
    {                                                           \
        gettimeofday(&__ta_tim, NULL);                          \
        __ta_usecs=__ta_tim.tv_sec*1000000+__ta_tim.tv_usec;	\
    }

# define D_TACK()														\
    {																	\
        gettimeofday(&__ta_tim, NULL);									\
        __ta_usecs=__ta_tim.tv_sec*1000000+__ta_tim.tv_usec-__ta_usecs;	\
    }

# define D_TIMER_MICRO_S __ta_usecs

# define D_TIMER_CLEAR_AVG()					\
    {											\
        __ta_timings=0;							\
        __ta_usecs_avg=0;                       \
    }

# define D_TIMER_SAVE_AVG()						\
    {											\
        __ta_timings++;							\
        __ta_usecs_avg+=__ta_usecs;				\
    }

# define D_TIMER_AVG_MICRO_S  __ta_usecs_avg/(double)__ta_timings


# define D_TIMER_CLEAR_SUM()					\
    {											\
        __ta_usecs_sum=0;                       \
    }

# define D_TIMER_SAVE_SUM()						\
    {											\
        __ta_usecs_sum+=__ta_usecs;				\
    }

# define D_TIMER_SUM_MICRO_S  __ta_usecs_sum
*/
//}//end namespace std

}

#endif
