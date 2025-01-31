# Ring Trees

---

The implementations of the required classes can be found in the files `point.h`, `separator.h` and `tree.h`.

File `experiment.cpp` contains the code for constructing the trees and finding the separators within their inner nodes.

The queries used are located in zip file `queries.zip`.

The testing of the trees (measuring query times) is in `testq.cpp` (Subsection 5.4).

To run `experiment.cpp` and `testq.cpp`, update 'file' in the `Makefile` to match the desired file. Then, run `make` that generates the executable. The executables take a command line argument, the size of the query sample.
