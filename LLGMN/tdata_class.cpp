//���t�f�[�^�N���X�̎���

#include <iostream>
#include <vector>
#include "tdata_class.h"

using namespace std;

//���t�f�[�^�N���X�̃R���X�g���N�^
Tdata::Tdata(int input_size, int output_size)
{
	input.resize(input_size, 0);
	output.resize(output_size, 0);
}