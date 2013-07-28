This is my attempt to make an implementation of basic libnfc functionality in C++. I was not happy with the reference C programs, so I wrote this for myself. I think this is substantially less spaghetti-coded than the reference implementations.

It doesn't have everything, but it's clear enough that it should be trivial to add more functionality from libnfc.

That said, I did violate a few coding rules, so this isn't exactly high-quality code. It's just good enough that I'm probably not going to dump more time into it.

The Classic card reading/writing is tested and seems to work fine.

The Ultralight card reading/writing has not been tested at all (I don't have the hardware yet). 