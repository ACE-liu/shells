// =====================================================================================
//
//       Filename:  smsbl_motor.cpp
//
//        Version:  1.0
//        Created:  1/12/2019 05:15:10 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  liuliu, liu.liu@iim.ltd
//        Company:  IIM
//
//    Description:  smsbl motor
//
// =====================================================================================

#include "smsbl_motor.h"
#include <boost/concept_check.hpp>


namespace{
//指令类型
//指令                  	功能                           					值               参数长度
//PING (查询)           	查询工作状态                    					0x01                0
//READ DATA(读)         	查询控制表里的字符               					0x02 		    2
//WRITE DATA(写)        	往控制表里写入字符 	       					0x03 		    大于等于1
//REGWRITE DATA(异步写)  类似于WRITE DATA,但是控制字符写入后并不马上动作,直到ACTION指令到达 	0x04 		    不小于2
//ACTION(执行异步写) 	触发REG WRITE写入的动作 						0x05 		    0
//SYCNWRITE DATA(同步写) 用于同时控制多个舵机 						0x83                不小于2

uint8_t INST_PING= 0x01;
uint8_t INST_READ= 0x02;
uint8_t INST_WRITE= 0x03;
uint8_t INST_REG_WRITE= 0x04;
uint8_t INST_ACTION= 0x05;
uint8_t INST_RESET= 0x06;
uint8_t INST_SYNC_WRITE= 0x83;
//uint8_t INST_ANGLE_RESET= 0x0a;

//波特率定义

// #define		SMSBL_1M		0
// #define		SMSBL_0_5M		1
// #define		SMSBL_250K		2
// #define		SMSBL_128K		3
// #define		SMSBL_115200	4
// #define		SMSBL_76800		5
// #define		SMSBL_57600		6
// #define		SMSBL_38400		7

//内存表定义
//-------EPROM(只读)--------
//uint8_t SMSBL_VERSION_L= 0;
//uint8_t SMSBL_VERSION_H= 1;
uint8_t SMSBL_MODEL_L= 3;
uint8_t SMSBL_MODEL_H= 4;

//-------EPROM(读写)--------
// uint8_t SMSBL_ID= 5;
// uint8_t SMSBL_BAUD_RATE= 6;
// uint8_t SMSBL_RETURN_DELAY_TIME= 7;
// uint8_t SMSBL_RETURN_LEVEL= 8;
// uint8_t SMSBL_MIN_ANGLE_LIMIT_L= 9;
// uint8_t SMSBL_MIN_ANGLE_LIMIT_H= 10;
// uint8_t SMSBL_MAX_ANGLE_LIMIT_L= 11;
// uint8_t SMSBL_MAX_ANGLE_LIMIT_H= 12;
// uint8_t SMSBL_LIMIT_TEMPERATURE= 13;
// uint8_t SMSBL_MAX_LIMIT_VOLTAGE= 14;
// uint8_t SMSBL_MIN_LIMIT_VOLTAGE= 15;
// uint8_t SMSBL_MAX_TORQUE_L= 16;
// uint8_t SMSBL_MAX_TORQUE_H= 17;
// uint8_t SMSBL_ALARM_LED= 19;
// uint8_t SMSBL_ALARM_SHUTDOWN= 20;
// uint8_t SMSBL_COMPLIANCE_P= 21;
// uint8_t SMSBL_COMPLIANCE_D= 22;
// uint8_t SMSBL_COMPLIANCE_I=23;
// uint8_t SMSBL_PUNCH_L= 24;
// uint8_t SMSBL_PUNCH_H= 25;
// uint8_t SMSBL_CW_DEAD= 26;
// uint8_t SMSBL_CCW_DEAD= 27;
// uint8_t SMSBL_MAX_CURRENT_L= 28;
// uint8_t SMSBL_MAX_CURRENT_H= 29;
// uint8_t SMSBL_ANGLE_RESOLVING= 30;
//uint8_t SMSBL_OFS_L= 31;
// uint8_t SMSBL_OFS_H= 32;
//uint8_t SMSBL_MODE= 33;            // 33 SM30BL
// uint8_t SMSBL_PROTECT_TORQUE= 34;
// uint8_t SMSBL_PROTECT_TIME= 35;
// uint8_t SMSBL_OVLOAD_TORQUE= 36;

//-------SRAM(读写)--------
uint8_t SMSBL_TORQUE_ENABLE=40;
uint8_t SMSBL_ACC= 41;
// uint8_t SMSBL_GOAL_POSITION_L= 42;
// uint8_t SMSBL_GOAL_POSITION_H= 43;
// uint8_t SMSBL_GOAL_TIME_L= 44;
// uint8_t SMSBL_GOAL_TIME_H= 45;
uint8_t SMSBL_GOAL_SPEED_L= 78;   //46 SM30BL
// uint8_t SMSBL_GOAL_SPEED_H= 79;   //47 SM30BL
uint8_t SMSBL_LOCK= 55;

//-------SRAM(只读)--------
uint8_t SMSBL_PRESENT_POSITION_L=56;
// uint8_t SMSBL_PRESENT_POSITION_H=57;
uint8_t SMSBL_PRESENT_SPEED_L=58;
// uint8_t SMSBL_PRESENT_SPEED_H=59;
uint8_t SMSBL_PRESENT_LOAD_L=60;
// uint8_t SMSBL_PRESENT_LOAD_H=61;
uint8_t SMSBL_PRESENT_VOLTAGE=62;
uint8_t SMSBL_PRESENT_TEMPERATURE=63;
// uint8_t SMSBL_REGISTERED_INSTRUCTION=64;
uint8_t SMSBL_MOVING=66;
// uint8_t SMSBL_PRESENT_GOALPOS_L=67;
// uint8_t SMSBL_PRESENT_GOALPOS_H=68;
uint8_t SMSBL_PRESENT_CURRENT_L=69;
// uint8_t SMSBL_PRESENT_CURRENT_H=70;
}

