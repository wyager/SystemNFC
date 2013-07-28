This is my attempt to make an implementation of basic libnfc functionality in C++. I was not happy with the reference C programs, so I wrote this for myself. I think this is substantially less spaghetti-coded than the reference implementations.

It doesn't have everything, but it's clear enough that it should be trivial to add more functionality from libnfc.

That said, I did violate a few coding rules, so this isn't exactly high-quality code. It is, however, quite functional.

The Classic card reading/writing is tested and seems to work fine.

The Ultralight card reading/writing has not been tested at all (I don't have the hardware yet). 

To build on a generic linux distro (like Debian on the Raspberry Pi), put all files in the same directory and run `bash build.sh`. You have to have libnfc installed, as well as g++-4.7. This works fine (albeit slowly) for me on my Raspberry Pi.

To build on OS X, just suck everything into Xcode and make sure you have the libnfc library and header files set up properly. 

For Windows and anyone else, just make sure you have a c++11 compiler and the libnfc library.