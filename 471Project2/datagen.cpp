#include <fstream.h>
#include <iostream.h>
#include <iomanip.h>
#include <cstdlib>
#include <stdlib.h>
#include <math.h>

int main()
{
     float temp, sum=0.0; int knt=0;
    // Declare input and output files her
    fstream output;
    int i;
    // Open the two file
    output.open("testfile.dat", ios::out);
    // Read the first record of teh input file
  for (i=0; i < 300; i++) output << (int)((float)rand()/RAND_MAX*10000.0) << endl;
 for (i=0; i < 300; i++) output << 70000+(int)((float)rand()/RAND_MAX*10000.0) << endl; 
  for (i=0; i < 300; i++) output << 10000+(int)((float)rand()/RAND_MAX*10000.0) << endl; 
  for (i=0; i < 300; i++) output << 20000+(int)((float)rand()/RAND_MAX*10000.0) << endl; 
  for (i=0; i < 300; i++) output << 90000+(int)((float)rand()/RAND_MAX*10000.0) << endl; 
  for (i=0; i < 300; i++) output << 40000+(int)((float)rand()/RAND_MAX*10000.0) << endl; 
  for (i=0; i < 300; i++) output << 50000+(int)((float)rand()/RAND_MAX*10000.0) << endl; 
   for (i=0; i < 300; i++) output << 80000+(int)((float)rand()/RAND_MAX*10000.0) << endl; 
  for (i=0; i < 300; i++) output << 60000+(int)((float)rand()/RAND_MAX*10000.0) << endl; 
  
  for (i=0; i < 300; i++) output << 30000+(int)((float)rand()/RAND_MAX*10000.0) << endl; 
 
  
  
   output.close();  // Close the output file
   system("pause");
return 0;
}