iim_ego::execution::smsbl_motor::smsbl_motor()
    : Level(1),
      End(0),
      keyValue(0xfe),
      high_flag(1<<15)
{   
    fd = -1;
}

iim_ego::execution::smsbl_motor::~smsbl_motor()
{

}

bool
iim_ego::execution::smsbl_motor::initialize(int baudrate,std::string serial_port)
{
    char buf[1024];
    if( fd != -1 )
        return false;
    sprintf(buf, "%s", serial_port.c_str());
    fd = open(buf, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1)
        return false;
    fcntl(fd, F_SETFL, FNDELAY);
    tcgetattr(fd, &orgopt);
    tcgetattr(fd, &curopt);
    speed_t CR_BAUDRATE = B115200;
    switch(baudrate){
    case 9600:
        CR_BAUDRATE = B9600;
        break;
    case 19200:
        CR_BAUDRATE = B19200;
        break;
    case 38400:
        CR_BAUDRATE = B38400;
        break;
    case 57600:
        CR_BAUDRATE = B57600;
        break;
    case 115200:
        CR_BAUDRATE = B115200;
        break;
    case 230400:
        CR_BAUDRATE = B230400;
        break;
    default:
        break;
    }
    cfsetispeed(&curopt, CR_BAUDRATE);
    cfsetospeed(&curopt, CR_BAUDRATE);

//     printf("serial speed %d\n",baudrate);
    /* Mostly 8N1 */
    curopt.c_cflag &= ~PARENB;
    curopt.c_cflag &= ~CSTOPB;
    curopt.c_cflag &= ~CSIZE;
    curopt.c_cflag |= CS8;
    curopt.c_cflag |= CREAD;
    curopt.c_cflag |= CLOCAL;	//disable modem statuc check
    cfmakeraw(&curopt);		//make raw mode
    if (tcsetattr(fd, TCSANOW, &curopt) == 0){
        printf("serial init done...\n");
        return true;
    }
    return false;
}

std::string iim_ego::execution::smsbl_motor::getErrorStringbytype(const int error) const
{
    std::string rtn ="";
    if((error&2)!=0)
        rtn+="write bug :data do not totally written------";
    if((error&1)!=0)
        rtn+="write error :write data with -1 return------";
    
    return rtn;
}


int
iim_ego::execution::smsbl_motor::serial_write(uint8_t *buf,int len)
{
    int	retval;
    int status=0;
    retval = write(fd, buf, len);
    if(retval==len)
    {
        status |= WRITE_SUCCESS;
    }
    else if(retval==-1)
    {
        status |= WRITE_ERROR;
        printf("serial port is unplugged or not stable.");
        //abort();
    }
    else   // retval<len
    {
        status |= WRITE_BUG;
        printf("serial data is written incompletely : %d, %d",len,retval);
    }
    return status;
}

int
iim_ego::execution::smsbl_motor::serial_read(uint8_t *buf,int len)
{

    int fs_sel;
    fd_set fs_read;

    struct timeval time;

    FD_ZERO(&fs_read);
    FD_SET(fd,&fs_read);

    time.tv_sec = 10;
    time.tv_usec = 0;

    //使用select实现串口的多路通信
    fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);
    if(fs_sel)
    {
        fs_sel = read(fd,buf,len);
        return fs_sel;
    }
    else
    {
        return false;
    }
}

