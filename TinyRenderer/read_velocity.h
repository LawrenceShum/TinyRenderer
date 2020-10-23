#pragma once
#include <fstream>
#include <iostream>

using namespace std;

class ReadVelocity
{
private:
	const char* filename;


public:
	ReadVelocity(const char*);

	~ReadVelocity();
};