#pragma once

// Field value constants for individual areas on gameboard

constexpr auto FV_Mine = 0x0001;									// A mine is on the field
constexpr auto FV_Flag = 0x0002;									// The field is suggested to contain mine, a flag is drawn after right click
constexpr auto FV_Ask = 0x0004;									// Question mark is drawn after second right click (not yet ready)
constexpr auto FV_0 = 0x0008;									// There is no mine in the surrounding fields
constexpr auto FV_1 = 0x0010;									// There is 1 mine in the surrounding fields
constexpr auto FV_2 = 0x0020;									// ...
constexpr auto FV_3 = 0x0040;									// And so on ...
constexpr auto FV_4 = 0x0080;									// ...
constexpr auto FV_5 = 0x0100;									//
constexpr auto FV_6 = 0x0200;									// There's six ...
constexpr auto FV_7 = 0x0400;									// ...
constexpr auto FV_8 = 0x0800;									// All of the surrounding fields contain a mine
constexpr auto FV_Clear = 0x1000;									// Field is cleared, wheter it contains mine or not
constexpr auto FV_Change = 0x2000;									// Not used anymore
constexpr auto FV_Pushed = 0x4000;									// Indicates if left button is down on a field

class Field
{
public:

	Field() { Value = 0; }
	Field(int value) { Value = value; }

	bool hasMine() { return Value & FV_Mine; }
	void setMine() {
		int x = Value;
		x |= FV_Mine;
		Value = x;
		Value |= FV_Mine; }
	bool hasFlag() { return Value & FV_Flag; }
	void switchFlag() { Value ^= FV_Flag; }
	bool isClear() { return Value & FV_Clear; }
	void setClear() { Value |= FV_Clear; }
	bool isPushed() { return Value & FV_Pushed; }
	void setPushed() { Value |= FV_Pushed; }
	void deletePushed() { Value &= ~FV_Pushed; }
	bool is0() { return Value & FV_0; }
	bool is1() { return Value & FV_1; }
	bool is2() { return Value & FV_2; }
	bool is3() { return Value & FV_3; }
	bool is4() { return Value & FV_4; }
	bool is5() { return Value & FV_5; }
	bool is6() { return Value & FV_6; }
	bool is7() { return Value & FV_7; }
	bool is8() { return Value & FV_8; }
	void set0() { Value |= FV_0; }
	void set1() { Value |= FV_1; }
	void set2() { Value |= FV_2; }
	void set3() { Value |= FV_3; }
	void set4() { Value |= FV_4; }
	void set5() { Value |= FV_5; }
	void set6() { Value |= FV_6; }
	void set7() { Value |= FV_7; }
	void set8() { Value |= FV_8; }
	void reset() { Value = 0; }

private:

	short int Value;

};