//1个16位数拆分为2个8位数
//DataL为低位，DataH为高位
void
iim_ego::execution::smsbl_motor::Host2SCS(uint8_t *DataL, uint8_t* DataH, uint16_t Data)
{
    if(End){
        *DataL = (Data>>8);
        *DataH = (Data&0xff);
    }else{
        *DataH = (Data>>8);
        *DataL = (Data&0xff);
    }
}

//2个8位数组合为1个16位数
//DataL为低位，DataH为高位
uint16_t
iim_ego::execution::smsbl_motor::SCS2Host(uint8_t DataL, uint8_t DataH)
{
    uint16_t Data;
    if(End){
        Data = DataL;
        Data<<=8;
        Data |= DataH;
    }else{
        Data = DataH;
        Data<<=8;
        Data |= DataL;
    }
    return Data;
}

int
iim_ego::execution::smsbl_motor::writeBuf(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen, uint8_t Fun)
{
    uint8_t i;
    uint8_t msgLen = 2;
    uint8_t bBuf[6];
    uint8_t CheckSum = 0;
    int status=0;
    bBuf[0] = 0xff;
    bBuf[1] = 0xff;
    bBuf[2] = ID;
    bBuf[4] = Fun;
    if(nDat){
        msgLen += nLen + 1;
        bBuf[3] = msgLen;
        bBuf[5] = MemAddr;
        status |=serial_write(bBuf, 6);

    }else{
        bBuf[3] = msgLen;
        status |=serial_write(bBuf, 5);
    }
    CheckSum = ID + msgLen + Fun + MemAddr;
    if(nDat){
        for(i=0; i<nLen; i++){
            CheckSum += nDat[i];
        }
        status |=serial_write(nDat, nLen);
    }
    CheckSum = ~CheckSum;
    status |=serial_write(&CheckSum, 1);
    return status;
}

//普通写指令
//舵机ID，MemAddr内存表地址，写入数据，写入长度
int
iim_ego::execution::smsbl_motor::genWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen)
{
    writeBuf(ID, MemAddr, nDat, nLen, INST_WRITE);
    return Ack(ID);
}

//异步写指令
//舵机ID，MemAddr内存表地址，写入数据，写入长度
int
iim_ego::execution::smsbl_motor::regWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen)
{
    writeBuf(ID, MemAddr, nDat, nLen, INST_REG_WRITE);
    return Ack(ID);
}

//同步写指令
//舵机ID[]数组，IDN数组长度，MemAddr内存表地址，写入数据，写入长度
int
iim_ego::execution::smsbl_motor::snycWrite(uint8_t ID[], uint8_t IDN, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen)
{ 
    uint8_t i, j;
    uint8_t mesLen = ((nLen+1)*IDN+4);
    uint8_t checkSum = 0;
    uint8_t bBuf[7];
    int status=0;
    bBuf[0] = 0xff;
    bBuf[1] = 0xff;
    bBuf[2] = keyValue;
    bBuf[3] = mesLen;
    bBuf[4] = INST_SYNC_WRITE;
    bBuf[5] = MemAddr;
    bBuf[6] = nLen;
    status |= serial_write(bBuf, 7);

    checkSum = keyValue + mesLen + INST_SYNC_WRITE + MemAddr + nLen;
    for(i=0; i<IDN; i++){
        status |=serial_write(ID+i, 1);
        status |=serial_write(nDat, nLen);
        checkSum += ID[i];
        for(j=0; j<nLen; j++){
            checkSum += nDat[j];
        }
    }
    checkSum = ~checkSum;
    status |=serial_write(&checkSum, 1);
    return status;
}

int
iim_ego::execution::smsbl_motor::writeByte(uint8_t ID, uint8_t MemAddr, uint8_t bDat,int *error)
{
    int status= writeBuf(ID, MemAddr, &bDat, 1, INST_WRITE);
    if(error!=NULL)
        *error=status;
    if(status != WRITE_SUCCESS)
        return 0;
    return Ack(ID);
}

int
iim_ego::execution::smsbl_motor::writeWord(uint8_t ID, uint8_t MemAddr, uint16_t wDat,int *error)
{
    uint8_t buf[2];
    Host2SCS(buf+0, buf+1, wDat);
    int status=writeBuf(ID, MemAddr, buf, 2, INST_WRITE);
    if(error!=NULL)
        *error=status;
    if(status != WRITE_SUCCESS)
        return 0;
    return Ack(ID);
}

