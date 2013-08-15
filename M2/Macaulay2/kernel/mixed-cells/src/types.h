#define MAX_RANDOM_INT 50
#define ABS(d) (((d)>=0)?(d):-(d))
#define MAX(a,b) (((a)>(b))?(a):(b))
namespace mixedCells
{
  // service functions ////////////////////////////////////////////////
  int round(double x) { return (x>0)? int(x+.499999) : int(x-.499999); }
  
  /// returns g = gcd(a,b)
  /// sgn(g) == sgn(a) 
  int gcd(int a, int b) 
  {
    bool p = (a>=0);
    while (true) {
      if (b == 0) return ((a>=0)==p)?a:-a;
      else {
	int c = a%b;
	a = b;
	b = c;
      }
    }
  }
  bool isPositive(double a)
  {
    return a>EPSILON;
  }
  bool isNegative(double a)
  {
    return a<-EPSILON;
  }
  bool isZero(double a)
  {
    return (a<EPSILON)&&(a>-EPSILON);
  }
  bool isZero2(double a)
  {
    return (a<EPSILON)&&(a>-EPSILON);
  }

  int volumeToInt(double d)
  {
    return (int)(ABS(d)+0.25);
  }

  double toDoubleForPrinting(double s)
  {
    return s;
  }

  bool isGreaterEqual(double a, double b)
  {
    return a>=b;
  }

  bool isEpsilonLessThan(double a, double b)
  {
    return a+EPSILON<b;
  }

  class DoubleGen; // g++ 4.1.2 wants this
  
  /** An inmitation integer class
   */


};// end namespace mixedCells

#include "type_doubleint.h"
#include "type_shortint.h"
#include "type_doublegen.h"
#include "type_ratgen.h"
