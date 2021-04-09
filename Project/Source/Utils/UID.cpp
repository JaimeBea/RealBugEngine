#include "UID.h"

#include <random>
#include <sstream>
#include <string>

#include "Utils/Leaks.h"

static std::random_device random;
static std::mt19937_64 mersenneTwister(random());
static std::uniform_int_distribution<UID> distribution;
static std::uniform_int_distribution<UID> distribution_hex(0, 15);
static std::uniform_int_distribution<UID> distribution_hex_2(8, 11);

UID GenerateUID() {
	return distribution(mersenneTwister);
}

std::string GenerateUID128() {
	
	std::stringstream ss;
	
	ss << std::hex;
	ss << std::uppercase;

	for (int i = 0; i < 8; i++) {
		ss << distribution_hex(mersenneTwister);
	}
	
	ss << "-";
	for (int i = 0; i < 4; i++) {
		ss << distribution_hex(mersenneTwister);
	}
	
	ss << "-4";
	for (int i = 0; i < 3; i++) {
		ss << distribution_hex(mersenneTwister);
	}
	
	ss << "-";
	ss << distribution_hex_2(mersenneTwister);
	
	for (int i = 0; i < 3; i++) {
		ss << distribution_hex(mersenneTwister);
	}

	ss << "-";
	
	for (int i = 0; i < 12; i++) {
		ss << distribution_hex(mersenneTwister);
	};

	return ss.str();
}