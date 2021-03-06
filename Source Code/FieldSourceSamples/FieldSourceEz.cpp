/*******************************************************************
	Author:  David Ge (dge893@gmail.com, aka Wei Ge)
	Last modified: 03/31/2018
	Allrights reserved by David Ge

********************************************************************/

#include "FieldSourceEz.h"


FieldSourceEz::FieldSourceEz(void)
{
	ppw = 0.0;
	r = 0;
}

/*
	in the book "understanding FDTD method", ppw is read from keyboard input.
	here we get it from a task parameter; parameter name is FS.PPW
*/
int FieldSourceEz::initialize(double Courant, int maximumRadius, TaskFile *taskParameters)
{
	int ret = FieldSource::initialize(Courant, maximumRadius, taskParameters);
	if(ret == ERR_OK)
	{
		ppw = taskParameters->getDouble(TP_FS_PPW, false);
		ret = taskParameters->getErrorCode();
		if(ret == ERR_OK)
		{
			if(ppw <= 0.0)
			{
				ret = ERR_TASK_INVALID_VALUE;
				taskParameters->setNameOfInvalidValue(TP_FS_PPW);
			}
		}
	}
	return ret;
}

/*
	the source is appplied only at (0,0,0),
	that is, radius == 0
*/
RadiusHandleType FieldSourceEz::setRadius(int radius)
{
	if(radius > 0)
	{
		//end applying the source
		return Finish;
	}
	//radius == 0, apply the source
	return NeedProcess;
}

/*
	due to how setRadius is coded
	it is called only when m=n=p=0
*/
void FieldSourceEz::handleData(int m, int n, int p)
{
	/*
		this part of code is taken from book "understanding FDTD method" by John B. Schneider
		ricker.c:
		double ezInc(double time, double location) {
			double arg;
			arg = M_PI * ((cdtds * time - location) / ppw - 1.0);
			arg = arg * arg;
			return (1.0 - 2.0 * arg) * exp(-arg);
		}
		3ddemo.c:
		Ex((SizeX - 1) / 2, SizeY / 2, SizeZ / 2) += ezInc(Time, 0.0);
	*/
	double arg;
	arg = M_PI * ((Cdtds * _time - 0.0) / ppw - 1.0);
	arg = arg * arg;
	//m=n=p=0 is the first element of array _fields
	_fields[0].E.z += (1.0 - 2.0 * arg) * exp(-arg);
}
