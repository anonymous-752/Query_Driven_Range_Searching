# Partition Trees

---

The implementations of the required classes can be found in the files `bptree.h`, `minspantree.h`, `nn.h`, `point.h` and `query.h`.

The experiment evaluating the accuracy of the Neural Networks is located in `testNeural.cpp` (Subsection 5.2).

File `train.cpp` contains the code for constructing the trees and training the Neural Networks within their inner nodes.

The testing of the trees (producing query answers and measuring query times) is in `testtree.cpp` (Subsection 5.4).

Finally, file `findacc.cpp` contains the code that takes a result file and computes the accuracy of the tree answers (Subsection 5.4).

To run `testNeural.cpp`, `train.cpp` and `testtree.cpp`, update 'file' in the Makefile to match the desired file. Then, run `make` that generates the executable. The executables take a command line argument, the size of the query sample. File `findacc.cpp` can be compiled with just `g++`.
