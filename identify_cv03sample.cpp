#include"identify_cv03class.h"
#include<iostream>
#include<fstream>
using namespace std;
int main()
{
	char in_file[]="test.in";
	char out_file[]="test_res.out";
	Img_segment7 s;
	s.init();
	s.input_file(in_file);
	s.reduce_niose();
	s.cv_identify();
	s.output_result(out_file);
	return 0;
}
