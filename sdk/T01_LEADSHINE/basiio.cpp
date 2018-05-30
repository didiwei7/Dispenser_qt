#include "baseio.h"

BOOL g_bIniIOOK;

LEADSHINE_COutBits::LEADSHINE_COutBits()
{
}
LEADSHINE_COutBits::LEADSHINE_COutBits(int card, int stbit, int bitnum)//stbit 开始位的位号，bitnum  从开始位往后多少位
{
    SetAddr(card, stbit, bitnum);
}
LEADSHINE_COutBits::~LEADSHINE_COutBits()
{
}
void LEADSHINE_COutBits::Out(int value)
{

}
void LEADSHINE_COutBits::Inc(int step)
{
    d_value += step;
    Out(d_value);
}
void LEADSHINE_COutBits::Dec(int step)
{
    d_value -= step;
    Out(d_value);
}
void LEADSHINE_COutBits::Clear() //out zero
{
    d_value = 0;
    Out(d_value);
}
int LEADSHINE_COutBits::Get()
{
    return d_value;
}
void LEADSHINE_COutBits::SetAddr(int card, int stbit, int bitnum)
{
    QVERIFY(card>=0 && card<=4);//0,1

    d_card = card;
    d_stbit = stbit;
    d_bitnum = bitnum;
    d_maxval = (0x01<<bitnum)-1;
}

LEADSHINE_COutByte::LEADSHINE_COutByte()
{
}
LEADSHINE_COutByte::~LEADSHINE_COutByte()
{
}
LEADSHINE_COutByte::LEADSHINE_COutByte(int card, int stbit)
{
    SetAddr(card, stbit);
}
void LEADSHINE_COutByte::SetAddr(int card, int stbit)
{
    LEADSHINE_COutBits::SetAddr(card, stbit, 8);
}

LEADSHINE_COutBit::LEADSHINE_COutBit()
{
}
LEADSHINE_COutBit::LEADSHINE_COutBit(int card, int bit)
{
    SetAddr(card, bit);
}
LEADSHINE_COutBit::~LEADSHINE_COutBit()
{
}
void LEADSHINE_COutBit::On()
{
    if (g_bIniIOOK)
    {
        ioc_write_outbit(d_card, d_bit,0);
        d_state = 1;
    }
}
void LEADSHINE_COutBit::Off()
{
    if (g_bIniIOOK)
    {
        ioc_write_outbit(d_card, d_bit,1);
        d_state = 0;
    }
}
void LEADSHINE_COutBit::Toggle()
{
    d_state==0?On():Off();
}
int LEADSHINE_COutBit::GetState()
{
    if (0 == ioc_read_outbit(d_card, d_bit))
    {
        d_state = 0;
    }
    else
    {
        d_state = 1;
    }
    return d_state;
}
void LEADSHINE_COutBit::SetAddr(int card, int bit)
{
    QVERIFY(card>=0 && card<=4);
    QVERIFY(bit>=0 && bit<=32);
    d_card = card;
    d_bit = bit;
}

LEADSHINE_CInBit::LEADSHINE_CInBit()
{
}
LEADSHINE_CInBit::LEADSHINE_CInBit(int card, int bit)
{
    SetAddr(card, bit);
}
LEADSHINE_CInBit::~LEADSHINE_CInBit()
{
}
int LEADSHINE_CInBit::Read()
{
    int input=-1;
    if (g_bIniIOOK)
    {
        input = ioc_read_inbit(d_card,d_bit);
    }
    return input;
}
void LEADSHINE_CInBit::SetAddr(int card, int bit)
{
    QVERIFY(card>=0 && card<=4);
    QVERIFY(bit>=0 && bit<=32);

    d_card = card;
    d_bit = bit;
}

LEADSHINE_CInByte::LEADSHINE_CInByte()
{
}
LEADSHINE_CInByte::LEADSHINE_CInByte(int card, int stbit)
{
    SetAddr(card, stbit);
}
LEADSHINE_CInByte::~LEADSHINE_CInByte()
{
}
int LEADSHINE_CInByte::Read()
{
    // long   input;
    return 0;
}
void LEADSHINE_CInByte::SetAddr(int card, int stbit)
{
    QVERIFY(card>=0 && card<=4);//0,1,2
    QVERIFY(stbit>=0 && stbit<=8);//0-8

    d_card = card;
    d_stbit = stbit;
}




























































