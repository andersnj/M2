#include "shortrationals.h"
#include "gmprationals.h"

#define SEED 1.0123456789
//#define DOUBLE_DOUBLE
namespace mixedCells
{

  // First working settings: (short machine integers as a base)
#ifdef SHORTRAT_SHORTRAT
  typedef ShortRat LType;
  typedef RatGen<LType> RType;
#endif
  //typedef RatGen<LType> RType;
  //typedef ShortInt LType;
#ifdef DOUBLE_DOUBLE
  typedef DoubleInt LType;
  typedef DoubleGen RType;
#endif
  //typedef double LType;
  //typedef double RType; // this can't be RType anymore?!!
  

  // Second working settings: (exact)
#ifdef GMPRAT_GMPRAT
  typedef GmpRational LType;
  typedef RatGen<LType> RType;
#endif

  
  void normalizeRowPair(Matrix<LType> &AL, int j, Vector<RType> &R
#if HASH
, Vector<LType> &H
#endif
)
  {
    Matrix<LType>::RowRef row = AL[j];
    LType g(0);
    R[j].assignGCD(g);
    for (int i=0; isOne(g) && i<AL.getWidth(); i++)
      g = gcd(g,row[i]);
    if (isZero(g)) return;
    for (int i=0; i<AL.getWidth(); i++)
      row[i] /= g;
    R[j] /= g;
#if HASH
    H[j] /= g;
#endif
  }

  void removeZeroRowsPair(Matrix<LType> &AL, Vector<RType> &AR)
{
  int n=0;
  for(int i=0;i<AL.getHeight();i++)
    {
      bool isZer=isZero(AR[i]);
      for(int j=0;j<AL.getWidth();j++)if(!isZero(AL[i][j]))isZer=false;
      if(!isZer)n++;
    }
  Matrix<LType> retL(n,AL.getWidth());
  Vector<RType> retR(n);
  n=0;
  for(int i=0;i<AL.getHeight();i++)
    {
      bool isZer=isZero(AR[i]);
      for(int j=0;j<AL.getWidth();j++)if(!isZero(AL[i][j]))isZer=false;
      if(!isZer){retR[n]=AR[i];retL[n++].set(AL[i].toVector());}
    }
  AL=retL;
  AR=retR;
}

int reducePair(Matrix<LType> &L, Vector<RType> &R, bool returnIfZeroDeterminant)
{
  //  if(width<=1)cerr<<height<<"x"<<width<<endl;
  int retSwaps=0;
  int currentRow=0;
  
  for(int i=0;i<L.getWidth();i++)
    {
      int s=L.findRowIndex(i,currentRow);
      
      if(s!=-1)
	{
	  if(s!=currentRow)
	    {
	      L.swapRows(currentRow,s);
	      {
		RType temp=R[s];
		R[s]=R[currentRow];
		R[currentRow]=temp;
	      }
	      retSwaps++;
	    }
	  if (LType::isField()) {
	    for(int j=currentRow+1;j<L.getHeight();j++)
	      {
		LType s = -L[j][i]/L[currentRow][i];
		L.multiplyAndAddRow(currentRow,s,j);
		R[j]+=s*R[currentRow];
	      }
	  } else {
	    for(int j=currentRow+1;j<L.getHeight();j++)
	      if (!isZero(L[j][i])) {
		//cerr << L[j].toVector() << endl;
		LType g = gcd(L[j][i],L[currentRow][i]);
		LType b = L[j][i]/g;
		LType a = L[currentRow][i]/g;
		L.replaceWithLinearCombination(j,a,currentRow,-b,j);
		R[j] = a*R[j]-b*R[currentRow];
		//cerr << "\n  a = " << a << ", b = " << b << " gcd = " << g << "\n " << (L[j][i]) << "\n";
	      }
	  };
	  currentRow++;
	}
      else
	if(returnIfZeroDeterminant)return -1;
    }

  return retSwaps;
}


  Vector<LType> normalForm(Vector<LType> vL, Matrix<LType> const &AL)//assume reduced
  {
    int pivotI=-1;
    int pivotJ=-1;
    int nonpivots=vL.size();
    while(AL.nextPivot(pivotI,pivotJ))
      {
	nonpivots--;
	LType s=vL[pivotJ]/AL[pivotI][pivotJ];
	vL-=s*AL[pivotI].toVector();
      }
    Vector<LType> vDestL=Vector<LType>(nonpivots);
    pivotI=-1;
    pivotJ=-1;
    int i=0;
    int last=-1;
    while(AL.nextPivot(pivotI,pivotJ))
      {
	while(pivotJ-1>last)
	  {
	    vDestL[i++]=vL[++last];
	  }
	last=pivotJ;
      }
    last++;
    while(last<AL.getWidth())
      vDestL[i++]=vL[last++];
    assert(i==nonpivots);
    return vDestL;
}


  void normalFormPair(Vector<LType> vL, RType vR, Vector<LType> &vDestL, RType &vDestR, Matrix<LType> const &AL, Vector<RType> const &AR)//assume reduced
{
  int pivotI=-1;
  int pivotJ=-1;
  int nonpivots=vL.size();
  while(AL.nextPivot(pivotI,pivotJ))
    {
      nonpivots--;
      //      v-=(v[pivotJ]/(*this)[pivotI][pivotJ])*(*this)[pivotI].toVector();
      LType s=vL[pivotJ]/AL[pivotI][pivotJ];
      vL-=s*AL[pivotI].toVector();
      vR-=s*AR[pivotI];
    }
  vDestL=Vector<LType>(nonpivots);
  vDestR=0;
  pivotI=-1;
  pivotJ=-1;
  int i=0;
  int last=-1;
  while(AL.nextPivot(pivotI,pivotJ))
    {
      while(pivotJ-1>last)
	{
	  vDestL[i++]=vL[++last];
	  //	    cerr<<"("<<(i-1)<<","<<last<<")";
	}
      last=pivotJ;
    }
  last++;
  while(last<AL.getWidth())
    vDestL[i++]=vL[last++];
  vDestR=vR;
  //if(debug)cerr<<v<<":"<<ret<<endl;
  assert(i==nonpivots);
  //  return ret;
}


void normalFormPairs(Matrix<LType> const &mL, Vector<RType> const &mR, Matrix<LType> &retL, Vector<RType> &retR, Matrix<LType> const &AL, Vector<RType> const &AR )//assume reduced
{
  Matrix<LType> tempretL=Matrix<LType>(mL.getHeight(),AL.getWidth()-AL.numberOfPivots());
  Vector<RType> tempretR=Vector<RType>(mL.getHeight());
  for(int i=0;i<mL.getHeight();i++)
    {
      Vector<LType> temp;
      normalFormPair(mL[i].toVector(),mR[i],temp , tempretR[i], AL, AR);
      tempretL[i].set(temp);
    }
  //    ret[i].set(normalForm(m[i].toVector()));
  //return ret;
  retL=tempretL;
  retR=tempretR;
}

}

std::ostream& operator<<(std::ostream& s, const IntegerVector &v)
{
  int minimalFieldWidth=2;
  s<<"(";
  for(int i=0;i<v.size();i++)
    {
      if(i!=0)s<<",";
      s<<v[i];
    }
  s<<")";
  return s;
}

std::ostream& operator<<(std::ostream& s, std::vector<int> const &v)
{
  s<<"(";
  for(int i=0;i<v.size();i++)
    {
      if(i!=0)s<<",";
      s<<v[i];
    }
  s<<")";
  return s;
}

namespace mixedCells
{
  class Statistics
  {
  public:
    int nLPs;
    int nRekCalls;
    int nCells;
    int nFeasible;
    int forFree;
    int nMTKNodes;
    int nLPRunNodes;
    Statistics():
      nLPs(0),
      nRekCalls(0),
      nCells(0),
      nFeasible(0),
      forFree(0),
      nMTKNodes(1),
      nLPRunNodes(0)
    {
    }
 
    friend std::ostream& operator<<(std::ostream& s, const Statistics &S)
    {
      s<<"Number of LPs solved:"<<S.nLPs<<endl;
      s<<"Number of recursive calls:"<<S.nRekCalls<<endl;
      s<<"Number of cells found:"<<S.nCells<<endl;
      s<<"Number of feasible LPs:"<<S.nFeasible<<endl;
      s<<"Number of combinatorially deduced infeasibilities:"<<S.forFree<<endl;
      s<<"Number of MizutaniTakedaKojima nodes:"<<S.nMTKNodes<<endl;
      s<<"Number of LP run nodes:"<<S.nLPRunNodes<<endl;
    }
  };
  Statistics statistics;


  template<class typL> char hash2(typL v)
    {
      return 	(((char*)(&v))[5]&255)+(((char*)(&v))[7]&128);
    }

