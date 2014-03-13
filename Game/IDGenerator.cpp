#include "IDGenerator.hpp"

std::default_random_engine CIDGenerator::m_generator;
std::uniform_int_distribution<long> CIDGenerator::m_distribution(-RAND_MAX, RAND_MAX);
