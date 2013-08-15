#ifndef TYPESHORTINT_H_INCLUDED
#define TYPESHORTINT_H_INCLUDED

namespace mixedCells
{
  class ShortInt{
    int rep;
  public:
    ShortInt(int a)
    {
      rep=a;
    }
    
    MY_INLINE static bool isField() {return false;}

    friend bool isZero(ShortInt const &a)
    {
      return (a.rep==0);
    }
    friend bool isZero2(ShortInt const &a)
    {
      return (a.rep==0);
    }
    friend bool isOne(ShortInt const &a)
    {
      return isZero(a.rep-1);
    }
    friend ShortInt operator/(ShortInt const &a, ShortInt const &b)
    {
      assert(a.rep%b.rep==0);
      return ShortInt(a.rep/b.rep);
    }
    friend ShortInt operator-(ShortInt const &a)
    {
      return ShortInt(-a.rep);
    }
    friend ShortInt operator-(ShortInt const &a, ShortInt const &b)
    {
      return ShortInt(a.rep-b.rep);
    }
    friend ShortInt operator+(ShortInt const &a, ShortInt const &b)
    {
      return ShortInt(a.rep+b.rep);
    }
    friend class DoubleGen;
    friend class DoubleGen operator*(ShortInt const &a, DoubleGen const &b);
    // friend class TrueGen;
    // friend class TrueGen operator*(ShortInt const &a, TrueGen const &b);
    //    friend class TrueGen operator/(ShortInt const &a, TrueGen const &b);//!!!!!!!!!!
    friend int volumeToInt(ShortInt const &a)
    {
      return ABS(a.rep);
    }
    friend ShortInt operator*(ShortInt const &s, ShortInt const &t)
    {
      return ShortInt(s.rep*t.rep);
    }
    void operator+=(ShortInt const &a)
    {
      rep+=a.rep;
    }
    void operator-=(ShortInt const &a)
    {
      rep-=a.rep;
    }
    void operator/=(ShortInt const &a)
    {
      // only exact divisions should be allowed (no round off).
      assert(rep%a.rep==0);
      rep/=a.rep;
    }
    void operator*=(ShortInt const &a)
    {
      rep*=a.rep;
    }
    friend ShortInt gcd(ShortInt const &s, ShortInt const &t)
    {
      return gcd(s.rep,t.rep);
    }
    friend double toDoubleForPrinting(ShortInt const &s)//change this to produce string
    {
      return s.rep;
    }
    friend std::ostream& operator<<(std::ostream& s, const ShortInt &a)
    {
      s<<a.rep;
      return s;
    }
    /*    friend bool isGreaterEqual(ShortInt const &a, ShortInt const &b)
    {
      return greaterEqual(a,b);
      }*/
    friend bool isNegative(ShortInt const &a)
    {
      return a.rep<0;
    }
    friend bool isEpsilonLessThan(ShortInt const &a, ShortInt const &b)
    {
      return a.rep<b.rep;
    }
  };
};
#endif
