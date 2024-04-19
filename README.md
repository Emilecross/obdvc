Instructions:
Run "make" to create the executable.

This will create a file: main

Run as follows: cat <input_source> | ./main <depth> [> <output_destination>]

Some notes:

OrderBook.hpp is the version that it is using at the moment. This walks the table on add, update, delete, execute
This one is slow as it walks the table every time.

OrderBook2.hpp is not working and segfaults. It aimed to keep track of the Nth iterator (where N = min(depth, mapSize)).
I did not want to use std::distance as it is linear in time. I wanted something that is constant time so I used prices to compare. However Buy and Sell maps order by price differently so this code needs to be refactored to get it to work properly.

In hindsight I should have just use +ve and -ve values for the maps instead to work around using so much templating and issues
like the iterator logic.