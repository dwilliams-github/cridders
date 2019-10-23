# Cridders

A artificial life simulation, written for MacOS, around April 1999.
The development environment was CodeWarrior. Using c++ for MacOS
projects was bleeding edge (the standard language at that point
was Pascal). You can see comments created by a starting template
in the main executable source file.

Each animal had a set of "genes" that determined its behavior.
Animals are paired off to create children, which inherit some
random combination of the traits of their parents. I think I also
had random mutations tossed in for good measure.

Animals can attack and eat other animals. For survival, you either
ate your neighbors, or learned to avoid them. I think I was hoping
that different populations of predators and herbivores would spontaneously
emerge.

There is also the option of editing the landscape to put up barriers
of a sort between populations, to restrict movement to a degree.

It looks like the parameters were hard coded, since I didn't provide
any advanced interface.