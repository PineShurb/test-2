﻿// test 2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

typedef unsigned          char uint8_t;


typedef  struct SETTING_PARAMS {
	float		stard[20];
	float		cable[20];

}SettingParams_t;

typedef struct
{
	float	distance[20];
	float	velocity[20];
	uint8_t	check_in[20];
	uint8_t count;
}Temp_flow_t;


typedef  struct DDMSJ_INFO {
	char  isValid;                      //

	char  DDMSJ_Hydrological_Station[9];//Ë®ÎÄÕ¾±àÂë Ë®ÎÄÕ¾±àÂë8Î»×Ö·û,¶àÉêÇë1byte´æ´¢×Ö·û´®½áÊøÎ»
	char  DDMSJ_Section_Code[5];        //¶ÏÃæ±àÂë
	char  DDMSJ_Measure_Platform[2];    //²âÁ¿Æ½Ì¨
	char  DDMSJ_DateTime[11];           //Ê±¼ä Àý:2019-01-02  ¶àÉêÇë1byte´æ´¢×Ö·û´®½áÊøÎ»
	char  DDMSJ_Measure_Num;           //²âÁ¿µãÊý 1~32
	float DDMSJ_Data[32][2];            //Æðµã¾à ¸ß³Ì
}DDMSJ_t;

typedef struct
{
	double	flow;
	uint8_t	Wind_Dir;
	uint8_t Wind_Power;
	float	distance[20];
	float	velocity[20];
	float	waterdepth[20];	//Ë®Î»
	uint8_t	use[20];
	uint8_t	check_in[20];
	uint8_t count;

	float left;			//×óË®±ß
	float right;		//ÓÒË®±ß
	float CurrentArea;	//Ë®Ãæ»ý
	float fWaterWide;	//Ë®Ãæ¿í¶È
	float midwater;		//Æ½¾ùË®Î»
	float maxveloc;		//×î´óÁ÷ËÙ
						//	float velocit;
	float area;
}MeausreFlowResult_t;
typedef  struct
{
	MeausreFlowResult_t	flowresult;		//´¹Ïß´¦ÀíºóÊý¾Ý±£´æÎ»ÖÃ

}Measure_t;
Temp_flow_t		xtemp;
SettingParams_t   xSettingParams;
Measure_t    xMeasure;
DDMSJ_t     xDdmsj;

/*
float getRopeLongByDistance(float distance)
{
	float ropeLong = distance;
	for (int i = 0; i < CableLevelCorrectionPairList.Count; ++i)
	{
		if (CableLevelCorrectionPairList[i].distance <= distance && i + 1 < CableLevelCorrectionPairList.Count && CableLevelCorrectionPairList[i + 1].distance >= distance)
		{
			float dt = (CableLevelCorrectionPairList[i + 1].ropeLong - CableLevelCorrectionPairList[i].ropeLong);
			if (dt == 0)
			{
				dt = 1;
			}
			float cosValue = (CableLevelCorrectionPairList[i + 1].distance - CableLevelCorrectionPairList[i].distance) / dt;
			if (cosValue != 0)
			{
				ropeLong = (distance - CableLevelCorrectionPairList[i].distance) / cosValue + CableLevelCorrectionPairList[i].ropeLong;
			}
			else
			{
				ropeLong = 0;
			}
			break;
		}
		else if (CableLevelCorrectionPairList[i].distance <= distance && i + 1 >= CableLevelCorrectionPairList.Count)
		{
			ropeLong = (distance - CableLevelCorrectionPairList[i].distance) + CableLevelCorrectionPairList[i].ropeLong;
			break;
		}
	}
	return ropeLong;
}*/

float StartDistance_Compensate(float dis)
{
	float a, b, m, n;
	uint8_t i = 0,j=0;

	for (i = 0;i<5;i++)
	{
		if (dis == xSettingParams.cable[i])
			return xSettingParams.stard[i];
	}
	for (i = 0;i<4;i++)
	{
		if ((dis<xSettingParams.cable[i + 1]) && (dis>xSettingParams.cable[i]))
		{
			a = xSettingParams.cable[i];
			b = xSettingParams.cable[i + 1];
			m = xSettingParams.stard[i];
			n = xSettingParams.stard[i + 1];

			if (a != b)
			{
				dis = (dis - a)*(n - m) / (b - a) + m;
				return dis;
			}
		}
		if ((i == 0) || (xSettingParams.cable[i] > 0))
		{
			j = i;
		}
		else
			break;
	}
	dis = dis  + xSettingParams.stard[j]- xSettingParams.cable[j];

	return dis;
}

