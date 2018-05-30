#ifndef BASEIO_H
#define BASEIO_H

#include <QtWidgets>
#include <QTest>
#include <Windows.h>

#include "IOC0640.H"
#include "LTDMC.h"



#include <QtCore/qglobal.h>
#ifndef AFX_API_EXPORT
#  define AFX_API_EXPORT Q_DECL_EXPORT
#else
#  define AFX_API_EXPORT Q_DECL_IMPORT
#endif

class AFX_API_EXPORT LEADSHINE_COutBits
{
public:
    LEADSHINE_COutBits();
    LEADSHINE_COutBits(int card, int stbit, int bitnum);
    ~LEADSHINE_COutBits();
    virtual void SetAddr(int card, int stbit, int bitnum);

public:
    void Out(int value);
    void Inc(int step=1);
    void Dec(int step=1);
    void Clear();
    int Get();

protected:
    int d_card;
    int d_stbit;
    int d_bitnum;
    unsigned short d_value;
    int d_maxval;
};

class AFX_API_EXPORT LEADSHINE_COutByte :public LEADSHINE_COutBits
{
public:
    LEADSHINE_COutByte();
    LEADSHINE_COutByte(int card, int stbit);
    ~LEADSHINE_COutByte();
    void SetAddr(int card, int stbit);
};

class AFX_API_EXPORT LEADSHINE_COutBit
{
public:
    LEADSHINE_COutBit();
    LEADSHINE_COutBit(int card, int bit);
    ~LEADSHINE_COutBit();
    void SetAddr(int card, int bit);
public:
    void On();
    void Off();
    void Toggle();
    int GetState();
protected:
    int d_card;
    int d_bit; //0-7
    int d_state; //0-1
};

class AFX_API_EXPORT LEADSHINE_CInByte
{
public:
    LEADSHINE_CInByte();
    LEADSHINE_CInByte(int card, int stbit);
    ~LEADSHINE_CInByte();
public:
    int Read();
protected:
    void SetAddr(int card, int stbit);
protected:
    int d_card;
    int d_stbit;

};

class AFX_API_EXPORT LEADSHINE_CInBit
{
public:
    LEADSHINE_CInBit();
    LEADSHINE_CInBit(int card, int bit);
    ~LEADSHINE_CInBit();
public:
    int Read();
    void SetAddr(int card, int bit);
protected:
    int d_card;
    int d_bit;
};

#endif // BASEIO_H
