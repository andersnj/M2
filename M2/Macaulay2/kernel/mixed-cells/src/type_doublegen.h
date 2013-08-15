#ifndef TYPEDOUBLEGEN_H_INCLUDED
#define TYPEDOUBLEGEN_H_INCLUDED
namespace mixedCells
{
  class DoubleGen{
    double rep;
  public:
    DoubleGen()
    {
      rep=0;
    }
    DoubleGen(double a)
    {
      rep=(double)a;
    }
    
    void random()
    {
      rep=0;
      for(int i=0; i<4; i++) rep=(rand()&4095)+(rep/4096);
      rep/=4096;
    }
    friend bool isZero(DoubleGen const &a)
    {
      return isZero(a.rep);
    }
    friend bool isZero2(DoubleGen const &a)
    {
      return isZero(a.rep);
    }
    friend bool isOne(DoubleGen const &a)
    {
      return isZero(a.rep-1);
    }

    friend DoubleGen operator/(DoubleGen const &a, DoubleGen const &b)
    {
      return DoubleGen(a.rep/b.rep);
    }
    friend DoubleGen operator-(DoubleGen const &a)
    {
      return DoubleGen(-a.rep);
    }
    friend DoubleGen operator-(DoubleGen const &a, DoubleGen const &b)
    {
      return DoubleGen(a.rep-b.rep);
    }
    friend DoubleGen operator+(DoubleGen const &a, DoubleGen const &b)
    {
      return DoubleGen(a.rep+b.rep);
    }
    friend class DoubleGen operator*(DoubleInt const &a, DoubleGen const &b)
    {
      return DoubleGen(a.rep*b.rep);
    }
    /*    friend class DoubleGen operator*(ShortRat const &a, DoubleGen const &b)
    {
      return DoubleGen(a.c*b.rep/a.d);
      }*/
    friend class DoubleGen operator*(ShortInt const &a, DoubleGen const &b)
    {
      return DoubleGen(a.rep*b.rep);
    }
    /* friend class TrueGen operator*(ShortInt const &a, TrueGen const &b) */
    /* { */
    /*   return TrueGen(a.rep*b.rep); */
    /* } */
    friend DoubleGen operator*(DoubleGen const &s, DoubleGen const &t)
    {
      //      assert(0);//WHY IS THIS CALLED?
      return DoubleGen(s.rep*t.rep);
    }
    /*    friend class DoubleGen operator/(DoubleInt const &a, DoubleGen const &b)
    {
      return DoubleGen(a.rep/b.rep);
      }*/
    void operator+=(DoubleGen const &a)
    {
      rep+=a.rep;
    }
    void operator-=(DoubleGen const &a)
    {
      rep-=a.rep;
    }
    void operator/=(DoubleInt const &a)
    {
      // only exact divisions should be allowed (no round off).
      assert(round(rep)%round(a.rep)==0);
      rep/=a.rep;
    }
    void operator/=(ShortInt const &a)
    {
      // only exact divisions should be allowed (no round off).
      assert(round(rep)%a.rep==0);
      rep/=a.rep;
    }/*
    void operator/=(ShortRat const &a)
    {
      rep = rep * a.d / a.c;
      }*/
    friend bool isPositive(DoubleGen const &a)
    {
      return a.rep>EPSILON;
    }
    friend bool isNegative(DoubleGen const &a)
    {
      return a.rep<-EPSILON;
    }
    friend double toDoubleForPrinting(DoubleGen const &s)//change this to produce string
    {
      return s.rep;
    }
    friend std::ostream& operator<<(std::ostream& s, const DoubleGen &a)
    {
      s<<toDoubleForPrinting(a);
      return s;
    }
    friend bool isGreaterEqual(DoubleGen const &a, DoubleGen const &b)
    {
      return isGreaterEqual(a.rep,b.rep);
    }
    friend bool operator<(DoubleGen const &a, DoubleGen const &b)
    {
      return a.rep<b.rep;
    }

    /**
       This will return the largest integer by which *this is
       divisible. If all integers divide, then 0 is returned.
     */
    void assignGCD(DoubleInt &dest)const
    {
      dest=DoubleInt(0);
    }
    void assignGCD(ShortInt &dest)const
    {
      dest=ShortInt(0);
    }
    /*    void assignGCD(ShortRat &dest)const
    {
      dest=ShortRat(0);
      }*/
  }; // end DoubleGen
};
#endif
