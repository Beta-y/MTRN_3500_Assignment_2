#include "EmbeddedFunctions.h"
#include "Galil.h"

//Code from https://github.com/IHadinoto/Object-Oriented-PLC
Galil::Galil()
{
	Functions = new EmbeddedFunctions;//Pointer to EmbeddedFunctions, through which all Galil Function calls will be made
	g = 0; //Connection handle for the Galil, passed through most Galil function calls
	for (int i = 0; i < 1024; ReadBuffer[i] = NULL, i++);
	for (int i = 0; i < 3; ControlParameters[i] = 0, i++);
	setPoint = 0; //Position Control Setpoint
}

Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address)
{
	Functions = new EmbeddedFunctions;//Pointer to EmbeddedFunctions, through which all Galil Function calls will be made
	g = 0; //Connection handle for the Galil, passed through most Galil function calls
	for (int i = 0; i < 1024; ReadBuffer[i] = NULL, i++);
	for (int i = 0; i < 3; ControlParameters[i] = 0, i++);
	setPoint = 0;

	GReturn status = Functions->GOpen(address, &g);
}

Galil::~Galil()
{
	if (g)
	{
		Functions->GClose(g);
		delete Functions;
	}
}

// Page 137
void Galil::DigitalOutput(uint16_t value) {
	char Command[128] = "";
	ReadBuffer[0] = NULL;

	if (value < 256) {
		sprintf_s(Command, "OP %d,0;", value);
	}
	else if (value >= 256 && value < 65536) {
		int highbyte = value / 256;// 2^8, 0xFF00
		int lowbyte = value % 256; // 0x00FF
		sprintf_s(Command, "OP %d,%d;", lowbyte, highbyte);
	}
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
}
// Page 137
void Galil::DigitalByteOutput(bool bank, uint8_t value)
{
	char Command[128] = "";
	ReadBuffer[0] = NULL;

	if (bank == 0) {
		sprintf_s(Command, "OP %d;", value);//low
	}
	else {
		sprintf_s(Command, "OP ,%d;", value);//high, Watch out the ","
	}
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
}
// Page 73& CB:Clear Bit, SB:Setr Bit
void Galil::DigitalBitOutput(bool val, uint8_t bit)			// Write single bit to digital outputs. 'bit' specifies which bit
{
	char Command[128] = "";
	ReadBuffer[0] = { NULL };

	if (val == 0) {
		sprintf_s(Command, "CB %d;", bit);
		Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
	}
	else {
		sprintf_s(Command, "SB %d;", bit);
		Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
	}
}

// DIGITAL INPUTS
uint16_t Galil::DigitalInput()			// Return the 16 bits of input data
{
	double Input = 0;
	int j = 0;
	for (int i = 0; i < 16; i++) {
		Input += pow(2 * int(DigitalBitInput(i)), j);
		j++;
	}
	if (DigitalBitInput(0) == 0) {
		Input--;
	}
	return Input;
}
uint8_t Galil::DigitalByteInput(bool bank)	// Read either high or low byte, as specified by user in 'bank'
{
	double Input = 0;
	int j = 0;
	if (bank == 0) {
		for (int i = 0; i < 8; i++) {
			Input += pow(2 * int(DigitalBitInput(i)), j);
			j++;
		}
		if (DigitalBitInput(0) == 0) {
			Input--;
		}
	}
	else {
		for (int i = 8; i < 16; i++) {
			Input += pow(2 * int(DigitalBitInput(i)), j);
			j++;
		}
		if (DigitalBitInput(8) == 0) {
			Input--;
		}
	}
	return Input;
}
//P125&P37 
bool Galil::DigitalBitInput(uint8_t bit)		// Read single bit from current digital inputs. Above functions
{
	char Command[128] = "";
	sprintf_s(Command, "MG @IN[%d];", bit);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
	std::string* Response = new std::string(ReadBuffer);
	if (atoi(Response->c_str()) == 0) { //atoi:Convert str to int
		bit = true;
	}
	else {
		bit = false;
	}
	return bit;
}

bool Galil::CheckSuccessfulWrite()	// Check the string response from the Galil to check that the last 
{
	std::string* Response = new std::string(ReadBuffer);
	std::string convert = Response->c_str();
	std::size_t found = convert.find(':');
	if (found == std::string::npos) {
		return false;
	}
	else {
		return true;
	}
}

// ANALOG FUNCITONS
//P125&P29
float Galil::AnalogInput(uint8_t channel)						// Read Analog channel and return voltage			
{
	float voltage = 0.0;
	char Command[128] = "";
	sprintf_s(Command, "MG @AN[%d];", channel);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
	std::string* Response = new std::string(ReadBuffer);
	voltage = std::stof(Response->c_str());
	return voltage;
}
//P65
void Galil::AnalogOutput(uint8_t channel, double voltage)		// Write to any channel of the Galil, send voltages as
{
	char Command[128] = "";
	ReadBuffer[0] = { NULL };
	sprintf_s(Command, "AO %d,%f;", channel, voltage);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
}
//P64
void Galil::AnalogInputRange(uint8_t channel, uint8_t range)	// Configure the range of the input channel with
{
	char Command[128] = "";
	sprintf_s(Command, "AQ %d,%d;", channel, range);
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
}

// ENCODER / CONTROL FUNCTIONS
//P184
void Galil::WriteEncoder()				// Manually Set the encoder value to zero
{
	char Command[128] = "";
	sprintf_s(Command, "WE 0,0;");
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
}
//P147
int Galil::ReadEncoder()					// Read from Encoder
{
	int encoder = 0;
	char Command[128] = "";
	sprintf_s(Command, "QE;");
	Functions->GCommand(g, Command, ReadBuffer, sizeof(ReadBuffer), 0);
	std::string* Response = new std::string(ReadBuffer);
	encoder = std::stoi(Response->c_str());
	return encoder;
}
void Galil::setSetPoint(int s)			// Set the desired setpoint for control loops, counts or counts/sec
{
	setPoint = s;
}
void Galil::setKp(double gain)			// Set the proportional gain of the controller used in controlLoop()
{
	ControlParameters[0] = gain;
}
void Galil::setKi(double gain)			// Set the integral gain of the controller used in controlLoop()
{
	ControlParameters[1] = gain;
}
void Galil::setKd(double gain)			// Set the derivative gain of the controller used in controlLoop()
{
	ControlParameters[2] = gain;
}

//Already Defined
//void Galil::PositionControl(bool debug);	// Run the control loop. ReadEncoder() is the input to the loop. The motor is the output.

//void Galil::SpeedControl(bool debug)		// same as above. Setpoint interpreted as counts per second

// Operator overload for '<<' operator. So the user can say cout << Galil; This function should print out the
// output of GInfo and GVersion, with two newLines after each.
std::ostream& operator<<(std::ostream& output, Galil& galil)
{
	GCon g = galil.g;
	char buf[1024];
	std::string* Version;
	std::string* Info;

	GVersion(buf, sizeof(buf));
	Version = new std::string(buf);

	GInfo(g, buf, sizeof(buf));
	Info = new std::string(buf);

	output << "GVersion Test: " << Version->c_str() << std::endl << std::endl << std::endl << "GInfo Test: " << Info->c_str() << std::endl << std::endl << std::endl;
	return output;
}