  /**
     The Reducer class represents a d-dimensional linear subspace L of
     (typL)^n\times typR together with a description of how to compute
     the normal form of a vector modulo L with (n-d)+1 entries.

     One important feature of Reducer is that it allows pushing and
     popping generators for L efficiently - that is it allows to
     increase or decrease the dimension of L.

     The linear subspace is represented by a reduced matrix [mL|mR].
     In order not to change the complete matrix when pushing or
     popping a generator the ordering of the coordinates with respect
     to which this matrix is reduced is changed dynamically. For this
     to work the pivotIndices and isPivot vectors are needed.

     It is not possible to push a vector which forces the last column
     to have a pivot (the push call will "fail"), see push(). The last
     column will never have a pivot.
   */  
  template<class typL, class typR> class Reducer
{
  struct RREMatrix
  {//Struct for holding compact representation of the the RRE form
    int originalLeftDim;
    int npivots;
    Matrix<typL> mL;
    Vector<typR> mR;
#if HASH
    Vector<typL> mH;
#endif
    Vector<int> nonPivotIndices;
    Vector<int> pivotIndices;
    //Allocating only
  RREMatrix(int n, int d):
    originalLeftDim(n),
      npivots(d),
      mL(d,n-d),
      mR(d),
#if HASH
      mH(d),
#endif
      nonPivotIndices(n-d),
      pivotIndices(d)
      {
      }
    friend std::ostream& operator<<(std::ostream& s, RREMatrix const &m)
    {
      s<<"RREMatrix:"<<endl;
      s<<"mL"<<endl<<m.mL;
      s<<"mR"<<endl<<m.mR;
      s<<"mH"<<endl<<m.mH;
      s<<"nonPivotIndices"<<endl<<m.nonPivotIndices<<endl;
      s<<"pivotIndices"<<endl<<m.pivotIndices<<endl;
      return s;
    }
  };

  vector<RREMatrix> RREMatrices;

  /**
     This is the left hand side of the matrix representing the
     subspace L. Notice that only the first d rows are used.
   */
  Matrix<typL> mL;
  /**
     This column vector is the right-most column of the matrix
     representing the subspace L. Notice that only the first d entries
     are used.
   */
  Vector<typR> mR;
#if HASH
  Vector<typL> mH;
#endif
  Vector<typL> tempL;
  typR tempR;
  Vector<typL> temp2L;
  typR temp2R;
#if HASH
  typL temp2H;
#endif
  /**
     The ith entry of this vector contains the index of the pivot in
     the ith row. This vector has length n at initialization but only
     the first d entries are meaningful.
   */
  IntegerVector pivotIndices;
  /**
     The ith entry of this matrix tells whether the ith column of
     [mL] (restricted to the first d rows) contains a pivot.
   */
  IntegerVector 
    //vector<char> 
isPivot;
  /**
     The dimension of the ambient space of L is n+1.
   */
  int n;
  /**
     The dimension of the subspace L.
   */
  int d;

  //Temporaries
  //vector<Vector<typL> > xVectors;
  vector</*bool*/char> temp;//bools are stored as bits, which makes them slow to look up
 public:
  Reducer(int n_):
    n(n_),
    d(0),
    mL(n_,n_),
    mR(n_,1),
#if HASH
      mH(n_,1),
#endif
    pivotIndices(n_),
    isPivot(n_),
    tempL(n_),
      temp2L(n_)
  {
    //    for(int i=0;i<n+1;i++)xVectors.push_back(Vector<typL>(i));
    for(int i=0;i<=n;i++)RREMatrices.push_back(RREMatrix(n,i));
    for(int i=0;i<n;i++)RREMatrices[0].nonPivotIndices[i]=i;
    for(int i=0;i<n;i++)isPivot[i]=false;
  }
  /**
     Add the line generated by (vL,vR) to the subspace. Notice that if
     this makes the standard basis vector e_{n+1} go into the L, then
     the push is not performed and false is returned. Also if the line
     was already contained in L no push is performed and the return
     value is false. In the first case we get an inconsistency for our
     LPs since the last coordinate of an element in the kernel is
     forced to be zero. The latter case never happens in our
     application since the dimension is supposed to drop by one every
     time because of the generic lifting.?????? Is this really true?
   */
__attribute__ ((noinline))
  bool pushBLA(Vector<typL> const &vL, typR const &vR
#if HASH
, typL const &vH
#endif
)
  {
    //    cerr<<"PUSHING"<<vL<<endl;
    mL[d].set(vL);
#if HASH
    mH[d]=vH;
#endif
    mR[d]=vR;
    for(int i=0;i<d;i++)
      {
	mR[d]+=-mL[d][pivotIndices[i]]*mR[i];
#if HASH
	mH[d]+=-mL[d][pivotIndices[i]]*mH[i];
#endif
	mL.multiplyAndAddRow(i,-mL[d][pivotIndices[i]],d);
      }
    int pivot=-1;
    for(int j=0;j<n;j++)if(!isZero(mL[d][j])){pivot=j;break;}
    if(pivot==-1) 
      {
	if(isZero(mR[d])) { // mR[d] == 0 means non-generic
	  assert(false);
	  /** implement measures treating the non-generic case
	   One strategy: 
           -- (relation=equation)
           -- (generate heights with full precision; do not store)
	   -- build relations and inequalities (contained in cones) 
	      so that they have full-precision RHS
	   -- relations should store the list (more like a pair) of _involved heights_
	   -- reducers (mL,mR) should remember a reference to the original relation 
	   -- Reducer stores precision for _every_ height
	   -- pop() should restore the precision of every height 
	      to the original precision of the parent node. (How?)
	   */
	}
	return false;
      }
    if (typL::isField()) {
      typL mult = 1/mL[d][pivot];
      mR[d] = mult*mR[d];
#if HASH
      mH[d] = mult*mH[d];
#endif
      mL.scaleRow(d,mult);
    } else {
      normalizeRowPair(mL,d,mR
#if HASH
		       ,mH
#endif
);
    }
    pivotIndices[d]=pivot;
    isPivot[pivot]=true;

    if(1)    {
      //      cerr<<"PIVVVOOOT"<<pivot<<"D"<<d<<"N"<<n<<endl;
      // Remove one entry from nonpivot list at next level
      int i=0;
      for(;i<n-d;i++)
	if(RREMatrices[d].nonPivotIndices[i]!=pivot)
	  {
	    RREMatrices[d+1].nonPivotIndices[i]=RREMatrices[d].nonPivotIndices[i];
	  }
	else
	  break;

      int pivotIndexInNew=i;
      i++;
      for(;i<n-d;i++)
	{
	  RREMatrices[d+1].nonPivotIndices[i-1]=RREMatrices[d].nonPivotIndices[i];
	}
      // Add one entry to pivot entries at next level
      for(i=0;i<d;i++)
	RREMatrices[d+1].pivotIndices[i]=RREMatrices[d].pivotIndices[i];
      RREMatrices[d+1].pivotIndices[i]=pivot;


      // Reduce new equation
      //      Vector<typL> vLTemp=vL;
      tempL=vL;
      typR vRTemp=vR;
#if HASH
      typL vHTemp=vH;
#endif

      for(i=0;i<d;i++)
	{
	  for(int j=0;j<n-d;j++)
	    tempL[RREMatrices[d].nonPivotIndices[j]]-=tempL[RREMatrices[d].pivotIndices[i]]*RREMatrices[d].mL[i][j];
	  vRTemp-=tempL[RREMatrices[d].pivotIndices[i]]*RREMatrices[d].mR[i];
#if HASH
	  vHTemp-=tempL[RREMatrices[d].pivotIndices[i]]*RREMatrices[d].mH[i];
#endif
	  tempL[RREMatrices[d].pivotIndices[i]]=0;
	}

      //cerr<<"before"<<vLTemp<<endl;
      // Scale new equation pivot to 1
      typL inv=1/tempL[pivot];
      for(int i=0;i<tempL.size();i++)tempL[i]*=inv;
      vRTemp=inv*vRTemp;
#if HASH
      vHTemp=inv*vHTemp;
#endif
      //cerr<<"after"<<vLTemp<<endl;

      //cerr<<"after2"<<vLTemp<<endl;

      // Reduce previous rows with respect to new equation
      for(i=0;i<d;i++)
	{
	  //cerr<<"scaling:"<<RREMatrices[d].mL[i][pivotIndexInNew];
	  for(int j=0;j<n-d-1;j++)
	    RREMatrices[d+1].mL[i][j]=RREMatrices[d].mL[i][j+(j>=pivotIndexInNew)]-tempL[RREMatrices[d+1].nonPivotIndices[j]]*RREMatrices[d].mL[i][pivotIndexInNew];
	  RREMatrices[d+1].mR[i]=RREMatrices[d].mR[i]-RREMatrices[d].mL[i][pivotIndexInNew]*vRTemp;//!!
#if HASH
	  RREMatrices[d+1].mH[i]=RREMatrices[d].mH[i]-RREMatrices[d].mL[i][pivotIndexInNew]*vHTemp;//!!
#endif
	}

      // Include new equation
      for(int j=0;j<n-d-1;j++)
	RREMatrices[d+1].mL[d][j]=tempL[RREMatrices[d+1].nonPivotIndices[j]];
      RREMatrices[d+1].mR[d]=vRTemp;
#if HASH
      RREMatrices[d+1].mH[d]=vHTemp;
#endif
    }

    d++;
    //cerr<<*this;
    //cerr<<RREMatrices[d];
      //exit(0);
    return true;
  }
  /**
     Pop the latest added generator to L.
   */
  void pop()
  {
    d--;
    isPivot[pivotIndices[d]]=false;
  }
  friend std::ostream& operator<<(std::ostream& s, Reducer const &r)
  {
    s<<"Reducer:"<<endl;
    s<<r.mL.submatrix(0,0,r.d,r.mL.getWidth());
    s<<r.mR.subvector(0,r.d);
#if HASH
    s<<r.mH.subvector(0,r.d);
#endif
    return s;
  }
  /**
     This mehtod transforms the inequalities (rows of the matrix
     [originalL|originalR]) to their normal forms modulo L by reducing
     with the matrix representation of L. Only coordinates with no
     pivots have to be stored. That is, n-d for the left handside, and
     1 for the right hand side. In total d coordinates are removed.

     The output is stored in [LeftHandSide|RightHandSide], and these
     two matrices (/vector) must have been initialiazed to the right
     width, and with a sufficient number of rows before calling.

     If the normal form of an inequality turns out to be zero, then it
     is not stored. The total number of stored inequalities is the
     return value of the method.
   */
  /*  int storeAndSplitNormalForms(Matrix<typL> const &originalL, Vector<typR> const &originalR, Matrix<typL> &LeftHandSide, Vector<typR> &RightHandSide)
  {//IS THIS CODE DEAD?
    int ret=0;
    assert(originalL.getWidth()==n);
    for(int i=0;i<originalL.getHeight();i++)
      {
	typR tempR=originalR[i];
	for(int j=0;j<n;j++)
	  {
	    tempL[j]=originalL[i][j];
	  }
	for(int k=0;k<d;k++)
	  {
	    tempR+=-tempL[pivotIndices[k]]*mR[k];
	    mL.maddRowToVector(k,-tempL[pivotIndices[k]],tempL);
	  }
	if(!(tempL.isZero()&&isZero(tempR)))
	  {
	    RightHandSide[ret]=-tempR;
	    int J=0;
	    for(int j=0;j<n;j++)if(!isPivot[j])LeftHandSide[ret][J++]=tempL[j];
	    ret++;
	  }
      }
    return ret;
    }*/
  /**
     This mehtod transforms the inequalities (rows of the matrix
     [sourceL|sourceR]) to their normal forms modulo L by reducing
     with the matrix representation of L. Only coordinates with no pivots have
     to be stored. Therefore, the input has n+1 coordinates for the
     left handside, and 1 for the right hand side. The output has only
     n-d coordinates for the left handside.

     The output is stored in [destinationL|destinationR], but
     starting at row destinationOffset. The two matrices (/vector)
     must have been initialiazed to the right width, and with a
     sufficient number of rows before calling.

     If the normal form of an inequality turns out to be zero, then it
     is not stored. The total number of stored inequalities is the
     return value of the method unless if one of the inequalities is
     inconsistent. In this case a -1 is returned. FIX DOCUMENTATION
   */
__attribute__ ((noinline))
  int reduction(Matrix<typL> const &sourceL, Vector<typR> const sourceR,
#if HASH
		Vector<typL> const sourceH,
#endif
 Matrix<typL> &destinationL, Vector<typR> &destinationR,
#if HASH
		Vector<typL> &destinationH,
#endif
 int destinationOffset)
  {
    /*    cerr<<"-----------------------"<<endl;
    cerr<<*this;
    cerr<<"Source"<<sourceL<<sourceR;
    cerr<<"Destination"<<destinationL<<destinationR;
    cerr<<"Destination offset"<<destinationOffset<<endl;
    */
    int ret=0;
    assert(sourceL.getWidth()==n);
    //    assert(tempL.size()==n);//??

#if 1
    for(int i=0;i<sourceL.getHeight();i++)
      {
	Matrix<LType>::RowRef dest=destinationL[destinationOffset+ret];
	bool leftHandSideZero=true;
	for(int j=0;j<n-d;j++)
	  {
	    typL t=sourceL[i][RREMatrices[d].nonPivotIndices[j]];
	    for(int k=0;k<d;k++)
	      t-=(sourceL[i][RREMatrices[d].pivotIndices[k]])*(RREMatrices[d].mL[k][j]);
	    
	    dest[j]=t;
	    if(!isZero(t)){leftHandSideZero=false;}
	  }
	typR tR=sourceR[i];
	for(int k=0;k<d;k++)
	  tR-=(sourceL[i][RREMatrices[d].pivotIndices[k]])*(RREMatrices[d].mR[k]);
	destinationR[destinationOffset+ret]=tR;
#if HASH
	typL tH=sourceH[i];
	for(int k=0;k<d;k++)
	  tH-=(sourceL[i][RREMatrices[d].pivotIndices[k]])*(RREMatrices[d].mH[k]);
	destinationH[destinationOffset+ret]=tH; //Why not use the hash value to check if left hand side is zero?
#endif	

	if(leftHandSideZero)
	  {
	    if(isNegative(tR))
	      return -1;//infeasible
	  }
	else
	  ret++;
      }
#else
include Hash data in this part too
    for(int i=0;i<sourceL.getHeight();i++)
      {
	typR tempR=sourceR[i];
	for(int j=0;j<n;j++)tempL[j]=sourceL[i][j];
	//cerr<<"temp"<<temp;
	//cerr<<mL;
	for(int k=0;k<d;k++)
	  {
	    tempR+=(-(tempL[pivotIndices[k]]))*mR[k];
	    mL.maddRowToVector(k,-(tempL[pivotIndices[k]]),tempL);
	  }
		
	bool leftHandSideZero=true;
	for(int j=0;j<n;j++)if(!isZero(tempL[j])){leftHandSideZero=false;break;}
	if(leftHandSideZero)
	  {
	    if(isNegative(tempR))return -1;//infeasible
	  }
	else
	  {
	    int J=0;
	    Matrix<LType>::RowRef dest=destinationL[destinationOffset+ret];
	    for(int j=0;j<n;j++)if(!isPivot[j])dest[J++]=tempL[j];
	    destinationR[destinationOffset+ret]=tempR;
	    ret++;
	  }
      }
#endif
    //cerr<<"Destination"<<destinationL<<destinationR;
    return ret;
  }
  //#define HASH 1
#if HASH
  //  static unsigned char hashTable[256];
 mutable unsigned char hashTable[256];
#endif
__attribute__ ((noinline))
  int singleSingleReduction(Matrix<typL> const &sourceL, Vector<typR> const &sourceR,
#if HASH
			   Vector<typL> const &sourceH,
#endif
			    Matrix<typL> &destinationL, Vector<typR> &destinationR,
#if HASH
			   Vector<typL> &destinationH,
#endif
			    const int newPivotIndex, int ret, const int i)
 {
    typL scalar=sourceL[i][newPivotIndex];

    if(1) // change to optimize code below
      {
	for(int j=0;j<newPivotIndex;j++)
	  destinationL[ret][j]=sourceL[i][j]-scalar*temp2L[j];
	for(int j=newPivotIndex;j<n-d;j++)
	  destinationL[ret][j]=sourceL[i][j+1]-scalar*temp2L[j];
	destinationR[ret]=sourceR[i]-scalar*temp2R;
#if HASH
	destinationH[ret]=sourceH[i]-scalar*temp2H;
#endif
      }
    else
      {
	for(int j=0;j<n-d;j++)
	  destinationL[ret][j]=sourceL[i][j+(j>=newPivotIndex)]-scalar*temp2L[j];
	destinationR[ret]=sourceR[i]-scalar*temp2R;
      }

#if 0
    else /* Here are various alternatives */
      {
	MatrixDouble::RowRef dest=destination[ret];
	MatrixDouble::const_RowRef src=source[i];

	typ * __restrict destB=&(dest[0]);
	const typ *srcB=&(src[0]);
	typ *temp2B=&(temp2[0]);

	/*for(signed long j=0;j<newPivotIndex;j++)
	  destB[j]=srcB[j]-scalar* temp2B[j];
	srcB++;

	destB+=(n-d+1);
	srcB+=(n-d+1);
	temp2B+=(n-d+1);

	for(signed long j=-(n-d+1)+newPivotIndex;j<0;j++)
	  destB[j]=srcB[j]-scalar* temp2B[j];
*/	
	/*		destB+=newPivotIndex;
	srcB+=newPivotIndex;
	temp2B+=newPivotIndex;
	for(signed long j=-newPivotIndex;j<0;j++)
	  destB[j]=srcB[j]-scalar* temp2B[j];
	srcB++;

	destB+=(n-d+1)-newPivotIndex;
	srcB+=(n-d+1)-newPivotIndex;
	temp2B+=(n-d+1)-newPivotIndex;

	for(signed long j=-(n-d+1)+newPivotIndex;j<0;j++)
	  destB[j]=srcB[j]-scalar* temp2B[j];
	*/
	/*for(int j=0;j<newPivotIndex;j++)
	  destB[j]=srcB[j]-scalar* temp2B[j];
	srcB++;

	for(int j=newPivotIndex;j<n-d+1;j++)
	  destB[j]=srcB[j]-scalar* temp2B[j];
	*/
	for(int j=newPivotIndex;j>0;j--)
	  *(destB++)=*(srcB++)-scalar* *(temp2B++);
	srcB++;
	for(int j=n-d+1-newPivotIndex;j>0;j--)
	*(destB++)=*(srcB++)-scalar* *(temp2B++);
	/*	for(int j=0;j<newPivotIndex;j++)
	  destB[j]=srcB[j]-scalar*temp2B[j];
	for(int j=newPivotIndex;j<n-d+1;j++)
	  destB[j]=srcB[j+1]-scalar*temp2B[j];	
	*/
      }
#endif
    //	bool leftHandSideZero=true;
    //	for(int j=0;j<n-d;j++)if(!m.isZero(destination[ret][j])){leftHandSideZero=false;break;}
    assert(destinationL.getWidth()==n-d);
    bool leftHandSideZero=destinationL[ret].isZero();
    if(leftHandSideZero)
	  {
	    if(isNegative(destinationR[ret]))return -1;//infeasible
	  }
	else
	  {
	    if(0)
	      {//this does not work because also Ainv needs to be scaled??
	      //	      cerr<<ret;
	      //cerr<<destinationL;
	      //scale row
	      int j=0;
	      for(;j<destinationL.getWidth();j++)if(!isZero2(destinationL.data[destinationL.getWidth()*ret+j]))break;
	      typL multiplier=1/destinationL.data[destinationL.getWidth()*ret+j];
	      if(isNegative(multiplier))multiplier=-multiplier;
	      destinationL.scaleRow(ret,multiplier);
	      destinationR[ret]=multiplier*destinationR[ret];
	      //	      cerr<<destinationL;
	    }

#if HASH
	    assert(destinationL.getWidth()==n-d);
	    //unsigned char h=destinationL.hashValue(ret,n-d);

	    unsigned char h=hash2(destinationH[ret]);

	    //cerr<<(int)h<<endl;
	    if((hashTable[h]!=255)&&destinationL.rowsAreEqual(ret,hashTable[h],n-d))
	      {
		if(destinationR[ret]<destinationR[hashTable[h]])destinationR[hashTable[h]]=destinationR[ret];
		ret--;
	      }
	    else
	      hashTable[h]=ret;
#endif
	    ret++;
	  }
	return ret; 
}

//__attribute__ ((inline))
 int inner(int N,int ret, Matrix<typL> const &sourceL, Vector<typR> const &sourceR,
#if HASH
	    Vector<typL> const &sourceH,
#endif
	    int numberOfUsedRowsInSource, Matrix<typL> &destinationL, Vector<typR> &destinationR,
#if HASH
	    Vector<typL> &destinationH,
#endif
	    int retmul, int imul)
 {
   for(int i=0;i<numberOfUsedRowsInSource;i++,imul+=sourceL.getWidth())
     if(__builtin_expect(!temp[i],true))
       {
	 //	 typL sum=0;
	 typL scalar=sourceL.data[imul];
	 for(int j=0;j<N;j++)
	   /*sum+=*/destinationL.data[retmul+j]=sourceL.data[imul+j+1]-scalar*temp2L[j];
	 destinationR[ret]=sourceR[i]-scalar*temp2R;
#if HASH
	 destinationH[ret]=sourceH[i]-scalar*temp2H;
#endif


	 bool leftHandSideZero=/*isZero(sum)&&*/destinationL[ret].isZero();
	 if(leftHandSideZero)
	   {if(isNegative(destinationR[ret]))return -1;}//infeasible
	 else
	   {
#if HASH
	 unsigned char h=hash2(destinationH[ret]);
	 if((hashTable[h]!=255)&&destinationL.rowsAreEqual(ret,hashTable[h],N))
	   {
	     if(destinationR[ret]<destinationR[hashTable[h]])destinationR[hashTable[h]]=destinationR[ret];
	     ret--;
	     retmul-=destinationL.getWidth();
	   }
	 else
	   hashTable[h]=ret;
#endif	 



	     ret++;retmul+=destinationL.getWidth();


	   }
       }
   return ret;
 }
__attribute__ ((noinline))
 int theLoop(int ret, int newPivotIndex, Matrix<typL> const &sourceL, Vector<typR> const &sourceR,
#if HASH
	     Vector<typL> const &sourceH,
#endif
 int numberOfUsedRowsInSource, Matrix<typL> &destinationL, Vector<typR> &destinationR
#if HASH
	     , Vector<typL> &destinationH
#endif
)
 {
   int retmul=ret*destinationL.getWidth();
   int imul=0;

   if(__builtin_expect(newPivotIndex==0,true))
     {
       int N=n-d;
       switch(N)
	 {
	   /*	 case 0:
	   return inner(0,ret,sourceL,sourceR,numberOfUsedRowsInSource, destinationL, destinationR,retmul,imul);
	   break;*/
	 case 1:
	   return inner(1,ret,sourceL,sourceR,
#if HASH
			sourceH,
#endif
			numberOfUsedRowsInSource, destinationL, destinationR,
#if HASH
			destinationH,
#endif
			retmul,imul);
	   break;
	 case 2:
	   return inner(2,ret,sourceL,sourceR,
#if HASH
			sourceH,
#endif
			numberOfUsedRowsInSource, destinationL, destinationR,
#if HASH
			destinationH,
#endif
			retmul,imul);
	   break;
	 case 3:
	   return inner(3,ret,sourceL,sourceR,
#if HASH
			sourceH,
#endif
			numberOfUsedRowsInSource, destinationL, destinationR,
#if HASH
			destinationH,
#endif
			retmul,imul);
	   break;
	 case 4:
	   return inner(4,ret,sourceL,sourceR,
#if HASH
			sourceH,
#endif
			numberOfUsedRowsInSource, destinationL, destinationR,
#if HASH
			destinationH,
#endif
			retmul,imul);
	   break;
	 default:
	   return inner(N,ret,sourceL,sourceR,
#if HASH
			sourceH,
#endif
			numberOfUsedRowsInSource, destinationL, destinationR,
#if HASH
			destinationH,
#endif
			retmul,imul);
	   break;
	 }
     }
   else
     {
       for(int i=0;i<numberOfUsedRowsInSource;i++,imul+=sourceL.getWidth())
	 if(__builtin_expect(!temp[i],true))
	   {
	     typL scalar=sourceL.data[imul+newPivotIndex];
	     //	  for(int j=0;j<n-d;j++)
	     //  destinationL.data[retmul+j]=sourceL.data[imul+j+(j>=newPivotIndex)]-scalar*temp2L[j];
	     for(int j=0;j<newPivotIndex;j++)
	       destinationL.data[retmul+j]=sourceL.data[imul+j]-scalar*temp2L[j];
	     for(int j=newPivotIndex;j<n-d;j++)
	       destinationL.data[retmul+j]=sourceL.data[imul+j+1]-scalar*temp2L[j];
	     destinationR[ret]=sourceR[i]-scalar*temp2R;
#if HASH
	     destinationH[ret]=sourceH[i]-scalar*temp2H;
#endif
	     
	     bool leftHandSideZero=destinationL[ret].isZero();
	     if(leftHandSideZero)
	       {if(isNegative(destinationR[ret]))return -1;}//infeasible
	     else
	       {
		 ret++;retmul+=destinationL.getWidth();
	       }
	   }
     }

    return ret;
 }
/*__attribute__ ((noinline))
 int theLoop(int ret, int newPivotIndex, Matrix<typL> const &sourceL, Vector<typR> const &sourceR, int numberOfUsedRowsInSource, Matrix<typL> &destinationL, Vector<typR> &destinationR)
 {
   int retmul=ret*destinationL.getWidth();
   int imul=0;
   //   cerr<<newPivotIndex<<" "<<numberOfUsedRowsInSource<<" "<<n-d<<endl;
   for(int i=0;i<numberOfUsedRowsInSource;i++,imul+=sourceL.getWidth())
      if(__builtin_expect(!temp[i],true))
	{
	  typL scalar=sourceL.data[imul+newPivotIndex];
	  //	  for(int j=0;j<n-d;j++)
	  //  destinationL.data[retmul+j]=sourceL.data[imul+j+(j>=newPivotIndex)]-scalar*temp2L[j];
	  	  for(int j=0;j<newPivotIndex;j++)
	    destinationL.data[retmul+j]=sourceL.data[imul+j]-scalar*temp2L[j];
	  for(int j=newPivotIndex;j<n-d;j++)
	  destinationL.data[retmul+j]=sourceL.data[imul+j+1]-scalar*temp2L[j];
	  destinationR[ret]=sourceR[i]-scalar*temp2R;
	  
	  bool leftHandSideZero=destinationL[ret].isZero();
	  if(leftHandSideZero&&isNegative(destinationR[ret]))return -1;//infeasible
	  ret++;retmul+=destinationL.getWidth();
	}
    return ret;
 }
*/
  /**
     This mehtod transforms the inequalities (rows of the matrix
     [sourceL|sourceR]) to their normal forms modulo L by reducing
     with the matrix representation of L, under the assumption that
     the equations were already reduced by the first d-1 rows of the
     matrix representation of L. Only coordinates with no pivots have
     to be stored. Therefore, the input has n-d+1 coordinates for the
     left handside, and 1 for the right hand side. The output has
     only n-d coordinates for the left handside.

     The output is stored in [destinationL|destinationR], but
     starting at row destinationOffset. The two matrices (/vector)
     must have been initialiazed to the right width, and with a
     sufficient number of rows before calling.

     If the normal form of an inequality turns out to be zero, then it
     is not stored. The total number of stored inequalities is the
     return value of the method unless if one of the inequalities is
     inconsistent. In this case a -1 is returned. FIX DOCUMENTATION
   */
 int singleReductionMakeBasisFirst(Matrix<typL> const &sourceL, Vector<typR> const &sourceR,
#if HASH
				   Vector<typL> const &sourceH,
#endif
 int numberOfUsedRowsInSource, Matrix<typL> &destinationL, Vector<typR> &destinationR,
#if HASH
				   Vector<typL> &destinationH,
#endif
				   vector<int> const &oldBasis, Matrix<typL> const &oldAinv, Matrix<typL> &newAinv)
 {
    /*    newAinv=Matrix<typL>(n-1,n-1);
    int K=0;
    for(int k=0;k<n;k++)
      if(k!=..)
	{
	  int I=0;
	  for(int i=0;i<n;i++)
	    if(i!=a)
	      {
		newAinv[K][I]=Ainv[k][i]-x[i]/x[a]*Ainv[k][a];
		I++;
	      }
	  K++;
	}
    */

    assert(d>0);
    assert(destinationL.getWidth()==n-d);
    assert(sourceL.getWidth()==n-d+1);

#if HASH
    memset(hashTable,255,256);
#endif

    if(pivotIndices[d-1]==n)//if we have a pivot on the right hand side then the system is already infeassible and there is no reason to add????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
      {
	return 0;
      }

    int numberOfPivotsBeforeCurrent=0;
    for(int j=0;j<pivotIndices[d-1];j++)if(isPivot[j])numberOfPivotsBeforeCurrent++;
    int J=0;
    for(int j=0;j<n;j++)if((!isPivot[j])&(j!=pivotIndices[d-1])){temp2L[J++]=mL[d-1][j];}
    temp2R=mR[d-1];
#if HASH
    temp2H=mH[d-1];
#endif
    int newPivotIndex=pivotIndices[d-1]-numberOfPivotsBeforeCurrent;


//puts basis first
    int nn=oldBasis.size();
    //Vector<typL> &x=xVectors[nn];
    Vector<typL> x(nn);
    //assert(x.size()==nn);
    #if 1
    {
      Vector<typL> temp3(nn);
      int J=0;
      isPivot[pivotIndices[d-1]]=false;
      for(int j=0;j<n;j++)if((!isPivot[j])/*||(j==pivotIndices[d-1])*/){temp3[J++]=mL[d-1][j];}
      isPivot[pivotIndices[d-1]]=true;
      //We now reduce 
      for(int i=0;i<nn;i++)x[i]=oldAinv.vectorDotJthColumn(temp3,i);
    }
    #else
    {
      isPivot[pivotIndices[d-1]]=false;
      int J=0;
      for(int j=0;j<n;j++)
	if((!isPivot[j])){oldAinv.multiplyAndAddRow(J++,mL[d-1][j],x);}
      isPivot[pivotIndices[d-1]]=true;
    }
    #endif
      int a=0;
    while(a<x.size())
      {
	if(!isZero(x[a]))break;
	a++;
      }
    assert(a!=x.size());

    //new basis will not contain a

    //compute new Ainv:
    {
      //      cerr<<oldAinv;
      assert(newAinv.getHeight()==nn-1);
      assert(newAinv.getWidth()==nn-1);
      //, Matrix<typL> &newAinv      
      //      Matrix<typL> newAinv(nn-1,nn-1);
      int K=0;
      for(int k=0;k<nn;k++)
	if(k!=newPivotIndex)
	  {
	    int I=0;
	    for(int i=0;i<nn;i++)
	      if(i!=a)
		{
		  newAinv[K][I]=oldAinv[k][i]-x[i]/x[a]*oldAinv[k][a];
		  I++;
		}
	    K++;
	  }
      //      cerr<<"NewAinv"<<newAinv;
    }


    //    cerr<<"OLDBASIS"<<oldBasis<<endl;
    //    cerr<<"OLDBASIS";
    //    cerr<<n;
    //for(int i=0;i<oldBasis.size();i++)cerr<<","<<oldBasis[i];cerr<<endl;
    int ret=0;

    if(numberOfUsedRowsInSource>temp.size()){temp=vector<char/*bool*/>(numberOfUsedRowsInSource);
      for(int i=0;i<numberOfUsedRowsInSource;i++)temp[i]=false;}
    for(int i=0;i<nn;i++)
      if(i!=a)
	{
	  ret=singleSingleReduction(sourceL,sourceR,
#if HASH
				    sourceH,
#endif
				    destinationL,destinationR,
#if HASH
				    destinationH,
#endif
				    newPivotIndex,ret,oldBasis[i]);
	  temp[oldBasis[i]]=true;
	  if(ret<0)goto returnminusone;//throw infeasibility result to parent
	}
#if HASH
    for(int i=0;i<numberOfUsedRowsInSource;i++)
      if(!temp[i])
      {
	ret=singleSingleReduction(sourceL,sourceR,
#if HASH
				  sourceH,
#endif
				  destinationL,destinationR,
#if HASH
				  destinationH,
#endif
				  newPivotIndex,
				  ret,i);
	if(ret<0)goto returnminusone;//throw infeasibility result to parent
      }
#else //this code only work with hash disabled
    ret=theLoop(ret, newPivotIndex,sourceL,sourceR,numberOfUsedRowsInSource,destinationL,destinationR);
#endif

    //for(int i=0;i<n;i++)
      //      if(i!=a)
    //cerr<<"Destination"<<destination;
    for(int i=0;i<nn;i++)
      temp[oldBasis[i]]=false;
    return ret;
 returnminusone:
    for(int i=0;i<nn;i++)
      temp[oldBasis[i]]=false;
    return -1;
  }
 int singleReduction(Matrix<typL> const &sourceL, Vector<typR> const &sourceR,
#if HASH
		     Vector<typL> const &sourceH,
#endif
		     int numberOfUsedRowsInSource, Matrix<typL> &destinationL,		     Vector<typR> &destinationR
#if HASH
		     ,Vector<typL> const &destinationH
#endif
)
  {
    /*   cerr<<"------------++++++++++-----------"<<endl;
    cerr<<*this;
    cerr<<"Source"<<source;
    cerr<<"Destination"<<destination;
    cerr<<"NumberOfUsedRowsInSource"<<numberOfUsedRowsInSource<<endl;
    */
    //    cerr<<"n"<<n<<"d"<<d<<"dw"<<destination.getWidth()<<"sw"<<source.getWidth();
    assert(d>0);
    assert(destinationL.getWidth()==n-d);
    assert(sourceL.getWidth()==n-d+1);

#if HASH
    memset(hashTable,255,256);
#endif

    if(pivotIndices[d-1]==n)//if we have a pivot on the right hand side then the system is already infeassible and there is no reason to add????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
      {
	return 0;
      }

    int numberOfPivotsBeforeCurrent=0;
    for(int j=0;j<pivotIndices[d-1];j++)if(isPivot[j])numberOfPivotsBeforeCurrent++;
    int J=0;
    for(int j=0;j<n;j++)if((!isPivot[j])&&(j!=pivotIndices[d-1])){temp2L[J++]=mL[d-1][j];}
    temp2R=mR[d-1];
#if HASH
    temp2H=mH[d-1];
#endif
    int newPivotIndex=pivotIndices[d-1]-numberOfPivotsBeforeCurrent;

    int ret=0;
    for(int i=0;i<numberOfUsedRowsInSource;i++)
      {
	ret=singleSingleReduction(sourceL,sourceR,
#if HASH
				  sourceH,
#endif
				  destinationL,destinationR,
#if HASH
				  destinationH,
#endif
				  newPivotIndex,ret,i);
	if(ret<0)return -1;//throw infeasibility result to parent
      }
    //cerr<<"Destination"<<destination;


    return ret;
  }
  int newAffineDimension()const
  {
    return n-d;
  }
#if HASH
  /**
     This method is given inequalities of the form [mL]*x<=[mR]
     and checks if there are some obvious inconsistencies. Only the
     first usedRows are taken into consideration. Two rows are an
     "obvious" inconsistency if they contradict each other. Since a
     complete check of obvious inconsistence takes time
     usedRows*log(usedRows), we do not make a complete check, but
     rather use a hashtable to match up rows of mL which are the same
     except sign.

     Return value true means that an inconsistency was found.

     The function is a method of the Reducer class even though n, d
     and hashTable are the only class members used.
   */
  bool hashedInconsistencyLookup(Matrix<typL> const &mL, Vector<typR> const &mR, Vector<typL> const &mH, int usedRows)const
  {
    //return false;
    //must be changed too
    int nMinusD=n-d;
    assert(mL.getWidth()==nMinusD);
    memset(hashTable,255,256);
    for(int i=0;i<usedRows;i++)
      {
	unsigned char h=hash2(mH[i]);
	hashTable[h]=i;
      }
    for(int i=0;i<usedRows;i++)
      {
	unsigned char h=hash2(-mH[i]);
	if(hashTable[h]!=255)
	  {
	    if(mL.rowsAreOpposite(i,hashTable[h],mL.getWidth()))if(isNegative(mR[i]+mR[hashTable[h]]))return true;
	  }
      }
    return false;
  }
#endif
}; // end of Reducer


