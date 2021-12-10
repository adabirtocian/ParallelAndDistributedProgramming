#include <iostream>
#include "KaratsubaAlgorithm.hpp"

KaratsubaAlgorithm::KaratsubaAlgorithm(std::vector<int> polynomial1, std::vector<int> polynomial2, int size1, int size2) :
	polynomial1{ polynomial1 }, polynomial2{ polynomial2 }, size1{ size1 }, size2{ size2 }
{
}

std::vector<int> KaratsubaAlgorithm::karatsubaAlgorithm(std::vector<int> polynomial1, std::vector<int> polynomial2)
{
	if (polynomial1.size() <= 4 || polynomial2.size() <= 4)
	{
		RegularAlgorithm r = RegularAlgorithm(polynomial1, polynomial2, polynomial1.size(), polynomial2.size());
		r.multiply();

		return r.getResult();
	}

	int smallerLen = (polynomial1.size() > polynomial2.size() ? polynomial1.size() : polynomial2.size()) / 2;

	// split the polynomials in two parts
	std::vector<int> smallCoeffP1 = this->slice(0, smallerLen - 1, polynomial1);
	std::vector<int> bigCoeffP1 = this->slice(smallerLen, polynomial1.size()-1, polynomial1);

	std::vector<int> smallCoeffP2 = this->slice(0, smallerLen - 1, polynomial2);
	std::vector<int> bigCoeffP2 = this->slice(smallerLen, polynomial2.size() - 1, polynomial2);

	std::vector<int> sumPartsP1 = this->add2Polynomials(smallCoeffP1, bigCoeffP1);
	std::vector<int> sumPartsP2 = this->add2Polynomials(smallCoeffP2, bigCoeffP2);

	// decompose in smaller parts
	std::vector<int> smallCoeffs = this->karatsubaAlgorithm(smallCoeffP1, smallCoeffP2);
	std::vector<int> sumPartsCoeffs = this->karatsubaAlgorithm(sumPartsP1, sumPartsP2);
	std::vector<int> bigCoeffs = this->karatsubaAlgorithm(bigCoeffP1, bigCoeffP2);
	std::vector<int> middleCoeffs = this->substract2Polynomials(this->substract2Polynomials(sumPartsCoeffs, smallCoeffs), bigCoeffs);

	// aggregate the resulting polynomial
	std::vector<int> result1 = this->shift(bigCoeffs, 2 * smallerLen);
	std::vector<int> result2 = this->shift(middleCoeffs, smallerLen);
	std::vector<int> result = this->add2Polynomials(this->add2Polynomials(result1, result2), smallCoeffs);

	return result;
}

std::vector<int> KaratsubaAlgorithm::slice(int start, int stop, std::vector<int> original)
{
	std::vector<int> slice;
	for (int i = start; i <= stop; ++i)
	{
		slice.push_back(original[i]);
	}
	
	return slice;
}

std::vector<int> KaratsubaAlgorithm::add2Polynomials(std::vector<int> polynomial1, std::vector<int> polynomial2)
{
	std::vector<int> result;
	int smallerLen = polynomial1.size() < polynomial2.size() ? polynomial1.size() : polynomial2.size();
	for (int i = 0; i < smallerLen; ++i)
	{
		result.push_back(polynomial1[i] + polynomial2[i]);
	}

	for (int i = smallerLen; i < polynomial1.size(); ++i)
	{
		result.push_back(polynomial1[i]);
	}

	for (int i = smallerLen; i < polynomial2.size(); ++i)
	{
		result.push_back(polynomial2[i]);

	}
	return result;
}

std::vector<int> KaratsubaAlgorithm::substract2Polynomials(std::vector<int> polynomial1, std::vector<int> polynomial2)
{
	std::vector<int> result;
	int smallerLen = polynomial1.size() < polynomial2.size() ? polynomial1.size() : polynomial2.size();
	for (int i = 0; i < smallerLen; ++i)
	{
		result.push_back(polynomial1[i] - polynomial2[i]);
	}

	for (int i = smallerLen; i < polynomial1.size(); ++i)
	{
		result.push_back(polynomial1[i]);
	}

	for (int i = smallerLen; i < polynomial2.size(); ++i)
	{
		result.push_back(polynomial2[i]);

	}
	return result;
}

std::vector<int> KaratsubaAlgorithm::shift(std::vector<int> original, int offset)
{
	std::vector<int> shifted = std::vector<int>(offset, 0);
	for (int i = 0; i < original.size(); ++i)
	{
		shifted.push_back(original[i]);
	}

	return shifted;
}

void KaratsubaAlgorithm::multiply()
{
	std::vector<int> result = this->karatsubaAlgorithm(this->polynomial1, this->polynomial2);
	for (auto coeff : result)
	{
		this->result.push_back(coeff);
	}
}


std::string KaratsubaAlgorithm::getPolynomialResult()
{
	std::string result = "";
	for (int i = this->result.size() - 1; i >= 0; --i)
	{
		result += std::to_string(this->result[i]) + " * x^" + std::to_string(i) + " + ";
	}
	result.erase(result.size() - 3, 3);
	return result;
}
