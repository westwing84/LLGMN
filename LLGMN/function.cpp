/****************************************
LLGMN�̎���
�֐�����
******************************************/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <cmath>
#include "tdata_class.h"
#include "function.h"

using namespace std;

//�������`�d
void forwardprop(
	vector<vector<vector<double>>>& weight,		//�d��
	vector<vector<vector<double>>>& In,			//�e�j���[�����ɑ΂������
	vector<vector<vector<double>>>& Out,		//�e�j���[�����ɑ΂���o��
	vector<double>& input,						//NN�ւ̓���
	vector<double>& non_linear_input,			//����`�ϊ���̓���
	vector<double>& output,		//NN����̏o��
	int input_size,				//���͂̌�(����)
	int k_class,				//�N���X(=�o�͂̌�)
	int component				//�R���|�[�l���g
) {
	int non_linear_input_size = 1 + input_size * (input_size + 3) / 2;	//����`�ϊ���̓��͂̌�
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
		In[0][i][0] = non_linear_input[i];
		Out[0][i][0] = In[0][i][0];
	}


	//��2�w�̓`�d
	for (int h = 0; h < non_linear_input_size; h++) {
		weight[h][k_class - 1][component - 1] = 0;	//��2�w�̈�ԍŌ�̃j���[�����ɑ΂���d�݂�0�Ƃ���
	}

	//2�w�ړ��͂̌v�Z
	for (int k = 0; k < k_class; k++) {
		for (int m = 0; m < component; m++) {
			In[1][k][m] = 0;
			for (int h = 0; h < non_linear_input_size; h++) {
				In[1][k][m] += Out[0][h][0] * weight[h][k][m];
			}
		}
	}

	//2�w�ڏo�͂̌v�Z
	double sum = 0;
	for (int k = 0; k < k_class; k++) {
		for (int m = 0; m < component; m++) {
			sum += exp(In[1][k][m]);
		}
	}
	for (int k = 0; k < k_class; k++) {
		for (int m = 0; m < component; m++) {
			Out[1][k][m] = exp(In[1][k][m]) / sum;
		}
	}


	//��3�w�̓`�d
	for (int k = 0; k < k_class; k++) {
		In[2][k][0] = 0;
		for (int m = 0; m < component; m++) {
			In[2][k][0] += Out[1][k][m];
		}
		output[k] = In[2][k][0];
	}
}


//�w�K
void learning(
	vector<Tdata> teaching_data,				//���t�f�[�^
	vector<vector<vector<double>>>& weight,		//�d��
	vector<vector<vector<double>>>& In,			//�e�j���[�����ɑ΂������
	vector<vector<vector<vector<double>>>>& Out,//�e�j���[�����ɑ΂���o��
	vector<vector<double>>& non_linear_input,	//����`�ϊ���̓���
	vector<vector<double>>& output,				//NN����̏o��
	int teaching_data_size,		//���t�f�[�^�̐�
	int input_size,				//���͂̌�(����)
	int k_class,				//�N���X(=�o�͂̌�)
	int component,				//�R���|�[�l���g
	double learning_rate,		//�w�K��
	int learning_times,			//�ő�w�K��
	double efunc_min,			//�]���֐��̎�������l
	int mode					//BATCH: �ꊇ�w�K�CSEQUENTIAL: �����w�K
) {
	int non_linear_input_size = 1 + input_size * (input_size + 3) / 2;	//����`�ϊ���̓��͂̌�
	double evaluation_func;		//�]���֐��̒l
	double diff_efunc;			//�]���֐��̔���
	double delta_weight;		//�d�݂̍X�V�l

	for (int times = 0; times < learning_times; times++) {
		//�e���t�f�[�^�ɑ΂��ď������`�d���s���C�o�͂𓾂�
		for (int tdata_num = 0; tdata_num < teaching_data_size; tdata_num++) {
			forwardprop(weight, In, Out[tdata_num], teaching_data[tdata_num].input, non_linear_input[tdata_num], output[tdata_num], input_size, k_class, component);
		}

		//�]���֐��̓��o
		evaluation_func = 0;
		for (int n = 0; n < teaching_data_size; n++) {
			for (int k = 0; k < k_class; k++) {
				evaluation_func -= teaching_data[n].output[k] * log10(output[n][k]);
			}
		}
		evaluation_func = abs(evaluation_func) / teaching_data_size;
		//if (times % 10 == 0) cout << evaluation_func << endl;
		if (abs(evaluation_func) < efunc_min) break;	//��������

		//�d�݂̍X�V
		for (int h = 0; h < non_linear_input_size; h++) {
			for (int k = 0; k < k_class; k++) {
				for (int m = 0; m < component; m++) {
					if (mode == BATCH) {	//�ꊇ�w�K
						delta_weight = 0;
						for (int n = 0; n < teaching_data_size; n++) {
							diff_efunc = (output[n][k] - teaching_data[n].output[k]) * Out[n][1][k][m] / output[n][k] * non_linear_input[n][h];
							delta_weight += diff_efunc;
						}
						delta_weight *= -learning_rate;
						weight[h][k][m] += delta_weight;
					}
					else {	//�����w�K
						for (int n = 0; n < teaching_data_size; n++) {
							diff_efunc = (output[n][k] - teaching_data[n].output[k]) * Out[n][1][k][m] / output[n][k] * non_linear_input[n][h];
							delta_weight = diff_efunc;
							delta_weight *= -learning_rate;
							weight[h][k][m] += delta_weight;
						}
					}
				}
			}
		}
	}
}


//�d�݂̏�����
void init_weight(vector<vector<vector<double>>>& weight) {
	random_device rnd;
	mt19937 mt(rnd());
	uniform_real_distribution<> rand12(-1.0, 1.0);	//-1�`1�͈̔͂̈�l����

	for (int i = 0; i < (int)(weight.size()); i++) {
		for (int j = 0; j < (int)(weight[i].size()); j++) {
			for (int k = 0; k < (int)(weight[i][j].size()); k++) {
				weight[i][j][k] = rand12(mt);
			}
		}
	}
}