  typedef Reducer<LType,RType> ReducerExact;



#if HASH
  //    unsigned char ReducerExact::hashTable[256];
#endif
}

/*
  Simplex algorithm for lp of the form
  min <w,y> subject to 
  yA=c
  y>=0

  The c vector does not have to be stored.
 */
namespace mixedCells
{
  static bool inRecursion=false;
  /*  template <class typL,class typR> class LP;
  template <class typL, class typR> bool haveEmptyIntersection(Cone<typL,typR> const &a, Cone<typL,typR> const &b, Reducer<typL,typR> *reducer=0);
  bool isUnboundedDirection(Vector<typL> const &v)const;
  */
  template <class typL,class typR> class LP
  {
  public:    int d,n;
    Matrix<typL> const *A;
    Matrix<typL> Ainv;
    Vector<typR> Ainvw;
    Vector<typR> const *www;
    Vector<typL> yValues;
    Vector<typL> edgeCandidateValues;
    int edgeCandidateOneEntry;
    vector<int> basis;
    vector<bool> inBasis;

    //    bool isUnboundedDirection(Vector<typL> const &v)const//after introducing row and choosing w as desired.
    bool isUnboundedDirection(Matrix<typL> const &m, Vector<typR> const &v, int i)const//after introducing row and choosing w as desired.
    {
      // We need to check that:
      // 1) introducing m[i] in the support of y, the other entries of y would also increase meaning that the y>=0 condition is never violated.
      // 2) this decreases the objective function.


      // First check
      for(int s=0;s<basis.size();s++)
	//	if(isNegative(-Ainv.vectorDotJthColumn(v,s)))
	if(isNegative(-m.rowDotColumnOfOther(i,Ainv,s)))
	  {
	    return false;
	  }

      // Second check
      typR d=-v[i];//SINCE THE DUAL LP IS A MINIMIZING PROBLEM, BUT OUR IMPLEMENTATION IS MAX, WE CHANGE SIGN
      for(int j=0;j<Ainvw.size();j++)d+=m[i][j]*Ainvw[j];
      /*d=-d;*/  if(isPositive(d)){/*cerr<<"TRUE\n";*/return true;}
      if(isNegative(d))return false;

      //return false;
      {
	cerr<<"Ainv"<<Ainv<<endl;
	cerr<<"i"<<i<<endl;
	cerr<<"v:"<<v<<endl;
	cerr<<"m"<<m<<endl;
	cerr<<"d:"<<d<<endl;
      }
      assert(0);
      return true;
    }

    void updateCandidateEdge(int i)
    {
      edgeCandidateOneEntry=i;
      // cerr << "Ainv " << Ainv;
      // cerr << "A " << A;
      for(int j=0;j<basis.size();j++)
	edgeCandidateValues[j]=-(*A).rowDotColumnOfOther(i,Ainv,j);

      /*      {static int a;
	a++;
	if((a&(257*255))==0){cerr<<edgeCandidateValues<<endl;
	  cerr<<edgeCandidateOneEntry<<endl;
	  //	  cerr<<Ainv;
	  //cerr<<basis;
	  for(int i=0;i<basis.size();i++)cerr<<","<<basis[i];cerr<<endl;
	  cerr<<*A;cerr<<*www;}
	  }*/
    }
    bool isImprovingDirection(int i)//const//i is non-basis
    {
      //      updateCandidateEdge(i);
      typR d=-(*www)[i];
      //      for(int j=0;j<basis.size();j++)d+=edgeCandidateValues[j]*w[basis[j]];
      for(int j=0;j<Ainvw.size();j++)d+=(*A)[i][j]*Ainvw[j];
      if(debug)cerr<<"EDGE"<<edgeCandidateValues<<"Oneidex:"<<edgeCandidateOneEntry<<"d"<<d<<endl;
      //      return dot(v,w)>0;
      if(isPositive(d))return true;
      if(isNegative(d))return false;
      /*      for(int i=0;i<v.size();i++)
	{
	  if(v[i]<-0.000001)return true;
	  if(v[i]>0.000001)return false;
	  }*/
	    return false;
	    //      return dot(v,w)>0;//-0.0001; //FUDGE FACTOR
    }
    typR improvement(/*int i,*/ int &newNonBasisMemberIndex)
    {
      //updateCandidateEdge(i);
      typR ew=-(*www)[edgeCandidateOneEntry];
      for(int j=0;j<basis.size();j++)ew-=edgeCandidateValues[j]*(*www)[basis[j]];
      typR ret;
      bool first=true;
      newNonBasisMemberIndex=-1;

      // This is the new anti-cycling rule
      for(int s=0;s<basis.size();s++)
	if(isNegative(edgeCandidateValues[s]))
	  {
	    if(first)
	      {
		newNonBasisMemberIndex=s;
		first=false;
	      }
	    else
	      {
		bool isBetter=true;
		//		  if(-yValues[s]/edgeCandidateValues[s]+EPSILON<-yValues[newNonBasisMemberIndex]/edgeCandidateValues[newNonBasisMemberIndex])goto isBetter;
		if(isEpsilonLessThan(-yValues[s]/edgeCandidateValues[s],-yValues[newNonBasisMemberIndex]/edgeCandidateValues[newNonBasisMemberIndex]))goto isBetter;
		if(isEpsilonLessThan(-yValues[newNonBasisMemberIndex]/edgeCandidateValues[newNonBasisMemberIndex],-yValues[s]/edgeCandidateValues[s]))goto isNotBetter;
		for(int a=0;a<basis.size();a++)
		  {
		    
		    if(isEpsilonLessThan(-Ainv[a][s]/edgeCandidateValues[s],-Ainv[a][newNonBasisMemberIndex]/edgeCandidateValues[newNonBasisMemberIndex]))goto isBetter;
		    if(isEpsilonLessThan(-Ainv[a][newNonBasisMemberIndex]/edgeCandidateValues[newNonBasisMemberIndex],-Ainv[a][s]/edgeCandidateValues[s]))goto isNotBetter;
		  }
		assert(0);
		isNotBetter:		    
		isBetter=false;
	      isBetter:
		if(isBetter)newNonBasisMemberIndex=s;
	      }
	  }
      if(newNonBasisMemberIndex!=-1)
	return -(yValues[newNonBasisMemberIndex])/edgeCandidateValues[newNonBasisMemberIndex]*ew;
      return 0;
    }
  public:
    /**
       The matrix A_ must be kept alive through out the life of the LP.

       How this should work in the future:
       We should not do memory allocation during the computation.
       Therefore there will be a fixed set of LPs that are stored in the recursion data.
       The A and c members are actually pointers to data in the recursion data.
       The LP will therefore not use all rows of A/entries of c, but rather use d to determine the height.
       Similarly only the first d entries of w and inBasis matter.

       To reach code that works with these conventions we do the following steps:
       (1) make the LP have a method setNumberOfRows() which must becalled before calling chooseRightHand...()
       (2) make the LP class actually use this number rather than A.getHeight() etc.
       (3) use tables in recursion data when constructing LP rather than copies of the data.
       (4) put the LPs themselves in the recursion data.
     */
  LP(Matrix<typL> const &A_):
    A(&A_),
      www(0),
      Ainv(A_.getWidth(),A_.getWidth()),
      yValues(A_.getWidth(),false),
      edgeCandidateValues(A_.getWidth(),false),
      Ainvw(A_.getWidth(),false),
      inBasis(A_.getHeight()),
      basis(A_.getWidth())
	{
	  d=-1;
	  //      d=A.getHeight();
	  n=A->getWidth();
	}
    
    void setNumberOfRows(int d_)
    {
      d=d_;
    }
    void setObjectiveFunction(Vector<typR> const &w_)
    {
      //      assert(w.size()==w_.size());// Sizes do not have to match anymore
      www=&w_;
    }
    friend std::ostream& operator<<(std::ostream& s, LP const &lp)
    {
      s<<"LP problem(d="<<lp.d<<")"<<endl;
      s<<"A="<<lp.A<<endl;
      
      s<<"w="<<(*lp.www)<<endl;
      s<<"yValues="<<lp.yValues<<endl;
      /*      {
	Matrix ym(1,lp.y.size());ym[0].set(lp.y);
	s<<"yA="<<ym*lp.A<<endl;
	}*/
      s<<"basis={";
      for(vector<int>::const_iterator i=lp.basis.begin();i!=lp.basis.end();i++)
	{
	  if(i!=lp.basis.begin())s<<",";
	  s<<*i;
	}
      s<<"}"<<endl;
      s<<"Ainv="<<lp.Ainv<<endl;
    }
    void updateAinvw()
    {
      Ainvw.clear();
      for(int j=0;j<basis.size();j++)
	for(int k=0;k<Ainv.getHeight();k++)
	  Ainvw[k]+=Ainv[k][j]*(*www)[basis[j]];
    }
    int step()
    {
      //      cerr<<A.getHeight()<<"x"<<A.getWidth()<<endl;
      typR impBest=-EPSILON;//FUDGE FACTOR
      int bestIndex=-1;
      int bestNewNonElementIndex=-1;
      for(int i=0;i<d;i++)
	if(!inBasis[i])
	{
	  //cerr<<"----"<<i<<"is not in Basis"<<endl;
	  if(isImprovingDirection(i))
	    {
	      updateCandidateEdge(i);
	      int newNonBasisElementIndex;
	      typR imp=improvement(/*i,*/newNonBasisElementIndex);
	      if(debug)cerr<<"Improvement"<<imp<<"newnonbasisindex"<<newNonBasisElementIndex<<"\n";
	      if(newNonBasisElementIndex==-1){
		/*		if(inRecursion)
		  {
		    cerr<<"UNBOUNDED"<<endl;
		    cerr<<*this;
		    }*/
/*cerr<<"UNBOUNDED"<<endl;*/return -1;
} //UNBOUNDED
	      //	      if((imp>=impBest))//needed for perturbation
	      if(isGreaterEqual(imp,impBest))//needed for perturbation
		{
		  impBest=imp;
		  bestIndex=i;
		  bestNewNonElementIndex=newNonBasisElementIndex;break;//<----not that it matters much, but we may return after the first improving direction is found.
		}
	    }
	}
      if(bestIndex==-1)
	return 0; //OPTIMAL
      
      updateCandidateEdge(bestIndex);

      typL scalar=-(yValues[bestNewNonElementIndex]/edgeCandidateValues[bestNewNonElementIndex]);
      yValues.madd(scalar,edgeCandidateValues);
      yValues[bestNewNonElementIndex]=scalar;
      //      assert(yValues[bestNewNonElementIndex]>-EPSILON);
      //cerr<<"Need to update Ainv according to edge direction"<<e;

      //cerr<<"Row "<<bestIndex<<"Can be expressed using"<<e<<endl;

      //cerr<<"|||||||scale"<<bestNewNonElementIndex<<"by"<<-e[basis[bestNewNonElementIndex]]/e[bestIndex];
      Ainv.scaleColumn(bestNewNonElementIndex,-1/edgeCandidateValues[bestNewNonElementIndex]);
      for(int i=0;i<basis.size();i++)
	if(i!=bestNewNonElementIndex)
	  {
	    Ainv.multiplyAndAddColumn(bestNewNonElementIndex,edgeCandidateValues[i]/1,i);
	    //cerr<<"Muladd"<<basis[i]<<"by"<<-e[basis[i]]/e[bestIndex]<<"to"<<bestNewNonElementIndex;
	  }
      //cerr<<endl;
      

      inBasis[basis[bestNewNonElementIndex]]=false;
      inBasis[bestIndex]=true;
      basis[bestNewNonElementIndex]=bestIndex;

      updateAinvw();

      /*      {
		Matrix Asub=Ainv;
	for(int i=0;i<Asub.getHeight();i++)
	  for(int j=0;j<Asub.getWidth();j++)
	    Asub[i][j]=A[basis[i]][j];
	
	cerr<<"Asubmatrix"<<Asub;
	cerr<<"Ainv"<<Ainv;
	cerr<<"Prod"<<Asub*Ainv;
	}*/
      //      y[basis[bestNewNonElementIndex]]=0;

      return 1;
    }
  private:
    void setBasis(vector<int> const &newBasis)
    {
      assert(newBasis.size()==basis.size());
      for(int i=0;i<basis.size();i++)basis[i]=newBasis[i];
    }
    void clearYValues()
    {
      for(int i=0;i<yValues.size();i++)yValues[i]=0;
    }
  public:
    void setTrivialBasisAndAinv(Matrix<typL> const &newAinv)
    {
      for(int i=0;i<basis.size();i++)basis[i]=i;
      clearYValues();

      for(int i=0;i<d;i++)inBasis[i]=false;

      for(int i=0;i<basis.size();i++)
	{
	  yValues[i]=1;
	  inBasis[basis[i]]=true;
	}
      Ainv=newAinv;//MALLOC
      updateAinvw();
    }
    void setBasisAndAinv(vector<int> const &newBasis, Matrix<typL> const &newAinv)
    {
      setBasis(newBasis);
      clearYValues();

      for(int i=0;i<d;i++)inBasis[i]=false;

      for(int i=0;i<newBasis.size();i++)
	{
	  yValues[i]=1;
	  inBasis[newBasis[i]]=true;
	}
      Ainv=newAinv;//MALLOC
      updateAinvw();
    }
    void setBasisAndComputeAinv(vector<int> const &newBasis)
    {
      setBasis(newBasis);
      clearYValues();
      Matrix<typL> ASub(A->getWidth(),A->getWidth());//MALLOC

      for(int i=0;i<d;i++)inBasis[i]=false;

      for(int i=0;i<newBasis.size();i++)
	{
	  yValues[i]=1;
	  inBasis[newBasis[i]]=true;
	  ASub[i].set((*A)[newBasis[i]].toVector());//MALLOC
	}
      Ainv=ASub.inverse();//MALLOC

      //      {static int a;a++;if((a&255)==0)cerr<<"Computed via Gauss:"<<Ainv;}

      updateAinvw();
    }
    void chooseRightHandSideToMakeFeasibleSolution()
    {
      assert(d>=0);
      Matrix<typL> A2=A->transposed();//MALLOC
      A2.reduce(false);
      int basisI=0;
      clearYValues();
      Matrix<typL> ASub(A->getWidth(),A->getWidth());//MALLOC
      int index=0;
      for(int i=0;i<d;i++)inBasis[i]=false;
      {
	int pivotI=-1;
	int pivotJ=-1;
	while(A2.nextPivot(pivotI,pivotJ))
	  {
	    yValues[index]=1;
	    inBasis[pivotJ]=true;
	    basis[basisI++]=pivotJ;
	    ASub[index++].set((*A)[pivotJ].toVector());//MALLOC
	  }
	assert(Ainv.getHeight()==ASub.getHeight());
	assert(Ainv.getWidth()==ASub.getWidth());
	Ainv=ASub.inverse();//MALLOC
	assert(Ainv.getHeight()==ASub.getHeight());
	assert(Ainv.getWidth()==ASub.getWidth());
	updateAinvw();
      }
      if(index!=A->getWidth())
	{
	  cerr<<*this;
	  cerr<<"A2"<<A2<<endl;
	  assert(0);
	}
      //    setBasisAndAinv(basis,Ainv);///Just for debugging
      //      setBasisAndComputeAinv(basis);///Just for debugging
    }
  };

