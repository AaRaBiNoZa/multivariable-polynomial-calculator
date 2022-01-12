# multivariable-polynomial-calculator
This is a project prepared for University.

A more detailed describtion is in MainPage.dox

To run/test it on your local machine first clone the repository and then in the main directory run these commands:

For release version:
```
mkdir release
cd release
cmake ..
make
make doc
```

Debug version:

```
mkdir debug
cd debug
cmake -D CMAKE_BUILD_TYPE=Debug ..
make
make doc
```
