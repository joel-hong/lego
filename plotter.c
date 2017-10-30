/*	LEGO Plotter
	By Joel Hong
 */
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MC0 "/sys/class/tacho-motor/motor0"
#define MC1 "/sys/class/tacho-motor/motor1"
#define MC2 "/sys/class/tacho-motor/motor2"
#define MC3 "/sys/class/tacho-motor/motor3"
#define MCX MC0
#define MCY MC1
#define MC_SPEED "150"
#define MAX_POINT 1000
#define ZOOM 3
#define YZOOM 2.7
#define WAIT 17000 /*to usec*/

FILE* fdCommand = NULL;
FILE* fdSpeedSp = NULL;
FILE* fdPositionSp = NULL;
FILE* fdCommandY = NULL;
FILE* fdSpeedSpY = NULL;
FILE* fdPositionSpY = NULL;

/*	Connect to and configure motor device drivers
*/
int connect()
{
	/*	X Motor open and setup */
	char* szPathMotor = MCX;
    char szPathCommand[256];
    char szPathSpeedSp[256];
    char szPathPositionSp[256];

    strcpy(szPathCommand, szPathMotor);
    strcat(szPathCommand, "/command");
    strcpy(szPathSpeedSp, szPathMotor);
    strcat(szPathSpeedSp, "/speed_sp");
    strcpy(szPathPositionSp, szPathMotor);
    strcat(szPathPositionSp, "/position_sp");

    fdCommand = fopen(szPathCommand, "a");
    fdSpeedSp = fopen(szPathSpeedSp, "r+");
    fdPositionSp = fopen(szPathPositionSp, "r+");

	if(fdCommand == NULL || fdSpeedSp == NULL || fdPositionSp == NULL){
        printf("\n Cannot open device: %s\n", szPathCommand);
        return 0;
	}
	
	/*	Y Motor open and setup */
    char* szPathMotorY = MCY;
    char szPathCommandY[256];
    char szPathSpeedSpY[256];
    char szPathPositionSpY[256];

    strcpy(szPathCommandY, szPathMotorY);
    strcat(szPathCommandY, "/command");
    strcpy(szPathSpeedSpY, szPathMotorY);
    strcat(szPathSpeedSpY, "/speed_sp");
    strcpy(szPathPositionSpY, szPathMotorY);
    strcat(szPathPositionSpY, "/position_sp");

    fdCommandY = fopen(szPathCommandY, "a");
    fdSpeedSpY = fopen(szPathSpeedSpY, "r+");
    fdPositionSpY = fopen(szPathPositionSpY, "r+");

	if(fdCommandY == NULL || fdSpeedSpY == NULL || fdPositionSpY == NULL){
        printf("\n Cannot open device: %s\n", szPathCommandY);
        return 0;
	}
	
	fprintf(fdCommand, "reset");
    fflush(fdCommand);
	fprintf(fdSpeedSp, MC_SPEED);
	fflush(fdSpeedSp);

	fprintf(fdCommandY, "reset");
    fflush(fdCommandY);
	fprintf(fdSpeedSpY, MC_SPEED);
	fflush(fdSpeedSpY);
	
	return 1;
}

/*	Disconnect from motor device drivers
*/
void disconnect()
{
	fclose(fdCommand);
    fclose(fdSpeedSp);
    fclose(fdPositionSp);
	fclose(fdCommandY);
    fclose(fdSpeedSpY);
    fclose(fdPositionSpY);
}

int nPreX = 0;
int nPreY = 0;

/*	LineTo
	Move to nX, nY by controlling X-motor and Y-motor
*/
void lineTo(int nX, int nY)
{
	char szX[32];
	char szY[32];
	unsigned int nWait;

	nX = (int)(ZOOM*nX);
	if (nX != nPreX){
		nWait = (unsigned int)(abs(nX-nPreX)*WAIT);
		sprintf(szX, "%d", nX);
	
		fprintf(fdPositionSp, szX);
		fflush(fdPositionSp);
		fprintf(fdCommand, "run-to-abs-pos");
		fflush(fdCommand);
		nPreX = nX;
printf("   X: %s %d\n", szX, nWait);
		usleep(nWait);
	}

	nY = (int)(YZOOM*nY);
	
	if (nY != nPreY){
		nWait = (unsigned int)(abs(nY-nPreY)*WAIT);
		sprintf(szY, "%d", nY);
	
printf("   Y: %s %d\n", szY, nWait);	
		fprintf(fdPositionSpY, szY);
		fflush(fdPositionSpY);
		fprintf(fdCommandY, "run-to-abs-pos");
		fflush(fdCommandY);
		nPreY = nY;
		usleep(nWait);
	}
}

/*	Read data file from 'stdin' and plot the file
*/
int main()
{
	int nX[MAX_POINT],nY[MAX_POINT],nCnt;
	int i;
	
	if (!connect()){
		printf("cannot connect to device. please check the motor cable number.\n");
		return 0;
	}	
	
	scanf("%d",&nCnt);
	
	for(i=0; i<nCnt; i++){
		scanf("%d %d",&nX[i], &nY[i]);
printf("[%d] %d,%d\n", i, nX[i], nY[i]);
		lineTo(nX[i], nY[i]);
	}
	
	disconnect(); 
	
	return 1;
}