  typedef LP<LType,RType> LPExact;

  class Polytope
  {
  public:
    int n;
    vector<vector<int> > vertices;
    Polytope(int n_, vector<vector<int> > const &vertices_):
      n(n_),
      vertices(vertices_)
    {
    }
    int ambientDimension()const{return n;}
  };
  vector<int> readVector(istream &s, int length)
  {
    vector<int> ret(length);
    for(int i=0;i<length;i++)
      s>>ret[i];
    return ret;
  }
  vector <Polytope> readPolytopes(istream &s, int &dim)
  {
    string temp;
    int numberOfPolytopes;
    s>> temp>>temp>>dim;
    s>> temp>>temp>>numberOfPolytopes;

    assert(numberOfPolytopes>0);
    assert(numberOfPolytopes<10000);

    s>> temp>>temp;
    vector<int> elem=readVector(s,numberOfPolytopes);

    s>> temp>>temp;
    vector<int> type=readVector(s,numberOfPolytopes);

    //cerr<<"\""<<temp<<"\"\n";

    vector<Polytope> ret;
    for(int i=0;i<numberOfPolytopes;i++)
      {
	vector<vector<int> > vertices(elem[i]);
	
	for(int j=0;j<elem[i];j++)
	  vertices[j]=readVector(s,dim);

	ret.push_back(Polytope(dim,vertices));
      }
    return ret;
  }

