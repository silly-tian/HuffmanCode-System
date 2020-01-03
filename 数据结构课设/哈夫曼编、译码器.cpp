//
//  哈夫曼编/译码器
//
// 问题描述
//
//   利用哈夫曼编码进行信息通信可以大大提高信道利用率，
//   缩短信息传输时间，降低传输成本。但是，
//   这要求在发送端通过一个编码系统对待传数据预先编码，
//   在接收端将传来的数据进行译码（复原）。
//   试为这样的信息收发站写一个哈夫曼的编/译码器。
//
// 设计要求
//
//   建立哈夫曼树：读入文件(*.souce)，统计文件中字符出现的频度，
//   并以这些字符的频度作为权值，建立哈夫曼树。
//   编码：利用已建立好的哈夫曼树，获得各个字符的哈夫曼编码，
//   并对正文进行编码，然后输出编码结果，并存入文件(*.code)中。
//   译码：利用已建立好的哈夫曼树将文件(*.code)中的代码进行译码，
//   并输出译码结果，并存入文件(*.decode)中。
//   利用位操作，实现文件的压缩与解压。（选作）

#define _CRT_SECURE_NO_WARNINGS
//#define N 128
//#define M 2 * N - 1
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

////哈弗曼树的结构体数组
//typedef struct
//{
//	int Weight;
//	int Parent, Lchild, Rchild;
//}HTNode, HuffmanTree[M + 1];
//
//// 存储哈夫曼编码
//typedef char * Huffmancode[N + 1];

//哈弗曼树的结构体数组
struct HuffmanTree
{
	int Weight;
	char Ch;
	int Parent, Lchild, Rchild;
};

int MeasuringFrequency(int frequency[], char name[]);
void IniHuffmanTree(HuffmanTree ht[], int frequency[], int count);
void CrtHuffmanTree(HuffmanTree ht[], int count);
void CrtHuffmanCode(HuffmanTree ht[], char hc[][128], int count);
void FileCode(HuffmanTree ht[], char hc[][128], char souce_name[]);
void FileDecode(HuffmanTree ht[], int count);

// 统计各个字符出现的频率
int MeasuringFrequency(int frequency[], char souce_name[])
{
	int ch;
	int count1 = 0;
	int count2 = 0;
	printf("请输入文件名(需要手动加入后缀名.souce): ");
	scanf("%s", souce_name);
	FILE *fp = fopen(souce_name, "r");
	if (fp == NULL)
	{
		printf("文件打开失败!!!\n");
		return -1;
	}
	while ((ch = fgetc(fp)) != EOF)
	{
		if (frequency[ch] == 0)
		{
			count1++;
		}
		frequency[ch]++;
		count2++;
	}
	printf("计算字符频率成功，共有字符%d个，其中包含英文字符%d种", count2, count1);
	fclose(fp);
	return count1;
}

// 初始化哈弗曼树
void IniHuffmanTree(HuffmanTree ht[], int frequency[], int count)
{
	int i;
	int j = 1;
	for (i = 0; i < 128; i++)
	{
		if (frequency[i] == 0)
		{
			continue;
		}
		ht[j].Weight = frequency[i];
		ht[j].Ch = i;
		ht[j].Parent = 0;
		ht[j].Lchild = 0;
		ht[j].Rchild = 0;
		j++;
	}
	for (; j < 2 * count - 1 + 1; j++)
	{
		ht[j].Ch = 0;
		ht[j].Weight = 0;
		ht[j].Parent = 0;
		ht[j].Lchild = 0;
		ht[j].Rchild = 0;
	}
	//for (i = 1; i < count + 1; i++) //遍历初始的结点(有权值)
	//{
	//	ht[i].Weight = frequency[i - 1];
	//	ht[i].Parent = 0;
	//	ht[i].Lchild = 0; 
	//	ht[i].Rchild = 0;
	//}
	//for (i = N + 1; i < 2 * count - 1 + 1; i++) // 遍历剩下的结点(没有权值)
	//{
		//ht[i].Weight = 0;
		//ht[i].Parent = 0;
		//ht[i].Lchild = 0;
		//ht[i].Rchild = 0;
	//}
}

// 创建哈夫曼树
void CrtHuffmanTree(HuffmanTree ht[], int count)
{
	int i;
	int j;
	int min1 = 2147483647; // 定义成int的最大值
	int min2 = 2147483647;
	int min1_i = 1;
	int min2_i = 1;
	for (i = count + 1; i < count * 2 - 1 + 1; i++) // 循环后面的结点
	{
		min1 = 2147483647;
		min2 = 2147483647;

		//寻找最小值和次小值
		for (j = 1; j < i; j++)
		{
			if (min1 > ht[j].Weight && ht[j].Parent == 0)
			{
				min2 = min1;
				min2_i = min1_i;
				min1 = ht[j].Weight;
				min1_i = j;
			}
			else if (min2 > ht[j].Weight && ht[j].Parent == 0)
			{
				min2 = ht[j].Weight;
				min2_i = j;
			}
		}

		//设置父母结点和孩子结点和权值
		ht[i].Lchild = min1_i;
		ht[i].Rchild = min2_i;
		ht[i].Weight = min1 + min2;
		ht[min1_i].Parent = i;
		ht[min2_i].Parent = i;
	}
}

