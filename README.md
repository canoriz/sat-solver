# sat-solver
A conflict driven clause learning based boolean satfiability problem (SAT) solver and checker in C++.

## Introduction
This is a course project of data structure class at Huazhong University of Science and Technology.
The course project is intended to implement a DPLL based SAT solver, but I believe this project is far beyond the requirements.

I have implemented conflict driven clause learning (CDCL), clause deletion, and restart strategy to make DPLL faster.

To verify the answers, I also made a answer judger, called checker.

This project included a binary puzzle solver using SAT solver, which is also a requirement of my course.

## Usage

### compilation:
```
make
```
The ```solver``` is the SAT solver.

The ```checker``` is the answer checker.

### Examples
Run 
```
./solver cnf tests/satisfied/S/problem3-100.cnf
```
and the answer will be at
```
tests/satisfied/S/problem3-100.res
```

Check answers
```
./checker tests/satisfied/S/problem3-100.res
```
if the output is ```1``` then the answer is corrent.


### Solve SAT problem
```./solver cnf path-to-cnf-file```

### Check SAT problem answers
``` ./checker path-to-cnf-file```

### Play binary puzzle
```./solver sud N```, in which ```N``` is an arbitrary positive even number.

This will generate a unique answer random N times N binary puzzle and solve it.

For more information about binary puzzle , see (binarypuzzle.com)[https://binarypuzzle.com/].

## Data formats
The SAT problem data and answer is described in the DIMACS format. A simple explanation is at 
[satcompetitions.org](http://www.satcompetition.org/2004/format-solvers2004.html).

## Note
The answer to one ```whatever-name.cnf``` file will be stored in ```whatever-name.res``` in the same path of ```.cnf``` file.

The solver should run before checker, so the checker can check the answers.

Due to the requirements of my instructor, the course project should not include any Standard Template Library (STL). 
In order to use some useful structures, I have implemented my own versions of a simple ```vector``` and hash table.
You can substitute my version for a C++ library version.

## References
I read many documents to complete my SAT solver.

One is MiniSAT's (paper)[http://minisat.se/Papers.html].
