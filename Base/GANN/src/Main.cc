#include <vector>
#include <iostream>
#include <time.h> 

#include "GeneticAlgorithm.h"

int main()
{
    srand((unsigned)time(NULL));

    GA ga;
    int trainingPass = 0;

    while (trainingPass < 200)
    {
        trainingPass++;

        ga.Epoch();
    }

    ga.TestFittestGenome();

    int a;
    std::cin >> a;

    return 0;
}