//读指令
//舵机ID，MemAddr内存表地址，返回数据nData，数据长度nLen
int
iim_ego::execution::smsbl_motor::Read(uint8_t ID, uint8_t MemAddr, uint8_t *nData, uint8_t nLen)
{
    int Size;
    uint8_t bBuf[5];
    writeBuf(ID, MemAddr, &nLen, 1, INST_READ);

    if(serial_read(bBuf, 5)!=5){
        return 0;
    }
    Size = serial_read(nData, nLen);
    if(serial_read(bBuf, 1)){
        return Size;
    }
    return 0;
}

//读1字节，超时返回-1
int iim_ego::execution::smsbl_motor::readByte(uint8_t ID, uint8_t MemAddr)
{
    uint8_t bDat;
    tcflush(fd, TCIFLUSH);
    int Size = Read(ID, MemAddr, &bDat, 1);
    if(Size!=1){
        return -1;
    }else{
        return bDat;
    }
}

//读2字节，超时返回-1
int iim_ego::execution::smsbl_motor::readWord(uint8_t ID, uint8_t MemAddr)
{	
    uint8_t nDat[2];
    int Size;
    uint16_t wDat;
    tcflush(fd, TCIFLUSH);
    Size = Read(ID, MemAddr, nDat, 2);
    if(Size!=2)
    {
        return -1;
    }
    wDat = SCS2Host(nDat[0], nDat[1]);
    return wDat;
}

//指令应答
int iim_ego::execution::smsbl_motor::Ack(uint8_t ID)
{       
    if(ID != keyValue && Level){
        uint8_t buf[6];
        uint8_t Size = serial_read(buf, 6);
        if(Size!=6){
            return 0;
        }
    }
    return 1;
}

int iim_ego::execution::smsbl_motor::writePos(uint8_t ID, int16_t Position, uint16_t Speed, uint8_t ACC, uint8_t Fun, int* error)
{
    if(Position<0){
        Position = -Position;
        Position |= high_flag;
    }
    uint8_t buf[7];
    buf[0] = ACC;
    Host2SCS(buf+1, buf+2, Position);
    Host2SCS(buf+3, buf+4, 0);
    Host2SCS(buf+5, buf+6, Speed);
    //for (int i=0;i<7;i++)
    //{
    //  printf("\r\nbuf[%d]:0x%x",i,buf[i]);
    //}
    int status=writeBuf(ID, SMSBL_ACC, buf, 7, Fun);
    if(error!=NULL)
        *error=status;
    if(status != WRITE_SUCCESS)
        return 0;
    return Ack(ID);
}


int iim_ego::execution::smsbl_motor::EnableTorque(uint8_t ID, uint8_t Enable,int *error)
{
    return writeByte(ID, SMSBL_TORQUE_ENABLE, Enable,error);
}

//写位置指令
//舵机ID，Position位置，加速度ACC，速度Speed
int iim_ego::execution::smsbl_motor::WritePos(uint8_t ID, int16_t Position, uint16_t Speed, uint8_t ACC,int *error)
{
    return writePos(ID, Position, Speed, ACC, INST_WRITE,error);
}

//异步写位置指令
//舵机ID，Position位置，加速度ACC，速度Speed
int iim_ego::execution::smsbl_motor::RegWritePos(uint8_t ID, int16_t Position, uint16_t Speed, uint8_t ACC,int *error)
{
    return writePos(ID, Position, Speed, ACC, INST_REG_WRITE,error);
}

void iim_ego::execution::smsbl_motor::RegWriteAction()
{
    writeBuf(keyValue, 0, NULL, 0, INST_ACTION);
}

//写位置指令
//舵机ID[]数组，IDN数组长度，Position位置，ACC加速度，速度Speed
void iim_ego::execution::smsbl_motor::SyncWritePos(uint8_t ID[], uint8_t IDN, int16_t Position, uint16_t Speed, uint8_t ACC)
{
    if(Position<0){
        Position = -Position;
        Position |= high_flag; 
    }
    uint8_t buf[7];
    buf[0] = ACC;
    Host2SCS(buf+1, buf+2, Position);
    Host2SCS(buf+3, buf+4, 0);
    Host2SCS(buf+5, buf+6, Speed);
    snycWrite(ID, IDN, SMSBL_ACC, buf, 7);
}

