//�֐��̃v���g�^�C�v�錾

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <cmath>
#include "tdata_class.h"

using namespace std;

static const int BATCH = 1, SEQUENTIAL = 2;	//BATCH: �ꊇ�w�K�CSEQUENTIAL: �����w�K

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
);

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
	int learning_times,			//�ő�w�K��
	double efunc_min,			//�]���֐��̎�������l
	double beta,				//TA�̒萔
	int mode					//BATCH: �ꊇ�w�K�CSEQUENTIAL: �����w�K
);

//�d�݂̏�����
void init_weight(vector<vector<vector<double>>>& weight);