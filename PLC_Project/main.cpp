//GalilTest
#include <Windows.h>
#include <iostream>
#include <conio.h>

#include "Galil.h"
#include "EmbeddedFunctions.h"
#include "gclib.h"
#include "gclibo.h"
#include "gclib_errors.h"
#include "gclib_record.h"


using namespace std;

GCon g = 0;

void Blink(Galil* galil);
void Travel(Galil* galil);
void Toggle(Galil* galil);

int main() {
    //GCon g = 0; //var used to refer to a unique connection
    EmbeddedFunctions* F = new EmbeddedFunctions;
    Galil* galil = new Galil(F, "192.168.0.120: 25555");

    // Testing

    /////////****************DA******************///////
    // DigitalOutput
    galil->DigitalOutput(0x0000);//DEC: 0 
    galil->DigitalOutput(0x0063);//DEC: 99 
    galil->DigitalOutput(0x00FF);//DEC: 255 
    galil->DigitalOutput(0x0100);//DEC: 256
    galil->DigitalOutput(0xFFFF);//DEC: 65535

    // DigitalByteOutput
    galil->DigitalByteOutput(0, 14);//Set lowbyte
    galil->DigitalByteOutput(1, 14);//Set highbyte

    // DigitalBitOutput
    galil->DigitalBitOutput(0, 5);//Clear 5th Bit
    galil->DigitalBitOutput(1, 5);//Set 5th Bit

    // DigitalInput
    uint16_t DigitalInput = galil->DigitalInput();
    std::cout << "DigitalInput is " << DigitalInput << std::endl;
    uint16_t DigitalInput = galil->DigitalByteInput(0) + galil->DigitalByteInput(1);// Read High+Lowyte

    // DigitalByteInput
    uint16_t DigitalByteInput = galil->DigitalByteInput(0);// Read Lowyte
    uint16_t DigitalByteInput = galil->DigitalByteInput(1);// Read Highyte
    
    std::cout << "DigitalByteInput is " << DigitalByteInput << std::endl;

    // DigitalBitInput
    for (int i = 0; i < 8; i++) {
        galil->DigitalBitInput(i);
    }
    for (int i = 8; i < 16; i++) {
        galil->DigitalBitInput(i);
    }

    // AnalogInputRange
    galil->AnalogInputRange(0, 2);//+- 10v
    galil->AnalogInput(0);

    // AnalogInput
    float AnalogInput = galil->AnalogInput(0);
    std::cout << "AnalogInput is " << AnalogInput << std::endl;

    // AnalogOutput
    for (int i = -10;i < 11;i++) {
        galil->AnalogOutput(7, i);
        Sleep(500);
    }

    /////////****************DA******************///////

    // Check Successful Write
    galil->DigitalOutput(65535);
    std::cout << "Check " << galil->CheckSuccessfulWrite() << std::endl;
    galil->DigitalOutput(65536);
    std::cout << "Check " << galil->CheckSuccessfulWrite() << std::endl;
    galil->DigitalByteOutput(1, 14);
    std::cout << "Check " << galil->CheckSuccessfulWrite() << std::endl;
    galil->DigitalByteOutput(2, 256);
    std::cout << "Check " << galil->CheckSuccessfulWrite() << std::endl;
    galil->DigitalBitOutput(1, 10);
    std::cout << "Check " << galil->CheckSuccessfulWrite() << std::endl;
    galil->DigitalBitOutput(1, 200);
    std::cout << "Check " << galil->CheckSuccessfulWrite() << std::endl;
    galil->AnalogOutput(7, 20);
    std::cout << "Check " << galil->CheckSuccessfulWrite() << std::endl;
    galil->AnalogOutput(7, 10);
    std::cout << "Check " << galil->CheckSuccessfulWrite() << std::endl;
    galil->AnalogOutput(7, 20);
    std::cout << "Check " << galil->CheckSuccessfulWrite() << std::endl;

    // ReadEncoder
    //galil->AnalogOutput(7, 1);
    //Sleep(1000);
    //galil->ReadEncoder();

    // WriteEncoder
    //galil->AnalogOutput(7, 0);
    //Sleep(1000);
    //galil->WriteEncoder();
    //galil->ReadEncoder();

    // PositionControl
    /*galil->AnalogOutput(7, 0);
    galil->WriteEncoder();
    galil->setSetPoint(5000);
    galil->setKp(1.0);
    galil->setKi(0.0);
    galil->setKd(0.0);
    galil->PositionControl(1);*/

    // SpeedControl
    /*galil->AnalogOutput(7, 0);
    galil->WriteEncoder();
    galil->setSetPoint(5000);
    galil->setKp(1.0);
    galil->setKi(0.0);
    galil->setKd(0.0);
    galil->SpeedControl(1);*/

    // Operator Overload
    //cout << *galil;

    // Blink
    /*while (!_kbhit()) {
        Blink(galil);
    }*/

    // Travel
    //Travel(galil);

    // Toggle
    //Toggle(galil);

    if (galil)
        delete(galil);

    return G_NO_ERROR;
}

void Blink(Galil* galil)
{
    galil->DigitalOutput(65535);
    Sleep(500);
    galil->DigitalOutput(0);
    Sleep(500);
}

void Travel(Galil* galil)
{
    for (int i = 0; i < 7; i++) {
        galil->DigitalBitOutput(1, i);
        Sleep(500);
        galil->DigitalBitOutput(0, i);
    }

    galil->DigitalBitOutput(1, 7);
    Sleep(500);
    galil->DigitalBitOutput(0, 7);

    for (int i = 6; i >= 0; i--) {
        galil->DigitalBitOutput(1, i);
        Sleep(500);
        galil->DigitalBitOutput(0, i);
    }
}

void Toggle(Galil* galil) {
    galil->DigitalOutput(0b01110111);
    int key = 1;
    while (key != 0) {
        key = _getch() - 48;
        std::cout << "Key = " << key << std::endl;
        int toggle = galil->DigitalBitInput(key - 1);
        std::cout << "Toggle = " << toggle << std::endl;
        galil->DigitalBitOutput(!toggle, key - 1);
    }
}