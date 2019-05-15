/**
 *          @file  smsbl_motor.h
 *
 *         @brief  smsbl motor
 *
 *       @version  1.0
 *          @date  1/12/2019 05:15:10 PM
 *        @author  liuliu, liu.liu@iim.ltd
 *
 * @section Description
 *
 *       Revision:  none
 *       Compiler:  g++
 *        Company:  IIM
 *
 * @section Description
 *
 * 飞特串行舵机代码
 *
 * 傅头
 *
 */

#ifndef SMSBL_MOTOR_H
#define SMSBL_MOTOR_H

#include <iostream>
#include <stdio.h>
#include<fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <string>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>


namespace iim_ego {
namespace execution {

/**
 * @brief class for smsbl motor
 *
 */
class smsbl_motor
{
public:
    /**
     * @brief constructor
     *
     */
    smsbl_motor();
    /**
     * @brief destructor
     *
     */
    ~smsbl_motor();
    /**
     * @brief  enable torque
     *
     * @param[in] ID :  motor id
     * @param[in] Enable: if enable
     * @param[out] error: output error
     * @return int
     */
    int EnableTorque(uint8_t ID, uint8_t Enable,int *error);
    /**
     * @brief write position for motor to rotate to
     *
     * @param[in] ID : motor id
     * @param[in] Position : goal position
     * @param[in] Speed : speed
     * @param[in] ACC : acc
     * @param[out] error : get serial port communication error type
     * @return int : if success return 1, otherwise return 0
     */
    int WritePos(uint8_t ID, int16_t Position, uint16_t Speed, uint8_t ACC = 0,int *error=NULL);
    /**
     * @brief  async write position for motor to rotate to
     *
     * @param[in] ID : motor id
     * @param[in] Position : goal position
     * @param[in] Speed : speed
     * @param[in] ACC : acc
     * @param[out] error : get serial port communication error type
     * @return int : if success return 1, otherwise return 0
     */
    int RegWritePos(uint8_t ID, int16_t Position, uint16_t Speed, uint8_t ACC = 0,int *error=NULL);
    /**
     * @brief execute async write position
     *
     */
    void RegWriteAction();
    /**
     * @brief sync write position
     *
     * @param[in] ID[] : motor id array
     * @param[in] Position : goal position
     * @param[in] Speed : speed
     * @param[in] ACC : acc
     * @param[out] error : get serial port communication error type
     */
    void SyncWritePos(uint8_t ID[], uint8_t IDN, int16_t Position, uint16_t Speed, uint8_t ACC = 0);
    /**
     * @brief write motor speed
     *
     * @param[in] ID : motor id
     * @param[in] Speed : motor goal speed
     * @param[in] ACC : motor goal acc
     * @return int : if success return 1
     */
    int WriteSpe(uint8_t ID, int16_t Speed, uint8_t ACC = 0);
    /**
     * @brief read motor position
     *
     * @param[in] ID : motor id
     * @param[out] Err : execution error message
     * @return int16_t : return motor position timeout return -1
     */
    int16_t ReadPos(uint8_t ID, uint8_t *Err = NULL);
    /**
     * @brief read motor speed
     *
     * @param[in] ID : motor id
     * @param[out] Err : execution error message
     * @return int16_t : return motor speed timeout return -1
     */
    int16_t ReadSpeed(uint8_t ID, uint8_t *Err = NULL);
    /**
     * @brief read motor load
     *
     * @param[in] ID : motor id
     * @return int : return motor load timeout return -1
     */
    int ReadLoad(uint8_t ID);
    /**
     * @brief read motor current
     * @param[in] ID : motor id
     * @return int : return motor current timeout return -1
     */
    int ReadCurrent(uint8_t ID);
    /**
     * @brief read motor voltage
     *
     * @param[in] ID : motor id
     * @return int : return motor voltage timeout return -1
     */
    int ReadVoltage(uint8_t ID);
    /**
     * @brief read motor moving status
     *
     * @param[in] ID : motor id
     * @return int : return motor moving status timeout return -1
     */
    int ReadMovingStatus(uint8_t ID);
    /**
     * @brief read motor temperature
     *
     * @param[in] ID : motor id
     * @return int : return motor temperature timeout return -1
     */
    int ReadTemper(uint8_t ID);
    /**
     * @brief ping motor
     *
     * @param[in] ID : motor id
     * @return int : return motor id timeout return -1
     */
    int Ping(uint8_t ID);
    /**
     * @brief reset motor to default config
     *
     * @param[in] ID : motor id
     * @return int : return 1 if success
     */
    int Reset(uint8_t ID);
    /**
     * @brief eprom unlock
     *
     * @param[in] ID : motor id
     * @return int
     */
    int unLockEprom(uint8_t ID);
    /**
     * @brief eprom lock
     *
     * @param[in] ID : motor id
     * @return int
     */
    int LockEprom(uint8_t ID);
    /**
     * @brief motor initialization
     *
     * @param[in] baudrate : serial port baudrate
     * @param[in] serial_port : serial port name
     */
    bool initialize(int baudrate,std::string serial_port);

