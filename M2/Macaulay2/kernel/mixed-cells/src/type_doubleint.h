#ifndef TYPEDOUBLEINT_INCLUDED
#define TYPEDOUBLEINT_INCLUDED

namespace mixedCells
{
  class DoubleInt{
  double rep;
  public:
    DoubleInt()
    {
      rep=0;
    }
    DoubleInt(double a)
    {
      rep=(double)a;
    }
    
    MY_INLINE static bool isField() 
    {
      return true; // this is true!
      //return false; // for experiment!!!
    }

    friend bool isZero(DoubleInt const &a)
    {
      return isZero(a.rep);
    }
    friend bool isZero2(DoubleInt const &a)
    {
      return isZero(a.rep);
    }
    friend bool isOne(DoubleInt const &a)
    {
      return isZero(a.rep-1);
    }
    friend DoubleInt operator/(DoubleInt const &a, DoubleInt const &b)
    {
      return DoubleInt(a.rep/b.rep);
    }
    friend DoubleInt operator-(DoubleInt const &a)
    {
      return DoubleInt(-a.rep);
    }
    friend DoubleInt operator-(DoubleInt const &a, DoubleInt const &b)
    {
      return DoubleInt(a.rep-b.rep);
    }
    friend DoubleInt operator+(DoubleInt const &a, DoubleInt const &b)
    {
      return DoubleInt(a.rep+b.rep);
    }
    friend class DoubleGen;
    friend class DoubleGen operator*(DoubleInt const &a, DoubleGen const &b);
    //    friend class DoubleGen operator/(DoubleInt const &a, DoubleGen const &b);//!!!!!!!!!!
    friend int volumeToInt(DoubleInt const &a)
    {
      return volumeToInt(a.rep);
    }
    friend DoubleInt operator*(DoubleInt const &s, DoubleInt const &t)
    {
      return DoubleInt(s.rep*t.rep);
    }
    friend DoubleInt gcd(DoubleInt const &s, DoubleInt const &t)
    {
      //assert(false);
      return gcd(round(s.rep),round(t.rep));
    }
    void operator+=(DoubleInt const &a)
    {
      rep+=a.rep;
    }
    void operator-=(DoubleInt const &a)
    {
      rep-=a.rep;
    }
    void operator/=(DoubleInt const &a)
    {
      // only exact divisions should be allowed (no round off).
      assert(round(rep)%round(a.rep)==0);
      rep/=a.rep;
    }
    void operator*=(DoubleInt const &a)
    {
      rep*=a.rep;
    }
    friend double toDoubleForPrinting(DoubleInt const &s)//change this to produce string
    {
      return s.rep;
    }
    friend std::ostream& operator<<(std::ostream& s, const DoubleInt &a)
    {
      s<<toDoubleForPrinting(a);
      return s;
    }
    /*    friend bool isGreaterEqual(DoubleInt const &a, DoubleInt const &b)
    {
      return greaterEqual(a,b);
      }*/
    friend bool isNegative(DoubleInt const &a)
    {
      return a.rep<-EPSILON;
    }
    friend bool isEpsilonLessThan(DoubleInt const &a, DoubleInt const &b)
    {
      return isEpsilonLessThan(a.rep,b.rep);
    }
  };
};
#endif