  template <class typL, class typR> class Cone;
  template <class typL, class typR> bool haveEmptyIntersection(Cone<typL,typR> const &a, Cone<typL,typR> const &b, Reducer<typL,typR> *reducer=0);

  /**
     This method represents a polyhedral region give by
     [inequalitiesL]*x<=[inequalitiesR]
     [equationsL]*x=[equationsR]
  */
  template <class typL,class typR> class Cone
  {
  public:
    int n;
    Matrix<typL> inequalitiesL;
    Vector<typR> inequalitiesR;
    Vector<typL> inequalitiesH;
    Matrix<typL> equationsL;
    Vector<typR> equationsR;
    Vector<typL> equationsH;
    /*    Cone(int n_, MatrixDouble const &inequalities_, MatrixDouble const &equations_):
      n(n_),
      inequalitiesL(inequalities_.submatrix(0,0,inequalities_.getHeight(),n_-1)),
      inequalitiesR(inequalities_.getHeight()),
      equationsL(equations_.submatrix(0,0,equations_.getHeight(),n_-1)),
      equationsR(equations_.getHeight())
    {
      for(int i=0;i<inequalitiesR.size();i++)inequalitiesR[i]=inequalities_[i][n-1];
      for(int i=0;i<equationsR.size();i++)equationsR[i]=equations_[i][n-1];
      assert((n-1)==inequalitiesL.getWidth());
      assert((n-1)==equationsL.getWidth());
      }*/
    Cone(int n_, Matrix<typL> const &ineqL, Vector<typR> const &ineqR, Matrix<typL> const &eqL, Vector<typR> const &eqR):
      n(n_),
      inequalitiesL(ineqL),
      inequalitiesR(ineqR),
      equationsL(eqL),
      equationsR(eqR)
    {
    } 
    friend std::ostream &operator<<(std::ostream &out, Cone const &cone)
    {
      out<<"Printing Cone"<<endl;
      out<<"Ambient dimension:"<<cone.n<<endl;
      out<<"InequalitiesL:"<<endl<<cone.inequalitiesL;
      out<<"InequalitiesR:"<<endl<<cone.inequalitiesR;
      out<<"EquationsL:"<<endl<<cone.equationsL;
      out<<"EquationsR:"<<endl<<cone.equationsR;
      out<<"Done printing Cone"<<endl;
      return out;
    }
    /*    bool doesContain(VectorDouble const &v)const
    {
      for(int i=0;i<equations.getHeight();i++)
	if(dot(v,equations[i].toVector())>0.0001)return false;
      for(int i=0;i<equations.getHeight();i++)
	if(dot(v,equations[i].toVector())<-0.0001)return false;
      for(int i=0;i<inequalities.getHeight();i++)
	if(dot(v,inequalities[i].toVector())<-0.0001)return false;
      return true;
      }*/
    bool hasPointWithLastCoordinatePositiveInCone(Matrix<typL> &coneInequalitiesL, Vector<typR> &coneInequalitiesR,
#if HASH
						  Vector<typL> &coneInequalitiesH,
#endif
						  int oldNumberOfInequalities, int &newNumberOfInequalities, ReducerExact &reducer/*, Matrix<typL> &Inequalities*/, LPExact &lp, bool quickExit/*=false*/, Matrix<typL> *Ainv/*if this is not null, then basis of A is the first entries */)//coneInequalitiesL/R must survive until lp is destroyed
    {
      //cerr<<"----INCONE"<<endl;
      statistics.nLPs++;
      int numberOfAddedInequalities=reducer.reduction(inequalitiesL,inequalitiesR,
#if HASH
						      inequalitiesH,
#endif
						      coneInequalitiesL,coneInequalitiesR,
#if HASH
						      coneInequalitiesH,
#endif
						      oldNumberOfInequalities);
      //cerr<<"ADDED:"<<numberOfAddedInequalities<<endl;
      if(numberOfAddedInequalities<0)return false;

      //  {static int n;n++;if((n&1023)==0)cerr<<coneInequalitiesL<<"used"<<oldNumberOfInequalities<<"+"<<numberOfAddedInequalities<<endl;}

      newNumberOfInequalities=oldNumberOfInequalities+numberOfAddedInequalities;

#if HASH
      if(reducer.hashedInconsistencyLookup(coneInequalitiesL,coneInequalitiesR, coneInequalitiesH,newNumberOfInequalities)){/*cerr<<"A";*/return false;}/*else cerr<<"B";*/
#endif
      int newAffineDimension=reducer.newAffineDimension();
      assert(coneInequalitiesL.getWidth()==newAffineDimension);
      /*Matrix<typL>*/ //Inequalities=coneInequalitiesL.submatrix(0,0,newNumberOfInequalities,newAffineDimension);
      //      Vector<typR>  RightHandSide=coneInequalitiesR.subvector(0,newNumberOfInequalities);//SINCE THE DUAL LP IS A MINIMIZING PROBLEM, BUT OUR IMPLEMENTATION IS MAX, WE CHANGE SIGN


      //lp=new LPExact(coneInequalitiesL);
      //lp=new LPExact(Inequalities,Vector<typL>(Inequalities.getWidth()));
      lp.setNumberOfRows(newNumberOfInequalities);
      lp.setObjectiveFunction(coneInequalitiesR/*RightHandSide*/);
      //      lp.chooseRightHandSideToMakeFeasibleSolution();
      if(Ainv)
      {
	//	vector<int> newBasis;
	//for(int i=0;i<newAffineDimension;i++)newBasis.push_back(i);
	//lp.setBasisAndComputeAinv(newBasis);
	lp.setTrivialBasisAndAinv(*Ainv);
      }      
      else
	lp.chooseRightHandSideToMakeFeasibleSolution();

      // cerr<<reducer;
      // cerr<<*this<<coneInequalitiesL<<coneInequalitiesR<<oldNumberOfInequalities<<newNumberOfInequalities<<endl;
      // cerr<<"INCONE"<<lp;
      int status;
      int loops=0;

      //      {static int i;i=(i+1)&1023;if(!i)cerr<<lp;}
      do
	{
	  if(loops++>10000)
	    {
	      cerr<<lp;
	      //debug=true;
	    }
	  status=lp.step();
	  if(quickExit && loops>=40)break;
	}
      while(status==1);
      //cerr<<"STATUS"<<status<<endl;//{static int p;assert(p++<9);}
      bool isFeasible=(status!=-1);
      if(isFeasible)statistics.nFeasible++;
      //squareMatrixAllocator.free();
      return isFeasible;
    }
    bool hasPointWithLastCoordinatePositive(ReducerExact *reducer=0)const
    {
      statistics.nLPs++;
      bool isFeasible;
      {
	Matrix<typL> Inequalities(0,0);
	Vector<typR> RightHandSide(0);
	Vector<typL> Hash(0);

	if(reducer)
	  {
	    assert(0);//it is not clear that right hand sides are treated correctly in the following code
	    int numberOfInequalities=inequalitiesL.getHeight();
	    int newAffineDimension=reducer->newAffineDimension();
	    Inequalities=Matrix<typL>(numberOfInequalities,newAffineDimension);
	    RightHandSide=Vector<typR>(numberOfInequalities);
#if HASH
	    Hash=Vector<typL>(numberOfInequalities);
#endif
	    //int newNumberOfInequalities=reducer->storeAndSplitNormalForms(this->inequalitiesL,this->inequalitiesR,Inequalities,RightHandSide);
	    int newNumberOfInequalities=reducer->reduction(this->inequalitiesL,this->inequalitiesR,
#if HASH
							   this->inequalitiesH,
#endif
Inequalities,RightHandSide,
#if HASH
							   Hash,				   
#endif
0);
	    assert(0);	    assert(newNumberOfInequalities>=0);
	    if(newNumberOfInequalities!=this->inequalitiesL.getHeight())
	      {
		Inequalities=Inequalities.submatrix(0,0,newNumberOfInequalities,Inequalities.getWidth());
		RightHandSide=RightHandSide.subvector(0,newNumberOfInequalities);
	      }
	  }
	else
	  {
	    Matrix<typL> equationsL=this->equationsL;
	    Vector<typR> equationsR=this->equationsR;
	    //	    equations.reduce(false);
	    reducePair(equationsL,equationsR,false);
	    //	    cerr << equationsL << endl << equationsR << endl; 
	    {
	      if(equationsL.numberOfPivots()!=equationsL.getHeight())return false;//THIS ONLY WORKS IF THERE ARE NO REPEATED EQUATIONS
	      /*	      int pivotI=-1;
	      int pivotJ=-1;
	      while(equations.nextPivot(pivotI,pivotJ))if(pivotJ==equations.getWidth()-1)return false;
	      */
	      int maxpivot=-1;
	      int pivotI=-1;
	      int pivotJ=-1;
	      while(equationsL.nextPivot(pivotI,pivotJ))if(maxpivot<pivotJ)maxpivot=pivotJ;
	      if((maxpivot+1)<equationsL.getHeight())if(!isZero(equationsR[maxpivot+1]))return false;	      
	    }
	    
	    /*	    MatrixDouble inequalities=equations.normalForms(this->inequalities);
	    inequalities.removeZeroRows();
	    
	    Inequalities=inequalities.submatrix(0,0,inequalities.getHeight(),inequalities.getWidth()-1);
	    RightHandSide=(-inequalities.submatrix(0,inequalities.getWidth()-1,inequalities.getHeight(),inequalities.getWidth())).transposed()[0].toVector();
	    */
	    // Here we compute normal forms of inequalities L+R, and store them in Inequalities and Righthandside
	    
	    normalFormPairs(inequalitiesL,inequalitiesR,Inequalities,RightHandSide,equationsL,equationsR);
	    removeZeroRowsPair(Inequalities,RightHandSide);
	    //RightHandSide=-RightHandSide;//SINCE THE DUAL LP IS A MINIMIZING PROBLEM, BUT OUR IMPLEMENTATION IS MAX, WE CHANGE SIGN
	  }

	
	//If inequalities do not span space, then we may restrict
	Inequalities=Inequalities.reduceDimension();
	for(int i=0;i<Inequalities.getHeight();i++)if(Inequalities[i].isZero())if(isNegative(RightHandSide[i]))return false;
	

	LPExact lp(Inequalities);
	lp.setNumberOfRows(Inequalities.getHeight());
	lp.setObjectiveFunction(RightHandSide);
	lp.chooseRightHandSideToMakeFeasibleSolution();
	//	cerr<<lp;assert(0);
	int status;
	do
	  {
	    status=lp.step();
	  }
	while(status==1);
	isFeasible=(status!=-1);
	//squareMatrixAllocator.free();
      }
      if(isFeasible)statistics.nFeasible++;
      return isFeasible;
    }
    void removeRedundantInequalities()
    {
      int old=inequalitiesL.getHeight();
      //cerr<<inequalitiesL;
      //cerr<<inequalitiesR;
      for(int i=0;i<inequalitiesL.getHeight();i++)
	{
	  inequalitiesL[i].set(-1*inequalitiesL[i].toVector());
	  inequalitiesR[i]=-inequalitiesR[i];
	  bool doRemove=!hasPointWithLastCoordinatePositive();
	  inequalitiesR[i]=-inequalitiesR[i];
	  inequalitiesL[i].set(-1*inequalitiesL[i].toVector());
	  if(doRemove)
	    {
	      inequalitiesL=combineOnTop(inequalitiesL.submatrix(0,0,i,inequalitiesL.getWidth()),inequalitiesL.submatrix(i+1,0,inequalitiesL.getHeight(),inequalitiesL.getWidth()));
	      inequalitiesR=concatenation(inequalitiesR.subvector(0,i),inequalitiesR.subvector(i+1,inequalitiesR.size()));
	      i--;
	    }
	}
      //fprintf(stderr,"%i->%i\n",old,inequalitiesL.getHeight());
    }
    friend Cone intersection(Cone const &a, Cone const &b)
    {
      assert(a.n==b.n);
      
      //return Cone(a.n, combineOnTop(a.inequalities,b.inequalities),combineOnTop(a.equations,b.equations));
      return Cone(a.n,combineOnTop(a.inequalitiesL,b.inequalitiesL),
		  concatenation(a.inequalitiesR,b.inequalitiesR),
		  combineOnTop(a.equationsL,b.equationsL),
		  concatenation(a.equationsR,b.equationsR));
    }
#if HASH
    void computeHash()
    {
      inequalitiesH=Vector<typL>(inequalitiesR.size());
      equationsH=Vector<typL>(equationsR.size());
      for(int i=0;i<inequalitiesR.size();i++)
	{
	  inequalitiesH[i]=inequalitiesL[i].toVector().hash(SEED);
	}
      for(int i=0;i<equationsR.size();i++)
	{
	  equationsH[i]=equationsL[i].toVector().hash(SEED);
	}
    }
#endif

    friend bool haveEmptyIntersection <>(Cone const &a, Cone const &b, Reducer<typL,typR> *reducer);
  }; // end of Cone

