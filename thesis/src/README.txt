Readme for cloth and fluid simulation

Cloth:
The basic implementation took approximately 3 hours to
complete.  Once this had been finished, gouraud shading
was added to improve the appearance of the simulation.
This addition took about an hour to implement.

A higher degree of accuracy was desired as well as a
speed increase, so 4th-order Runge-Kutta solving was
implemented.  This did not perform as well as had been
hoped, so the solver was reduced to 2nd-order Runge-Kutta
(midpoint).  It was hoped that implicit Euler integration
could be implemented, but there was not time to finish
that before the deadline.

Several higher-resolution tests were created to test the
speed limits of this program, the most interesting of which
is named table_cloth_new.txt, has nothing to do with a
tablecloth, and should be run with a timestep of 0.1-0.15.
(This test remains stable up to about 0.7, but any
semblance of realism vanishes by 0.25 or so.)


Fluid:
The trilinear interpolation took a few hours to implement.
The incompressibility code (ignoring air) took an hour or
two.  Many hours were spent trying to get the tests with
air working, but in the end it was decided that enough
work had been done and that while this may be pursued
outside of the scope of this class, all the work that
would be done for this project had been done.
The program works fine for tests that are all fluid,
but breaks down when there is air in the system.

A test was created (fluid_deep_spiral.txt) to further test
airless simulation.  This test runs nicely and creates
some interesting convection in the fluid, as well as
demonstrating that while this program does not properly 
simulate fluid with air, it is quite robust in simulations
without air.
