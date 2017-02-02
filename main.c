#include <reg52.h>
/*
* -------------------------------------------
* EE111 Project: Hello Robot
* Luo chenqi (Loki), QU zhixin (James), JAN.7 2017(Final Version)
* SIST, ShanghaiTech University
*/
/////////////////////////////////
#define uchar unsigned char
#define uint unsigned int

uint a = 0;
uint b = 0;
uchar code DSY_CODE[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d,
                         0x7d, 0x07, 0x7f, 0x6f, 0x00};

uchar code ledbits[] = {0x20, 0x10, 0x08};
uint now1, previous1 = 0;
uint now2, previous2 = 0;
uint a1, b1, c1, d1 = 0;
uchar data Buffer_Counts[] = {0, 0, 0};

uint Count_A = 0;
uint de_fre = 0;
uint maxw = 40;
//////////////

#define BAUD 2400L                 //set Baud rate
#define FOSC 22118400L             //set Master clock
#define CYCLE (12000000.0 / FOSC)  // according to the crystal frequency
sbit sound = P1 ^ 0;               // define sound pins
sbit Trig = P2 ^ 0;                // define sensor1 trig pin
sbit Echo = P2 ^ 1;                // define sensor1 echo pin
sbit Trig2 = P2 ^ 2;               // define sensor2 trig pin
sbit Echo2 = P2 ^ 3;               // define sensor2 echo pin

uint dist1;
uint dist2;
///////////////////////////
void DelayMS(uint x)  // delaypart
{
    uchar time;
    while (x--)
    {
        for (time = 0; time < 120; time++)
        {
            ;
        }
    }
}

float getDist1()
{
    uchar i = 100;
    float dist = 0.0f;
    uint count = 0;
    Trig = 1;
    while (i--)
        ;
    TMOD = 0x01;
    TH0 = TL0 = 0;
    Trig = 0;
    while (!Echo)
        ;
    TR0 = 1;
    while (Echo)
        ;
    TR0 = 0;
    count = (TH0 << 8) | TL0;
    dist = CYCLE * count * 0.017;  // multiply voice speed to get the distance
    return dist;
}

float getDist2()
{
    uchar i = 100;
    float dist = 0.0f;
    uint count = 0;

    Trig2 = 1;
    while (i--)
        ;
    TMOD = 0x01;
    TH0 = TL0 = 0;
    Trig2 = 0;
    while (!Echo2)
        ;
    TR0 = 1;
    while (Echo2)
        ;
    TR0 = 0;
    count = (TH0 << 8) | TL0;
    dist = CYCLE * count * 0.017;  // multiply voice speed to get the distance
    return dist;
}

/////////

void Show_Counts()
{
    uint i;
    de_fre++;
    if (de_fre == 20)  // we can adjust a proper frequency for detecting.only
                       // when it is 20 ,we will do the loop.
    {
        previous1 = now1;  // let the previous be the now .
        previous2 = now2;
        de_fre = 0;
        dist1 = (uint)getDist1();  // get the datum from both sensors
        dist2 = (uint)getDist2();
        now1 = dist1;
        now2 = dist2;
        if (previous1 < maxw && now1 > maxw)  // if passing through sensor1
        {
            a1 = 1;
            if (b1 == 2)  // if passing through sensor2 and sensor1 in order
            {
                if (Count_A > 0)  // avoid increasing ridiculous minus bugs.
                {
                    --Count_A;  // count minus one
                }
                DelayMS(10);

                a1 = 0;  // we need to reset the variable a1,b1 to zero.
                b1 = 0;
            }
        }
        if (previous2 < maxw && now2 > maxw)  // if passing through sensor2
        {
            b1 = 2;
            if (a1 == 1)  // if passing through sensor1 and sensor2 in order
            {
                ++Count_A;  // count increase one and say"welcome".
                sound = 1;  // trig the sound mode to make sound.
                sound = 0;
                DelayMS(10);
                a1 = 0;  // we need to reset the variable a1,b1 to zero.
                b1 = 0;
            }
        }
    }
    Buffer_Counts[2] = 0;
    Buffer_Counts[1] = Count_A % 100 / 10;
    Buffer_Counts[0] = Count_A % 10;
    if (Buffer_Counts[2] == 0)
    {
        Buffer_Counts[2] = 0x0a;
        if (Buffer_Counts[1] == 0)
        {
            Buffer_Counts[1] = 0x0a;
        }
    }

    for (i = 0; i < 3; i++)  // display every digit at every ms
    {
        P2 = ledbits[i];
        P0 = DSY_CODE[Buffer_Counts[i]];
        DelayMS(1);
    }
}

void main()
{
    IT0 = 1;
    IT1 = 1;
    PX0 = 1;
    IE = 0x85;
    Trig = 0;
    Echo = 1;
    Trig2 = 0;
    Echo2 = 1;  // initializing
    while (1)
    {
        Show_Counts();
        // sleep();
    }  
}