  template <class typL,class typR>
  inline bool haveEmptyIntersection(Cone<typL,typR> const &a, Cone<typL,typR> const &b, Reducer<typL,typR> *reducer=0)
  {
    return !intersection(a,b).hasPointWithLastCoordinatePositive(reducer);
    //    return false;//FIX THIS
    }
  

  template <class typL,class typR> class Fan
  {
    int n;
  public:
    vector<Cone<typL,typR> > cones;
    vector<Cone<typL,typR> > fullDimCones;
    vector<pair<int,int> > coneNeighbours;
    Matrix<typL> edges;// every cone comes from an edge - used for finding volume of mixed cell
    int getAmbientDimension()const{return n;}
    Fan(int n_):
      n(n_),
      edges(0,n_)
    {
    }
    int maximalNumberOfInequalitiesOfACone()
    {
      int ret=0;
      for(int i=0;i<cones.size();i++)if(ret<cones[i].inequalitiesL.getHeight())ret=cones[i].inequalitiesL.getHeight();
      return ret;
    }
    int size()const{return cones.size();}
    static double random()
    {
      double d=0;
      for(int i=0;i<53;i++){d=(d+(rand()&1))*0.5;}
      return d;
    }
    static Fan fromPolytope(Polytope const &p)
    {
      int numberOfVertices=p.vertices.size();
      Vector<typR> heights(numberOfVertices);
      for(int i=0;i<numberOfVertices;i++)heights[i].random();
      cerr<<"Heights"<<heights<<endl;

      vector<pair<int,int> > edges;

      for(int i=0;i<numberOfVertices;i++)
	for(int j=0;j<i;j++)
	  edges.push_back(pair<int,int>(j,i));


      int n=p.ambientDimension();
      Fan ret(n+1);
      Matrix<typL> edgeVectors(edges.size(),n);

      ret.coneNeighbours=edges;
      cerr<<"EDGESSIZE"<<edges.size()<<endl;
      
      for(int I=0;I<numberOfVertices;I++)//build fullDimCones
	{
	  	  Matrix<typL> equationsL(0,n);
	  Vector<typR> equationsR(0);
	  Matrix<typL> inequalitiesL(numberOfVertices-1,n);
	  Vector<typR> inequalitiesR(numberOfVertices-1);
	  
	  int K=0;
	  for(int k=0;k<numberOfVertices;k++)
	    if(k!=I)
	      {
		inequalitiesR[K]=heights[I]-heights[k];
		for(int j=0;j<n;j++)inequalitiesL[K][j]=p.vertices[I][j]-p.vertices[k][j];
		K++;
	      }
	  
	  ret.fullDimCones.push_back(Cone<typL,typR> (n+1,inequalitiesL,inequalitiesR,equationsL,equationsR));
	  
	}

      int I=0;
      for(vector<pair<int, int> >::const_iterator i=edges.begin();i!=edges.end();i++,I++)//build cones
	{
	  int a=i->first;
	  int b=i->second;
	  Matrix<typL> equationsL(1,n);
	  Vector<typR> equationsR(1);
	  equationsR[0]=heights[a]-heights[b];
	  for(int j=0;j<n;j++)equationsL[0][j]=p.vertices[a][j]-p.vertices[b][j];

	  Matrix<typL> inequalitiesL(numberOfVertices-1,n);
	  Vector<typR> inequalitiesR(numberOfVertices-1);
	  /*	  for(int j=0;j<n+1;j++)inequalities[0][j]=0;
	  inequalities[0][0]=1;//SIGN OF T
	  */	  int K=0/*1*/;
	  for(int k=0;k<numberOfVertices;k++)
	    if(k!=b)
	      {
		inequalitiesR[K]=heights[b]-heights[k];
		for(int j=0;j<n;j++)inequalitiesL[K][j]=p.vertices[b][j]-p.vertices[k][j];
		K++;
	      }
	  ret.cones.push_back(Cone<typL,typR> (n+1,inequalitiesL,inequalitiesR,equationsL,equationsR));
	  for(int j=0;j<n;j++)edgeVectors[I][j]=p.vertices[a][j]-p.vertices[b][j];
	}
      ret.edges=edgeVectors;
      return ret;
    }
    
    friend std::ostream &operator<<(std::ostream &out, Fan const &fan)
    {
      out<<"Number of cones in fan:"<<fan.size()<<endl;
      for(typename std::vector<Cone<typL,typR> >::const_iterator i=fan.cones.begin();i!=fan.cones.end();i++)
	{
	  out<<*i;
	}
      cerr<<fan.edges;
      out<<"Done printing fan"<<endl;
      return out;
    }
  };
  typedef Cone<LType,RType> ConeType;
  typedef Fan<LType,RType> FanType;

#define BITSPERINTEGER 32
#define BITSPERINTEGER_LOG2 5
#define INTEGERSPERSET 7
class BitSet
{
  //  vector<int> v;
  int v[INTEGERSPERSET];
  int n;
public:
  BitSet():
    n(0)
  {
  }
  BitSet(int n_):
    n(n_)
  {
    assert(n<INTEGERSPERSET*BITSPERINTEGER);
    for(int i=0;i<INTEGERSPERSET;i++)v[i]=0;
  }
  bool get(int i)const{
    assert(i>=0 && i<n);
    int mask=1<<(i&(BITSPERINTEGER-1));
    int index=i>>BITSPERINTEGER_LOG2;
    return mask&(v[index]);
  }
  void set(int i, bool value){
    assert(i>=0 && i<n);
    int mask=1<<(i&(BITSPERINTEGER-1));
    int index=i>>BITSPERINTEGER_LOG2;
    v[index]=(v[index]&(-1-mask))|((value)?mask:0);
  }
  //  int& operator[](int n){assert(n>=0 && n<v.size());return (v[n]);}
  //const int& operator[](int n)const{assert(n>=0 && n<v.size());return (v[n]);}
  void add(BitSet const &b)
  {
    assert(b.n==n);
    for(int i=0;i<INTEGERSPERSET;i++)
      v[i]|=b.v[i];
  }
  inline int size()const
  {
    return n;
  }
  friend std::ostream& operator<<(std::ostream& s, const BitSet &t)
  {
    s<<"(";
    for(int i=0;i<t.size();i++)
      {
	if(i!=0)s<<", ";
	s<<t.get(i);
      }
    s<<")\n";
    }
  BitSet negated()const
  {
    BitSet ret(size());
    for(int i=0;i<INTEGERSPERSET;i++)ret.v[i]=-1-v[i];
    return ret;
  }
  int sizeOfSubset()const
  {
    int ret=0;
    for(int i=0;i<size();i++)if(get(i))ret++;
    return ret;
  }
  };

class Table
{
  vector<vector<vector<BitSet> > > table;
public:
  Table(vector<FanType> const &l):
    table(l.size())
  {
    int N=l.size();
    for(int i=0;i<N;i++)
      {
	vector<vector<BitSet> > v(N);
	for(int j=0;j<N;j++)
	  {
	    vector<BitSet> w(l[i].size());
	    for(int k=0;k<l[i].size();k++)
	      {  w[k]=BitSet(l[j].size());
	      }
	    v[j]=w;
	  }
	table[i]=v;
      }
  }
  bool lookUp(int fan1, int cone1, int fan2, int cone2)
  {
    assert(fan1<table.size());
    assert(fan2<table[fan1].size());
    assert(cone1<table[fan1][fan2].size());
    assert(cone2<table[fan1][fan2][cone1].size());

    return table[fan1][fan2][cone1].get(cone2);
  }
  void set(int fan1, int cone1, int fan2, int cone2)
  {
    assert(fan1<table.size());
    assert(fan2<table[fan1].size());
    assert(cone1<table[fan1][fan2].size());
    assert(cone2<table[fan1][fan2][cone1].size());

    table[fan1][fan2][cone1].set(cone2,true);
    table[fan2][fan1][cone2].set(cone1,true);
    //table[fan1][fan2][cone1].setValue(cone2,true);
    //table[fan2][fan1][cone2].setValue(cone1,true);
  }
  BitSet const& nonCandidates(int fan1, int cone1, int fan2)const
  {
    assert(fan1<table.size());
    assert(fan2<table[fan1].size());
    assert(cone1<table[fan1][fan2].size());

    return table[fan1][fan2][cone1];
  }
  friend std::ostream& operator<<(std::ostream& s, const Table &t)
  {
    for(int i=0;i<t.table.size();i++)
      for(int j=0;j<t.table[i].size();j++)
	{
	  s<<"Entry ("<<i<<","<<j<<")\n";
	  for(int k=0;k<t.table[i][j].size();k++)
	    s<<t.table[i][j][k];
	}
  }
};

class RelationTable
{
  vector<FanType> fanList;
  Table knownEmptyIntersectionInIntersection;
  Table knownNonEmptyIntersection;
public:
  int numberOfSolvedLPs;
  RelationTable(vector<FanType> const &l):
    fanList(l),
    knownEmptyIntersectionInIntersection(l),
    knownNonEmptyIntersection(l),
    numberOfSolvedLPs(0)
  {

  }
  bool knownToIntersectTriviallyInIntersection(int fan1, int cone1, int fan2, int cone2)
  {
    assert(fan1<fanList.size());
    assert(fan2<fanList.size());
    assert(cone1<fanList[fan1].size());
    assert(cone2<fanList[fan2].size());

    return knownEmptyIntersectionInIntersection.lookUp(fan1,cone1,fan2,cone2);
  }
  bool intersectTriviallyInIntersection(int fan1, int cone1, int fan2, int cone2)
  {
    assert(fan1<fanList.size());
    assert(fan2<fanList.size());
    assert(cone1<fanList[fan1].size());
    assert(cone2<fanList[fan2].size());


    if(knownEmptyIntersectionInIntersection.lookUp(fan1,cone1,fan2,cone2))
      return true;
    if(knownNonEmptyIntersection.lookUp(fan1,cone1,fan2,cone2))
      return false;

    //    fprintf(Stderr,"UPDATING:f1:%i,c1:%i,f2:%i,c2:%i\n",fan1,cone1,fan2,cone2);
    bool ret;
    if((fan1!=fan2) && (cone1!=cone2))
      ret=/*Cone<LType,RType>::*/haveEmptyIntersection(fanList[fan1].cones[cone1],fanList[fan2].cones[cone2]);
    //ret=fanList[fan1].cones[cone1].haveEmptyIntersection(fanList[fan2].cones[cone2]);
    else
      ret=false;
    //    cerr<<"UPDATING:f1:"<<fan1<<",c1:"<<cone1<<",f2:"<<fan2<<",c2:"<<cone2<<"ret"<<ret<<"\n";
    numberOfSolvedLPs++;
    if(ret)
      knownEmptyIntersectionInIntersection.set(fan1,cone1,fan2,cone2);
    else
      knownNonEmptyIntersection.set(fan1,cone1,fan2,cone2);
    return ret;
  }
  const BitSet &getNonCandidates(int fan1, int cone1, int fan2)
  {
    //  for(int c2=0;c2<fanList[fan2].size();c2++)
	  //      intersectTriviallyInIntersection(fan1,cone1,fan2,c2);

    return knownEmptyIntersectionInIntersection.nonCandidates(fan1,cone1,fan2);
  }
  void markNoIntersectionInIntersection(int fan1, int cone1, int fan2, int cone2)
  {
    //    cerr<<"MARKING"<<fan1<<cone1<<fan2<<cone2<<endl;assert(fan1!=fan2 || cone1!=cone2);
    knownEmptyIntersectionInIntersection.set(fan1,cone1,fan2,cone2);
  }
  void markKnownNonEmptyIntersection(int fan1, int cone1, int fan2, int cone2)
  {
	  knownNonEmptyIntersection.set(fan1,cone1,fan2,cone2);
  }
  friend std::ostream& operator<<(std::ostream& s, const RelationTable &t)
  {
    s<<"knownEmptyIntersectionInIntersection:";
    s<<t.knownEmptyIntersectionInIntersection;
    s<<"knownNonEmptyIntersection:";
    s<<t.knownNonEmptyIntersection;
  }
};


struct RecursionData
{
  int ambientDimension; //with the t-coordinate
  vector<FanType> fans;
  vector<Matrix<LType> > inequalityMatricesL;//one matrix for each recursion level
  vector<Vector<RType> > inequalityMatricesR;
#if HASH
  vector<Vector<LType> > inequalityMatricesH;
#endif
  vector<LPExact> lpList;
  IntegerVector inequalityMatricesNumberOfUsedRows1;
  IntegerVector inequalityMatricesNumberOfUsedRows2;
  IntegerVector chosen;
  IntegerVector chosenFans;
  IntegerVector iterators; //just used for printing
  IntegerVector nCandidates; //just used for printing
  BitSet usedFans;
  int numberOfUsefulCalls;
  int totalNumberOfCalls;
  ReducerExact reducer;
  vector<Matrix<LType> > AinvList;//For all levels but possibly the first, this will contain the inverse of the first squarematrix of inequalityMatricesL
public:
  RelationTable table;
  int cellVolume()
  {
    Matrix<LType> m(fans.size(),ambientDimension-1);
    for(int i=0;i<fans.size();i++)
      m[i].set(fans[chosenFans[i]].edges[chosen[i]].toVector());
    //cerr<<m;
    LType d=m.reduceAndComputeDeterminant();
    //cerr<<"DETERMINANT"<<d<<endl;
    return volumeToInt(d);
  }
  RecursionData(int ambientDimension_, vector<FanType> const &fans_):
    ambientDimension(ambientDimension_),
    table(fans_),
    fans(fans_),
    chosen(fans_.size()),
    chosenFans(fans_.size()),
    usedFans(fans_.size()),
    iterators(fans_.size()),
    nCandidates(fans_.size()),
    numberOfUsefulCalls(0),
    totalNumberOfCalls(0),
    reducer(fans_.size()),
    inequalityMatricesL(0),
    inequalityMatricesR(0),
#if HASH
    inequalityMatricesH(0),
#endif
    inequalityMatricesNumberOfUsedRows1(fans_.size()),
    inequalityMatricesNumberOfUsedRows2(fans_.size())
  {
    int totalNumberOfInequalities=0;
    int maximalNumberOfInequalities=0;
    for(int i=0;i<fans_.size();i++)
      {
	int a=fans[i].maximalNumberOfInequalitiesOfACone();
	//cerr<<a;
	totalNumberOfInequalities+=a;
	if(a>maximalNumberOfInequalities)maximalNumberOfInequalities=a;
      }
    for(int i=0;i<ambientDimension-1;i++)
      {
	inequalityMatricesL.push_back(Matrix<LType>(min((i+1)*maximalNumberOfInequalities,totalNumberOfInequalities),ambientDimension-i-1-1));
	inequalityMatricesR.push_back(Vector<RType>(min((i+1)*maximalNumberOfInequalities,totalNumberOfInequalities)));
#if HASH
	inequalityMatricesH.push_back(Vector<LType>(min((i+1)*maximalNumberOfInequalities,totalNumberOfInequalities)));
#endif
	AinvList.push_back(Matrix<LType>(ambientDimension-i-1-1,ambientDimension-i-1-1));
      }
    for(int i=0;i<ambientDimension-1;i++)
      {
	lpList.push_back(LPExact(inequalityMatricesL[i]));
      }
  }
  BitSet computeCandidates(int index, int fanNumber)
  {
    BitSet nonCandidates(fans[fanNumber].size());
      for(int i=0;i<index;i++)
      {
	nonCandidates.add(table.getNonCandidates(chosenFans[i],chosen[i],fanNumber));
      }
      /* <------------------------------------------------------------------------------------------- Remember to never uncomment this
      for(int j=0;j<nonCandidates.size();j++)
	if((!nonCandidates.get(j)))// ||randBool()
	  for(int i=0;i<index;i++)
	    if(table.intersectTriviallyInIntersection(chosenFans[i], chosen[i], fanNumber, j))
	      {
		nonCandidates.set(j,true);
		break;
	      }
       */      
    return nonCandidates.negated();
  }

