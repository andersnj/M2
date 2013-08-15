#ifndef TYPERATGEN_H_INCLUDED
#define TYPERATGEN_H_INCLUDED

namespace mixedCells
{
  template <class Rational> class RatGen{
    static const int maxlength=3;
    int length;
    Rational v[maxlength];
  public:
    RatGen()
    {
      length=0;
    }
    /*ShortRatGen(double a)
    {
      rep=(double)a;
    }
    */

    RatGen(int a)
    {
      length=1;
      v[0]=a;
    }
    void random()
    {
      length=maxlength;
      for(int i=0; i<length; i++) v[i]=rand()%MAX_RANDOM_INT;
    }
    friend bool isZero(RatGen const &a)
    {
      for(int i=0;i<a.length;i++)if(!isZero(a.v[i]))return false;
      return true;
    }
    /*    friend bool isZero2(ShortRatGen const &a)
    {
      return isZero(a.rep);
      }*/
    /*    friend bool isOne(ShortRatGen const &a)
    {
      return isZero(a.rep-1);
      }*/

    /*    friend ShortRatGen operator/(ShortRatGen const &a, ShortRatGen const &b)
    {
      return ShortRatGen(a.rep/b.rep);
      }*/
    friend RatGen operator-(RatGen const &a)
    {
      RatGen ret;
      ret.length = a.length;
      for(int i=0;i<ret.length;i++) ret.v[i]=-a.v[i];
      return ret;
    }
    friend RatGen operator-(RatGen const &a, RatGen const &b)
    {
      RatGen ret;
      for(int i=0;i<maxlength;i++)ret.v[i]=a.v[i]-b.v[i];
      ret.length=MAX(a.length,b.length);
      return ret;
    }
    friend RatGen operator+(RatGen const &a, RatGen const &b)
    {
      RatGen ret;
      for(int i=0;i<maxlength;i++)ret.v[i]=a.v[i]+b.v[i];
      ret.length=MAX(a.length,b.length);
      return ret;
    }
    /*    friend class ShortRatGen operator*(DoubleInt const &a, ShortRatGen const &b)
    {
      return ShortRatGen(a.rep*b.rep);
      }*/
    friend class RatGen operator*(Rational const &a, RatGen const &b)
    {
      RatGen ret;
      for(int i=0;i<b.length;i++)ret.v[i]=a*b.v[i];
      ret.length=b.length;
      return ret;
    }
    /*
    friend class TrueGen operator*(ShortInt const &a, TrueGen const &b)
    {
      return TrueGen(a.rep*b.rep);
    }
    friend ShortRatGen operator*(ShortRatGen const &s, ShortRatGen const &t)
    {
      //      assert(0);//WHY IS THIS CALLED?
      return ShortRatGen(s.rep*t.rep);
      }*/
    /*    friend class ShortRatGen operator/(DoubleInt const &a, ShortRatGen const &b)
    {
      return ShortRatGen(a.rep/b.rep);
      }*/
    void operator+=(RatGen const &a)
    {
      if(length<a.length)length=a.length;
      for(int i=0;i<length;i++)v[i]+=a.v[i];
    }
    void operator-=(RatGen const &a)
    {
      if(length<a.length)length=a.length;
      for(int i=0;i<length;i++)v[i]-=a.v[i];
    }
    /*    void operator/=(DoubleInt const &a)
    {
      // only exact divisions should be allowed (no round off).
      assert(round(rep)%round(a.rep)==0);
      rep/=a.rep;
      }*/
    /*    void operator/=(ShortInt const &a)
    {
      // only exact divisions should be allowed (no round off).
      assert(round(rep)%a.rep==0);
      rep/=a.rep;
      }*/
    void operator/=(Rational const &a)
    {
      for(int i=0;i<length;i++)v[i]=v[i]/a;
    }
    friend bool isPositive(RatGen const &a)
    {
      for(int i=0;i<a.length;i++)
	{
	  if(isPositive(a.v[i]))return true;
	  if(isNegative(a.v[i]))return false;
	}
      return false;
    }
    friend bool isNegative(RatGen const &a)
    {
      for(int i=0;i<a.length;i++)
	{
	  if(isPositive(a.v[i]))return false;
	  if(isNegative(a.v[i]))return true;
	}
      return false;
    }
    friend double toDoubleForPrinting(RatGen const &s)//change this to produce string
    {
      double ret=0;
      double epsn=1;
      for(int i=0;i<s.length;i++)
	{
	  ret+=epsn*toDoubleForPrinting(s.v[i]);
	  epsn*=0.01;
	}
      return ret;
    }
    friend std::ostream& operator<<(std::ostream& s, const RatGen &a)
    {
      s<<toDoubleForPrinting(a);
      return s;
    }
    friend bool isGreaterEqual(RatGen const &a, RatGen const &b)
    {
      for(int i=0;i<maxlength;i++)
	{
	  if(a.v[i]<b.v[i])return false;
	  if(b.v[i]<a.v[i])return true;
	}
      return true;
    }
    friend bool operator<(RatGen const &a, RatGen const &b)
    {
      for(int i=0;i<maxlength;i++)
	{
	  if(a.v[i]<b.v[i])return true;
	  if(b.v[i]<a.v[i])return false;
	}
      return false;
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
    void assignGCD(Rational &dest)const
    {
      dest=Rational(0);
    }
  }; //end RatGen
};
#endif