void Cal_Water_shuibian(DDMSJ_t *pDdmsj, float Water_Level)
{
	float temparea;
	int i,j, k,l;
	k = 0;

	do {
		float cur_line = 0;
		if (xMeasure.flowresult.CurrentArea == 0)	//水流面积为0时，流量为0
		{
			xMeasure.flowresult.flow = 0;
			break;
		}

		xtemp.count = 0;
		//排序
		for (i = 0;i < 20;i++)
		{
			if (xMeasure.flowresult.check_in[i] > 0)
			{
				if (xtemp.count == 0)
				{
					xtemp.distance[0] = xMeasure.flowresult.distance[i];
					xtemp.velocity[0] = xMeasure.flowresult.velocity[i];
					xtemp.count = 1;
				}
				else {
					for (k = 0;k < xtemp.count;k++)
					{
						if (xMeasure.flowresult.distance[i] < xtemp.distance[k])
						{
							for (l = xtemp.count - 1;l >= k;l--)
							{
								xtemp.distance[l + 1] = xtemp.distance[l];
								xtemp.velocity[l + 1] = xtemp.velocity[l];
							}
							xtemp.distance[k] = xMeasure.flowresult.distance[i];
							xtemp.velocity[k] = xMeasure.flowresult.velocity[i];
							break;
						}
					}
					if (xtemp.count == k)
					{
						xtemp.distance[xtemp.count] = xMeasure.flowresult.distance[i];
						xtemp.velocity[xtemp.count] = xMeasure.flowresult.velocity[i];
					}
					xtemp.count++;
				}
			}
		}

		//排序完成

		for (i = 0;i < xtemp.count;i++)
		{
			if ((xtemp.distance[i] > xMeasure.flowresult.left) && (xtemp.distance[i] < xMeasure.flowresult.right))
				break;
		}
		if (i >= xtemp.count)
		{
			xMeasure.flowresult.flow = 0;
			break;
		}
		//此时有测流垂线位于左右水边之间
		cur_line = xMeasure.flowresult.left;
		for (i = 0;i < xtemp.count;i++)
		{
			if (xtemp.distance[i] > cur_line)
				break;
		}
		//现在计算cur_line与xtemp.distance[i]之间的面积
		temparea = 0;
		for (j = 0;j < pDdmsj->DDMSJ_Measure_Num;j++)
		{
			//当断面线位置位于cur_line和xtemp.distance[i]之间时，计算cur_line与断面线之间的面积，然后累积到temp_area中
		}

	} while (0);
}
int main()
{
/*
	xSettingParams.cable[1] = 10;
	xSettingParams.stard[1] = 120;
	xSettingParams.cable[2] = 20;
	xSettingParams.stard[2] = 140;
	xSettingParams.cable[3] = 30;
	xSettingParams.stard[3] = 160;
	xSettingParams.cable[4] = 40;
	xSettingParams.stard[4] = 180;
	xSettingParams.cable[5] = 50;
	xSettingParams.stard[5] = 200;
	for (int i = 0;i < 60;i++)
	{
		printf("%d,", i);
		printf("%.2f\n", StartDistance_Compensate(i));
	}*/

	if (1)
	{
		xMeasure.flowresult.distance[0] = 150;
		xMeasure.flowresult.velocity[0] = 0;
		xMeasure.flowresult.check_in[0] = 1;

		xMeasure.flowresult.distance[1] = 170;
		xMeasure.flowresult.velocity[1] = 1;
		xMeasure.flowresult.check_in[1] = 1;

		xMeasure.flowresult.distance[2] = 100;
		xMeasure.flowresult.velocity[2] = 2;
		xMeasure.flowresult.check_in[2] = 1;

		xMeasure.flowresult.distance[3] = 130;
		xMeasure.flowresult.velocity[3] = 3;
		xMeasure.flowresult.check_in[3] = 1;

		xMeasure.flowresult.distance[8] = 110;
		xMeasure.flowresult.velocity[8] = 8;
		xMeasure.flowresult.check_in[8] = 1;

		xMeasure.flowresult.distance[11] = 80;
		xMeasure.flowresult.velocity[11] = 11;
		xMeasure.flowresult.check_in[11] = 1;

		xMeasure.flowresult.distance[15] = 120;
		xMeasure.flowresult.velocity[15] = 15;
		xMeasure.flowresult.check_in[15] = 1;
	}

	Cal_Water_shuibian(&xDdmsj,1);

	for (int i = 0;i < xtemp.count;i++)
	{
		printf("%.1f,%.1f\n", xtemp.distance[i], xtemp.velocity[i]);
	}
	printf("count is:%d\n", xtemp.count);



	printf("按回车退出");
	getchar();
    return 0;
}

