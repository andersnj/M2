Near future agenda
------------------
eliminate dependence on ReducerExact by templating

* functions (non-class methods)
* classes LP, Cone, Fan  Done! -but cannot make haveEmptyIntersection a friend of Cone template 

Compilation
-----------
To compile, go to src/ and run

  g++ -O3 mixed-cells.cpp -lgmp -D<LTYPE_RTYPE> -DHASH=1
where 

  LTYPE_RTYPE is in {DOUBLE_DOUBLE, GMPRAT_GMPRAT, SHORTRAT_SHORTRAT}

  HASH determines whether a lookup table for conditions with matching RHSs is used

  USELPFORDYN, if set then "Kojima trick" is enabled

for example
  g++ -O3 mixed-cells.cpp -lgmp -DDOUBLE_DOUBLE -DHASH=1


Then run
time ./a.out ../demicsExamples/cyclic10.dat
to test the program.

To get stack traces when assertion fails, compile with
g++ -O3 mixed-cells.cpp -lgmp -DDOUBLE_DOUBLE -DHASH=1 -D__assert_fail=__assert_fail2 -DSTACKDUMP_ENABLED -rdynamic

To profile
g++ -O3 mixed-cells.cpp -pg
./a.out ../demicsExamples/cyclic11.dat
gprof >gprof.output 

To profile with oprofile compile with
g++ -O3 -DNDEBUG mixed-cells.cpp -lgmp -DDOUBLE_DOUBLE -DHASH=1 -D__assert_fail=__assert_fail2 -DSTACKDUMP_ENABLED -rdynamic

To profile with oprofile
sudo opcontrol --deinit
sudo opcontrol --init
sudo opcontrol --reset
sudo opcontrol --callgraph=40
sudo opcontrol --start
sudo opcontrol --stop
sudo opcontrol --dump
opreport -cgf | gprof2dot.py -f oprofile -s | dot -Tpng -o output.png

How to use etags:

run
etags *.cpp *.h
in the shell.
Then in emacs place the cursor on a variable or type name and 
press META "." to jump to the first occurence 
press CTRL-u META-"." to jump to the next occurence

Mysterious symbols:

  Ainv is the inverse of a (square) _submatrix_ of A
  Ainvw = Ainv*w 


Things we could do to improve the code
--------------------------------------
* Record and use inconsistency conditions dynamically (at the moment inconsistent "edge pairs" are precomputed)

* Use a heuristic a la Kojima to select the next polytope; this can be non-exact.

* Have an exact field, which stores its elements in words when they fit. For example using the gfan rationals in development.

* Do LP solving with doubles, and transform certificates to exact arithmetics.

* Increase perturbations precision dynamically.


Primal and duals
----------------

We want to solve LP systems of the form
Primal:
Ax\leq b

where the objective function is not specified.

The dual system looks like this:
min <b.y>
st. yA=gamma
y>=0

Here we are free to choose the gamma. We do so such that the system is always feasible.

The duality theorem of linear programming says:
Primal feasible,unbounded     <=> 	  Dual infeasible
Primal feasible,bounded	      <=>	  Dual feasible,bounded
Primal infeasible	      <=>	  Dual feasible,unbounded

Here the first option is never the case for our systems.

Proof of easy direction: Dual feasible,unbounded => Primal infeasible
Suppose y is a vertex, and z is the unbounded direction.
Then we have (y+z)A=gamma and yA=gamma, implying zA=0. Furthermore z is improving
meaning zb<0.
In the primal we are looking for x st. Ax<=b.
If we have such and x, then 0=zAz<=zb<0 which is a contradiction.

Recovering certificates
-----------------------
In case dual is unbounded, then we get a floating point representation of the improving ray z.
We convert this to integer, and check that zA=0, and zb<0 in exact arithmetics.
This will show that the primal is infeasible.

If on the other hand we are in an optimal vertex of the polytope, then we take a submatrix A' and
a subvector b' according to the active set of constraints.
Solving A'x=b' gives us the solution x. We solve this by row reducing [A'|b'] in exact arithmetic.
After this we substitute the exact solution into the primal, and check in exact arithmetics that
this is indeed a solution to the primal system.

Doxygen
-------

doxygen