  bool closure()
  {
    //cerr<<table;assert(0);
    bool ret=false;
    int a=0;
    for(int f1=0;f1<fans.size();f1++)
      {
	for(int f2=f1+1;f2<fans.size();f2++)
	  for(int c1=0;c1<fans[f1].size();c1++)
	    for(int c2=0;c2<fans[f2].size();c2++)
	      {
		//		if(!table.intersectTriviallyInIntersection(f1,c1,f2,c2))
		if(!table.knownToIntersectTriviallyInIntersection(f1,c1,f2,c2))
		  {
		    bool dontintersect=false;
		    for(int f3=0;f3<fans.size();f3++)
		      {
			// c = union of cones of f3 feasible wrt f1c1 and those feasible wrt f2c2  
			BitSet c=table.getNonCandidates(f1,c1,f3);
			c.add(table.getNonCandidates(f2,c2,f3));
			//cerr<<table.getNonCandidates(f1,c1,f3);
			//cerr<<table.getNonCandidates(f2,c2,f3);
			if(c.negated().sizeOfSubset()==0) // (cones of f3 feasible wrt f1c1) \cap (cones of f3 feasible wrt f2c2) = \emptyset  
			  {
			    dontintersect=true;
			    a++;
			    statistics.forFree++;
			    //  cerr<<" f3:"<<f3;
			    break;
			  }
						if(c.negated().sizeOfSubset()<4 && ((f3&7) ==0))//just an experiment
			  {
			    for(int k=0;k<c.size();k++)
			      if(!c.get(k))
				{
				  table.intersectTriviallyInIntersection(f1,c1,f3,k);
				  table.intersectTriviallyInIntersection(f2,c2,f3,k);
				}
				}
		      }
		    if(dontintersect)
		      {
			table.markNoIntersectionInIntersection(f1,c1,f2,c2);
			ret=true;
			//cerr<<"f1:"<<f1<<" f2:"<<f2<<" c1:"<<c1<<" c2:"<<c2<<endl;
			//cerr<<table;
		      }
		  }
	      }
      }
    cerr<<a<<" FOR FREE\n";
    return ret;
  }

  void transitiveClosure()
  {
    while(closure());
  }

