/****************************************
LLGMN�̎���

******************************************/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>

using namespace std;

//���t�f�[�^�N���X
class Tdata
{
public:
	vector<double> input;
	vector<double> output;
	Tdata(int input_size, int output_size);

private:

};

Tdata::Tdata(int input_size, int output_size)
{
	input.resize(input_size, 0);
	output.resize(output_size, 0);
}



int main() {
	
	return 0;
}

//�������`�d
void forwardprop(
	vector<vector<vector<double>>>& weight,		//�d��
	vector<vector<vector<double>>>& In,			//�e�j���[�����ɑ΂������
	vector<vector<vector<double>>>& Out,		//�e�j���[�����ɑ΂���o��
	vector<double>& input,		//NN�ւ̓���
	vector<double>& output,		//NN�ւ̏o��
	int input_size,				//���͂̌�(����)
	int k_class,				//�N���X
	int component				//�R���|�[�l���g(=�o�͂̌�)
) {
	int non_linear_input_size = 1 + input_size * (input_size + 3) / 2;	//����`�ϊ���̓��͂̌�
	vector<double> non_linear_input(non_linear_input_size);				//����`�ϊ���̓���
	int count1 = 0, count2 = 0;

	//���͂̔���`�ϊ�
	non_linear_input[0] = 1;
	for (int i = 0; i < input_size; i++) {
		non_linear_input[i + 1] = input[i];
	}
	for (int i = input_size + 1; i < non_linear_input_size; i++) {
		non_linear_input[i] = input[count1] * input[count2];
		count1++;
		if (count1 == input_size) {
			count2++;
			count1 = count2;
		}
	}

	//��1�w�̓`�d
	for (int i = 0; i < non_linear_input_size; i++) {
		In[0][0][i] = non_linear_input[i];
		Out[0][0][i] = In[0][0][i];
	}

}