//读位置，超时Err=1
int16_t iim_ego::execution::smsbl_motor::ReadPos(uint8_t ID, uint8_t *Err)
{
    int16_t curPos = readWord(ID, SMSBL_PRESENT_POSITION_L);
    if(curPos==-1){
        if(Err){
            *Err = 1;
        }
        return -1;
    }
    if(curPos&high_flag){
        curPos = -(curPos&~high_flag);
    }
    if(Err){
        *Err = 0;
    }

    return curPos;
}
//读速度，超时Err=1
int16_t iim_ego::execution::smsbl_motor::ReadSpeed(uint8_t ID, uint8_t *Err)
{
    int16_t curSpe = readWord(ID, SMSBL_PRESENT_SPEED_L);
    if(curSpe==-1){
        if(Err){
            *Err = 1;
        }
        return -1;
    }
    if(curSpe&high_flag){
        curSpe = -(curSpe&~high_flag);
    }
    if(Err){
        *Err = 0;
    }

    return curSpe;
}

int iim_ego::execution::smsbl_motor::ReadMainVersion(uint8_t ID)
{
    return readByte(ID, SMSBL_MODEL_L);
}

int iim_ego::execution::smsbl_motor::ReadMinorVersion(uint8_t ID)
{
    return readByte(ID, SMSBL_MODEL_H);
}
//速度控制模式
int iim_ego::execution::smsbl_motor::WriteSpe(uint8_t ID, int16_t Speed, uint8_t ACC)
{
    if(Speed<0){
        Speed = -Speed;
        Speed |= high_flag;
    }
    int res = writeByte(ID, SMSBL_ACC, ACC);
    if(res==-1){
        return -1;
    }
    return writeWord(ID, SMSBL_GOAL_SPEED_L, Speed);
}

// //PWM输出模式
// int iim_ego::execution::smsbl_motor::WritePWM(uint8_t ID, int16_t pwmOut)
// {
//     if(pwmOut<0){
//         pwmOut = -pwmOut;
//         pwmOut |= high_flag;
//     }
//     return writeWord(ID, SMSBL_GOAL_SPEED_L, pwmOut);
// }

//读负载，超时返回-1
int iim_ego::execution::smsbl_motor::ReadLoad(uint8_t ID)
{	
    return readWord(ID, SMSBL_PRESENT_LOAD_L);
}
//读当前电流
int iim_ego::execution::smsbl_motor::ReadCurrent(uint8_t ID)
{	
    return readWord(ID, SMSBL_PRESENT_CURRENT_L);
}
//读移动状态
int iim_ego::execution::smsbl_motor::ReadMovingStatus(uint8_t ID)
{	
    return readByte(ID, SMSBL_MOVING);
}

//读电压，超时返回-1
int iim_ego::execution::smsbl_motor::ReadVoltage(uint8_t ID)
{	
    return readByte(ID, SMSBL_PRESENT_VOLTAGE);
}

//读温度，超时返回-1
int iim_ego::execution::smsbl_motor::ReadTemper(uint8_t ID)
{	
    return readByte(ID, SMSBL_PRESENT_TEMPERATURE);
}

//Ping指令，返回舵机ID，超时返回-1
int iim_ego::execution::smsbl_motor::Ping(uint8_t ID)
{
    uint8_t bBuf[6];
    writeBuf(ID, 0, NULL, 0, INST_PING);
    int Size = serial_read(bBuf, 6);
    if(Size==6){
        return bBuf[2];
    }else{
        return -1;
    }
}

// int iim_ego::execution::smsbl_motor::wheelMode(uint8_t ID)
// {
//     return writeByte(ID, SMSBL_MODE, 1);
// }

// int iim_ego::execution::smsbl_motor::pwmMode(uint8_t ID)
// {
//     return writeByte(ID, SMSBL_MODE, 2);
// }

// int iim_ego::execution::smsbl_motor::joinMode(uint8_t ID)
// {
//     return writeByte(ID, SMSBL_MODE, 0);
// }

//复位舵机参数为默认值
int iim_ego::execution::smsbl_motor::Reset(uint8_t ID)
{
    writeBuf(ID, 0, NULL, 0, INST_RESET);
    return Ack(ID);
}

// int iim_ego::execution::smsbl_motor::WriteOfs(uint8_t ID, int16_t Ofs)
// {
//     if(Ofs<0){
//         Ofs = -Ofs;
//         Ofs |= high_flag;
//     }
//     return writeWord(ID, SMSBL_OFS_L, Ofs);
// }

int iim_ego::execution::smsbl_motor::unLockEprom(uint8_t ID)
{
    return writeByte(ID, SMSBL_LOCK, 0);//打开EPROM保存功能
}

int iim_ego::execution::smsbl_motor::LockEprom(uint8_t ID)
{
    return writeByte(ID, SMSBL_LOCK, 1);//关闭EPROM保存功能
}