// 哈弗曼树编码
void CrtHuffmanCode(HuffmanTree ht[], char hc[][128], int count)
{
	int i;
	char *cd; // 临时存储编码
	int start;
	int c; //自身结点
	int p;  // 父母结点
	cd = (char *)malloc(count * sizeof(char));
	cd[count - 1] = '\0';
	for (i = 1; i <= count; i++)
	{
		start = count - 1;
		c = i;
		p = ht[i].Parent;
		// 倒着存储编码在临时的cd里
		while (p != 0)
		{
			start--;
			if (ht[p].Lchild == c)
			{
				cd[start] = '0';
			}
			else
			{
				cd[start] = '1';
			}
			c = p;
			p = ht[p].Parent;
		}
		// 将临时的cd放在hc里
		strcpy(hc[ht[i].Ch], &cd[start]);
	}
	free(cd); // 释放临时
}

// 对正文进行编码，然后输出编码结果，并存入文件(*.code)中
void FileCode(HuffmanTree ht[], char hc[][128], char souce_name[])
{
	int ch;
	char code_name[260];
	int i;
	for (i = 0; i < 260; i++)
	{
		code_name[i] = souce_name[i];
		if (souce_name[i] == '.')
		{
			i++;
			break;
		}
	}
	code_name[i++] = 'c';
	code_name[i++] = 'o';
	code_name[i++] = 'd';
	code_name[i++] = 'e';
	code_name[i++] = '\0';

	FILE *fp_souce = fopen(souce_name, "r");
	FILE *fp_code = fopen(code_name, "wb");
	if (fp_souce == NULL)
	{
		printf("文件打开失败!!!\n");
		return;
	}
	printf("\n编译好的结果为:");
	while ((ch = fgetc(fp_souce)) != EOF)
	{
		printf("%s", hc[ch]);
		fputs(hc[ch], fp_code);
	}
	printf("\n编译成功!\n");
	fclose(fp_souce);
	fclose(fp_code);
	return;
}


// 译码：利用已建立好的哈夫曼树将文件(*.code)中的代码进行译码，
// 并输出译码结果，并存入文件(*.decode)中。
void FileDecode(HuffmanTree ht[], int count)
{
	int ch;
	char code_name[260];
	char decode_name[260];
	printf("请输入想要译码的文件名(需要手动加入后缀名.code): ");
	scanf("%s", code_name);
	int i;
	for (i = 0; i < 260; i++)
	{
		decode_name[i] = code_name[i];
		if (code_name[i] == '.')
		{
			i++;
			break;
		}
	}
	decode_name[i++] = 'd';
	decode_name[i++] = 'e';
	decode_name[i++] = 'c';
	decode_name[i++] = 'o';
	decode_name[i++] = 'd';
	decode_name[i++] = 'e';
	decode_name[i++] = '\0';
	FILE *fp_code = fopen(code_name, "rb");
	FILE *fp_decode = fopen(decode_name, "w");
	if (fp_code == NULL)
	{
		printf("文件打开失败!!!\n");
		return;
	}
	i = count * 2 - 1 + 1 - 1;
	printf("\n译码结果为: ");
	while ((ch = fgetc(fp_code)) != EOF)
	{
		if (i > 0 && i < count + 1)
		{
			printf("%c", ht[i].Ch);
			fputc(ht[i].Ch, fp_decode);
			i = count * 2 - 1 + 1 - 1;
		}
		if (ch == '0')
		{
			i = ht[i].Lchild;
		}
		else
		{
			i = ht[i].Rchild;
		}
	}
	if (i > 0 && i < count + 1)
	{
		printf("%c", ht[i].Ch);
		fputc(ht[i].Ch, fp_decode);
		i = count * 2 - 1 + 1 - 1;
	}
	printf("\n译码成功!\n");
	fclose(fp_code);
	fclose(fp_decode);
	//printf("计算字符频率成功，共有字符%d个，其中包含英文字符%d种", count2, count1);
	//fclose(fp);
	return;
}

int main()
{
	char souce_name[260];
	int frequency[128] = { 0 };
	int count = MeasuringFrequency(frequency, souce_name);
	HuffmanTree *ht = (HuffmanTree *)malloc(sizeof(HuffmanTree) * count * 2 - 1 + 1);
	char hc[128][128];
	IniHuffmanTree(ht, frequency, count); // 初始化哈弗曼树
	CrtHuffmanTree(ht, count); // 创造哈夫曼树
	CrtHuffmanCode(ht, hc, count); // 哈弗曼树编码
	FileCode(ht, hc, souce_name); // 对正文进行编码，然后输出编码结果，并存入文件(*.code)中
	FileDecode(ht, count); // 译码：利用已建立好的哈夫曼树将文件(*.code)中的代码进行译码，
						   // 并输出译码结果，并存入文件(*.decode)中。

	system("pause");
	return 0;
}