  void completeTable() // fills in this->table using pairs of edges and transitivity 
  {
	    for(int f1=0;f1<fans.size();f1++)
	    	for(int c1=0;c1<fans[f1].size();c1++)
	    	  for(int c2=0;c2<fans[f1].size();c2++)
	    		  if(c1!=c2)
	    			  table.markNoIntersectionInIntersection(f1,c1,f1,c2);
//	    		  else
//	    			  table.markKnownNonEmptyIntersection(f1,c1,f1,c2);


	    	for(int f1=0;f1<fans.size();f1++)
      {
	for(int f2=f1+1;f2<fans.size();f2++)
	for(int c1=0;c1<fans[f1].size();c1++)
	  for(int c2=0;c2<fans[f2].size();c2++)
	    table.intersectTriviallyInIntersection(f1,c1,f2,c2);
      }
		//		    cerr<<table;
		    transitiveClosure();
		    //   cerr<<table;
  }
#if HASH
  void computeHash()
  {
    for(int f1=0;f1<fans.size();f1++)
      for(int c1=0;c1<fans[f1].size();c1++)
	fans[f1].cones[c1].computeHash();
  }
#endif
  /*
    Returns mixed volume for subtree.
   */
  int rek(int index, Cone<LType,RType>  const &current, LP<LType,RType> const *parentLP=0)
  {
    statistics.nRekCalls++;
    totalNumberOfCalls++;

    int mixedVolumeAccumulator=0;

    if(index == fans.size())
      {
	//	cerr<<"CELL FOUND\n";
	numberOfUsefulCalls++;
	statistics.nCells++;
	return int(cellVolume()+0.5);
      }
    else
      {
	int bestIndex=-1;
	int bestNumberOfCandidates=1000000;
	BitSet bestCandidates;
	for(int i=0;i<fans.size();i++)
	  {
	    if(!usedFans.get(i))
	      {
		BitSet candidates=computeCandidates(index,i);
#ifdef USELPFORDYN
		for(int j=0;j<candidates.size();j++)
		  if(candidates.get(j))
		    {
		      bool knownToBeInfeasibleLP=false;
		      bool knownToBeInfeasible=false;
		       
#if 1 //<-- enable simplex call here (to compute knowToBeInfeasibleLP)
      //(this chuck is used to debug "Kojima trick"; runs LP truncated at 40 steps) 
		      Matrix<LType> Inequalities;
		       //Test using LP
		      bool pushed=reducer.pushBLA(fans[i].cones[j].equationsL[0].toVector(),fans[i].cones[j].equationsR[0]);
		      if(pushed)
			{
			  int numberOfAddedInequalities=0;
			  if(index!=0)inequalityMatricesNumberOfUsedRows1[index]=numberOfAddedInequalities=reducer.singleReduction(inequalityMatricesL[index-1],inequalityMatricesR[index-1],inequalityMatricesNumberOfUsedRows2[index-1],inequalityMatricesL[index],inequalityMatricesR[index]);
			  if(numberOfAddedInequalities==-1)
			    knownToBeInfeasibleLP=true;
			  if(numberOfAddedInequalities>=0 && index!=0)//why sharp here and not in the other similar if statement?
			    //if(numberOfAddedInequalities>0)//why sharp here and not in the other similar if statement?
			    {
			      //LPExact lp(inequalityMatricesL[index]);
			      if(!fans[i].cones[j].hasPointWithLastCoordinatePositiveInCone
				 (inequalityMatricesL[index],inequalityMatricesR[index],
				  inequalityMatricesNumberOfUsedRows1[index],
				  inequalityMatricesNumberOfUsedRows2[index],
				  reducer,/*Inequalities,*/lpList[index],true,0/*false*/))
				{
				  knownToBeInfeasibleLP=true;
				}
			    }
			  reducer.pop();
			}
		      else
			{
			  knownToBeInfeasibleLP=true;
			}
#endif
		      //Test using Kojima et al trick
		      if(parentLP)
			{
			  //assert(0);
			  //cerr<<"A:"<<i<<j<<fans[i].coneNeighbours.size()<<endl;
			  int firstsecond[2];
      			  firstsecond[0]=fans[i].coneNeighbours[j].first;
			  firstsecond[1]=fans[i].coneNeighbours[j].second;
			  for(int k=0;!knownToBeInfeasible&&k<2;k++)//NOTEST
			    {
			      int A=fans[i].fullDimCones[firstsecond[k]].inequalitiesR.size();
			      Matrix<LType> inequalitiesL(A,fans[i].getAmbientDimension()-index-2);
			      Vector<RType> inequalitiesR(A);
			      int added=reducer.reduction(fans[i].fullDimCones[firstsecond[k]].inequalitiesL,
						fans[i].fullDimCones[firstsecond[k]].inequalitiesR,
						inequalitiesL,inequalitiesR,0);
			      if(added==-1)
				{
				  knownToBeInfeasible=true;
				  // assert(knownToBeInfeasibleLP); //<---- Code for debugging the Kojima test (computation of knowToBeInfeasibleLP must be enabled before enabling this test).
				}
			      else
			      for(int ii=0;!knownToBeInfeasible&&ii<added;ii++)
				if(parentLP->isUnboundedDirection(inequalitiesL,inequalitiesR,ii))
				  {
				    knownToBeInfeasible=true;
				    /* if(!knownToBeInfeasibleLP) //<---- Code for debugging the Kojima test (computation of knowToBeInfeasibleLP must be enabled before enabling this test).
				      {
					cerr<<"parentLP"<<(*parentLP);
					cerr<<"inequalitiesL"<<inequalitiesL;
					cerr<<"inequalitiesR"<<inequalitiesR;
					cerr<<"ii"<<ii<<endl;					
					cerr<<"Chosen fans vector: "<<chosenFans<<endl;
					cerr<<"\nChosen cone vector: "<<chosen<<endl;
					cerr<<fans[chosenFans[0]].cones[chosen[0]];
					cerr<<"Fan index:"<<i<<endl;
					cerr<<"Cone index:"<<j<<endl;
					cerr<<fans[i].cones[j];
				      }
				      assert(knownToBeInfeasibleLP);*/
				  }
			    }
			}
		      knownToBeInfeasible|=knownToBeInfeasibleLP;
		      if(knownToBeInfeasible)candidates.set(j,false);
		    }
#endif
		//		cerr<<candidates;
		int n=candidates.sizeOfSubset();
		if(n<=bestNumberOfCandidates)  //we could choose a strict inequality
		  {
		    bestNumberOfCandidates=n;
		    bestCandidates=candidates;
		    bestIndex=i;
		  }
	      }
	  }
	assert(bestIndex!=-1);
	BitSet candidates=bestCandidates;//computeCandidates(index,bestIndex);


	chosenFans[index]=bestIndex;
	usedFans.set(chosenFans[index],true);


	nCandidates[index]=bestNumberOfCandidates;//just for printing

	static int iterationNumber;
	if(0)	if(!(iterationNumber++ & (16*256-1)))
	  //	  log2
	  	{
	  fprintf(stderr,"Iteration level:%i, Chosen fan:%i, Number of candidates:%i, Iteration Number:%i, Useful (%i/%i)=%f\n",index,bestIndex,bestNumberOfCandidates,iterationNumber,numberOfUsefulCalls,totalNumberOfCalls,float(numberOfUsefulCalls)/totalNumberOfCalls);
	  cerr<<"Chosen fans vector: "<<chosenFans<<endl;
	  cerr<<"\nChosen cone vector: "<<chosen<<endl;
	  cerr<<"\nNcandidates vector: "<<nCandidates<<endl;
	  cerr<<"\nIterator vector:    "<<iterators<<endl;
	  fprintf(stderr,"\n\n");
	  }


	statistics.nMTKNodes+=fans[chosenFans[index]].size();
	for(int i=0;i<fans[chosenFans[index]].size();i++)
	  if(candidates.get(i))
	    {
	      bool ok=true;
	      for(int j=0;j<index;j++)
		{
		  if(table.intersectTriviallyInIntersection(chosenFans[j],chosen[j],chosenFans[index],i))
		    {
		      ok=false;
		      break;
		    }
		}
	      if(ok)
		{
		  statistics.nLPRunNodes++;
		  bool pushed=reducer.pushBLA(fans[chosenFans[index]].cones[i].equationsL[0].toVector(),fans[chosenFans[index]].cones[i].equationsR[0]
#if HASH
					      ,fans[chosenFans[index]].cones[i].equationsH[0]
#endif
);
		  if(pushed)
		    {
		      int numberOfAddedInequalities=0;
		      if(index!=0)
			{
			  //			  inequalityMatricesNumberOfUsedRows1[index]=numberOfAddedInequalities=reducer.singleReduction(inequalityMatricesL[index-1],inequalityMatricesR[index-1],inequalityMatricesNumberOfUsedRows2[index-1],inequalityMatricesL[index],inequalityMatricesR[index]);
			  inequalityMatricesNumberOfUsedRows1[index]=numberOfAddedInequalities=reducer.singleReductionMakeBasisFirst(inequalityMatricesL[index-1],inequalityMatricesR[index-1],

#if HASH
																     inequalityMatricesH[index-1],
#endif
																     inequalityMatricesNumberOfUsedRows2[index-1],inequalityMatricesL[index],inequalityMatricesR[index],
#if HASH
																     inequalityMatricesH[index],
#endif
/*OLDBASIS*/lpList[index-1].basis,/*OLDAINV*/lpList[index-1].Ainv,AinvList[index]);
			}
		      /*		      cerr<<inequalityMatricesL[index];
		      cerr<<inequalityMatricesNumberOfUsedRows1[index];
		      cerr<<inequalityMatricesH[index];
		      for(int j=0;j<inequalityMatricesH[index].size();j++)
			{
			  cerr<<inequalityMatricesL[index][j].toVector()
			      <<" \t"
			      <<inequalityMatricesH[index][j]
			      <<" \t"
			      <<hex<<*((int64*)&(inequalityMatricesH[index][j]))
			      <<" \t"
			      <<int(hash2(inequalityMatricesH[index][j]))<<dec
			      <<" ";
			  cerr<<endl;
			  }
			  cerr<<endl;*/
		      if(numberOfAddedInequalities>=0)
			if(fans[chosenFans[index]].cones[i].hasPointWithLastCoordinatePositiveInCone
			   (inequalityMatricesL[index],inequalityMatricesR[index],
#if HASH
			    inequalityMatricesH[index],
#endif
			    inequalityMatricesNumberOfUsedRows1[index],
			    inequalityMatricesNumberOfUsedRows2[index],
			    reducer,
			    lpList[index],false,(index!=0)?&(AinvList[index]):0))
			  {			    
#if CHECK			    
			    if(haveEmptyIntersection(current,fans[chosenFans[index]].cones[i],&reducer))
			      {
				cerr<<current;
				cerr<<fans[chosenFans[index]].cones[i];
				assert(0);
			      }
#endif
			    chosen[index]=i;
			    
			    //Cone<LType,RType>  next=intersection(current,fans[chosenFans[index]].cones[i]/*,true*/);
			    //if(index==3)next.removeRedundantInequalities();//<----------What is the best level for optimizing?
			    {
			      mixedVolumeAccumulator+=rek(index+1,/*next*/current,&(lpList[index]));
			    }
			    chosen[index]=-1;//just for printing
			  }
		      reducer.pop();
		    }
		  iterators[index]++;//just for printing
		}
	    }
	nCandidates[index]=-1;//just for printing
	iterators[index]=0;//just for printing

	usedFans.set(chosenFans[index],false);
	chosenFans[index]=-1;
      }
    if(mixedVolumeAccumulator>0)numberOfUsefulCalls++;
    return mixedVolumeAccumulator;
  }
};




vector<FanType> reduceDimension(int ambientDimension, vector<FanType> const &fans, int &numberOfRemovedDimensions, LType &mixedVolumeMultiplier)
{//Assuming subspaces are hyperplanes
  int numberOfSubspaces=0;
  for(int i=0;i<fans.size();i++)
    if(fans[i].cones.size()==1)numberOfSubspaces++;

  int I=0;
  Matrix<LType> equationsL(numberOfSubspaces,ambientDimension-1);
  Vector<RType> equationsR(numberOfSubspaces);
  for(int i=0;i<fans.size();i++)
    if(fans[i].cones.size()==1)
      {
	//	equations[I++].set(fans[i].cones[0].equations[0].toVector());
	equationsL[I].set(fans[i].cones[0].equationsL[0].toVector());
	equationsR[I++]=fans[i].cones[0].equationsR[0];
      }
  //  equations.cycleColumnsLeft(1);
  reducePair(equationsL,equationsR,false);

  cerr<<"EQUATIONS"<<equationsL<<equationsR;

  //  equations.reduce(false);
  int rank=equationsL.numberOfPivots();
  numberOfRemovedDimensions=rank;


  vector<FanType> ret;
  I=0;
  for(int i=0;i<fans.size();i++)
    if(fans[i].cones.size()!=1)
      {
	assert(fans[i].fullDimCones.size());

	//cerr<<fans[i];
	FanType newFan(ambientDimension);
	newFan.edges=Matrix<LType>(fans[i].edges.getHeight(),ambientDimension-1-rank);
	newFan.coneNeighbours=fans[i].coneNeighbours;

	//WE MUST ALSO UPDATE THE FULLDIMENSINALCONES
	for(int j=0;j<fans[i].fullDimCones.size();j++)
	  {
	    Matrix<LType> equations2L=fans[i].fullDimCones[j].equationsL;
	    Vector<RType> equations2R=fans[i].fullDimCones[j].equationsR;
	    Matrix<LType> inequalities2L=fans[i].fullDimCones[j].inequalitiesL;
	    Vector<RType> inequalities2R=fans[i].fullDimCones[j].inequalitiesR;
	    normalFormPairs(equations2L, equations2R, equations2L,equations2R, equationsL, equationsR);
	    normalFormPairs(inequalities2L, inequalities2R, inequalities2L,inequalities2R, equationsL, equationsR);
	    newFan.fullDimCones.push_back(Cone<LType,RType> (inequalities2L.getWidth()+1,inequalities2L,inequalities2R,equations2L,equations2R));
	  }

	for(int j=0;j<fans[i].cones.size();j++)
	  {
	    Matrix<LType> equations2L=fans[i].cones[j].equationsL;
	    Vector<RType> equations2R=fans[i].cones[j].equationsR;
	    Matrix<LType> inequalities2L=fans[i].cones[j].inequalitiesL;
	    Vector<RType> inequalities2R=fans[i].cones[j].inequalitiesR;
	    //	    equations2=equations.normalForms(equations2);
	    normalFormPairs(equations2L, equations2R, equations2L,equations2R, equationsL, equationsR);

	    //cerr<<inequalities2L.getWidth()<<(int)equationsL.getWidth();
	      //inequalities2=equations.normalForms(inequalities2);
	    normalFormPairs(inequalities2L, inequalities2R, inequalities2L,inequalities2R, equationsL, equationsR);

	    //	    newFan.cones.push_back(Cone(inequalities2.getWidth(),inequalities2,equations2));
	    newFan.cones.push_back(Cone<LType,RType> (inequalities2L.getWidth()+1,inequalities2L,inequalities2R,equations2L,equations2R));

	    //	    cerr<<newFan;assert(0);
	    //cerr<<fans[i].edges.getHeight();


	    // We compute the edge vector for the new system by reducing it modulo equationsL.
	    // That will not change the determinant of a chosen subset of the vectors.
	    // Since the pivots entries now are zero in the edge vector, we get the determinant
	    // as the minor times the pivot entries of equationsL. The absolute value of the
	    // product of the pivot entries is stored in mixedVolumeMultiplier.
	    newFan.edges[j].set(normalForm(fans[i].edges[j].toVector(),equationsL));
	  }
	ret.push_back(newFan);
      }
  //  for(vector<Fan>::const_iterator i=ret.begin();i!=ret.end();i++)cerr<<*i;

  mixedVolumeMultiplier=equationsL.productOfPivots();
  if(isNegative(mixedVolumeMultiplier))mixedVolumeMultiplier=-mixedVolumeMultiplier;

  return ret;
}

};

