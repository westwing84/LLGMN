#pragma once
#include <iostream>
#include <vector>

using namespace std;


//���t�f�[�^�N���X
class Tdata
{
public:
	vector<double> input;		//����
	vector<double> output;		//�o��
	Tdata(int input_size, int output_size);
};

//�R���X�g���N�^
Tdata::Tdata(int input_size, int output_size)
{
	input.resize(input_size, 0);
	output.resize(output_size, 0);
}