    /**
     * @brief read motor main version
     *
     * @param[in] ID : motor id
     * @return int : if success return main version  timeout return -1
     */
    int ReadMainVersion(uint8_t ID);
    /**
     * @brief read motor minor version
     *
     * @param[in] ID : motor id
     * @return int : if success return minor version timeout return -1
     */
    int ReadMinorVersion(uint8_t ID);
    
    /**
     * @brief get write data error string
     *
     * @param[in] error : error type
     * @return string : return error string description by error
     */
    std::string getErrorStringbytype(const int error) const;

private:
    enum
    {
        WRITE_SUCCESS=0,
        WRITE_ERROR=1,
        WRITE_BUG=2,
    };
    /**
     * @brief  serial port write
     *
     * @param[in] buf : write data
     * @param[in] len : data len
     * @return int
     */
    int serial_write(uint8_t *buf,int len);
    /**
     * @brief serial port read
     *
     * @param[out] buf : read buf
     * @param[in] len : read len
     * @return int
     */
    int serial_read(uint8_t *buf,int len);
    /**
     * @brief write command
     *
     * @param[in] ID : motor id
     * @param[in] MemAddr : member address
     * @param[in] nDat : write data pointer
     * @param[in] nLen : data len
     * @return int : return 1 if success
     */
    int genWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen);
    /**
     * @brief async write command
     *
     * @param[in] ID : motor id
     * @param[in] MemAddr : member address
     * @param[in] nDat : write data pointer
     * @param[in] nLen : data length
     * @return int : return 1 if success
     */
    int regWrite(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen);
    /**
     * @brief sync write command
     *
     * @param[in] ID[] : id array
     * @param[in] IDN : id array count
     * @param[in] MemAddr : member address
     * @param[in] nDat : write data pointer
     * @param[in] nLen : data length
     */
    int snycWrite(uint8_t ID[], uint8_t IDN, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen);
    /**
     * @brief write one byte
     *
     * @param[in] ID : motor id
     * @param[in] MemAddr : member address
     * @param[in] bDat : write data
     * @param[out] error : get serial port communication error type
     * @return int : return 1 if success
     */
    int writeByte(uint8_t ID, uint8_t MemAddr, uint8_t bDat,int *error=NULL);
    /**
     * @brief write one word
     *
     * @param[in] ID : motor id
     * @param[in] MemAddr : member address
     * @param[in] wDat : write data
     * @param[out] error : get serial port communication error type
     * @return int : return 1 if success
     */
    int writeWord(uint8_t ID, uint8_t MemAddr, uint16_t wDat,int* error=NULL);
    /**
     * @brief read data from motor
     *
     * @param[in] ID : motor id
     * @param[in] MemAddr : member address
     * @param[out] nData : read data pointer
     * @param[in] nLen : read length
     * @return int : return read data length if success else return 0
     */
    int Read(uint8_t ID, uint8_t MemAddr, uint8_t *nData, uint8_t nLen);
    /**
     * @brief read one byte
     *
     * @param[in] ID : motor id
     * @param[in] MemAddr : member address
     * @return int : if success return read value else return -1
     */
    int readByte(uint8_t ID, uint8_t MemAddr);
    /**
     * @brief read one word
     *
     * @param[in] ID : motor id
     * @param[in] MemAddr : member address
     * @return int : if success return read value else return -1
     */
    int readWord(uint8_t ID, uint8_t MemAddr);
    /**
     * @brief read motor response
     *
     * @param[in] ID : motor id
     * @return int : if success return 1 else return 0
     */
    int Ack(uint8_t ID);
    /**
     * @brief motor write command
     *
     * @param[in] ID : motor id
     * @param[in] MemAddr : member address
     * @param[in] nDat : data pointer
     * @param[in] nLen : data length
     * @param[in] Fun : execution function type like INST_WRITE
     */
    int writeBuf(uint8_t ID, uint8_t MemAddr, uint8_t *nDat, uint8_t nLen, uint8_t Fun);
    /**
     * @brief one word split to two byte
     *
     * @param[out] DataL : low byte
     * @param[out] DataH : high byte
     * @param[in] Data  : in one word
     */
    void Host2SCS(uint8_t *DataL, uint8_t* DataH, uint16_t Data);//1个16位数拆分为2个8位数
    /**
     * @brief two byte convert to one word
     *
     * @param[in] DataL : low byte
     * @param[in] DataH : high byte
     * @return u16 word : value
     */
    uint16_t SCS2Host(uint8_t DataL, uint8_t DataH);//2个8位数组合为1个16位数
    /**
     * @brief motor write position
     *
     * @param[in] ID : motor id
     * @param[in] Position : motor goal position
     * @param[in] Speed : motor rotate speed
     * @param[in] ACC : motor rotate acc
     * @param[in] Fun : execution function type like INST_WRITE
     * @param[out] error : get serial port communication error type
     * @return int : return 1 if success
     */
    int writePos(uint8_t ID, int16_t Position, uint16_t Speed, uint8_t ACC, uint8_t Fun,int *error);
private:
    const uint8_t  Level; /** motor return level */
    const uint8_t  End; /** cpu little-endian big-endian type */
    const uint8_t  keyValue; /** key value */
    const int16_t  high_flag; /** high flag */
    int fd; /** serial port handle*/
    struct termios	orgopt, curopt; /** fd opt */
};

}
}
#endif

