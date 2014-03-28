/*
 * io.h
 *
 *  Created on: Mar 27, 2014
 *      Author: vlad
 */

#ifndef IO_H_
#define IO_H_

// input/output

#include <stdexcept>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "types.h"


namespace qpp
{
//TODO: use 1.0e+05 notation

// Displays an Eigen::MatrixX in friendly form
template<typename Derived>
void disp(const Eigen::MatrixBase<Derived> &A, unsigned int precision = 4,
		double chop = ct::chop, std::ostream& os = std::cout)
{
//std::cout << "typeid: " << typeid(Derived).name() << std::endl;
	os << std::setprecision(precision) << std::fixed << A;
}

template<> // complex matrix specialization
inline void disp(const Eigen::MatrixBase<Eigen::MatrixXcd> &A,
		unsigned int precision, double chop, std::ostream& os)

{
	if (A.rows() * A.cols() == 0)
	{
		os << "Empty [" << A.rows() << " x " << A.cols() << "] matrix.";
		os << std::endl;
		return;
	};

	std::ostringstream ostr;
	std::vector<std::string> vstr;
	std::string strtmp;

	for (int i = 0; i < A.rows(); i++)
	{
		for (int j = 0; j < A.cols(); j++)
		{
			strtmp.clear(); // clear the temporary string
			ostr.clear();
			ostr.str(std::string()); // clear the ostringstream,

			double re = static_cast<types::cplx>(A(i, j)).real();
			double im = static_cast<types::cplx>(A(i, j)).imag();

			if (std::abs(re) < chop && std::abs(im) < chop)
			{
				vstr.push_back("0 ");
			}
			else if (std::abs(re) < chop)
			{
				ostr << std::setprecision(precision) << std::fixed << im;
				vstr.push_back(ostr.str() + "i");
			}
			else if (std::abs(im) < chop)
			{
				ostr << std::setprecision(precision) << std::fixed << re;
				vstr.push_back(ostr.str() + " ");
			}
			else
			{
				ostr << std::setprecision(precision) << std::fixed << re;
				strtmp = ostr.str();

				strtmp += (im > 0 ? " + " : " - ");
				ostr.clear();
				ostr.str(std::string()); // clear
				ostr << std::setprecision(precision) << std::fixed
						<< std::abs(im);
				strtmp += ostr.str();
				strtmp += "i";
				vstr.push_back(strtmp);
			}
		}
	}

// determine the maximum lenght of the entries in each column
	std::vector<size_t> maxlengthcols(A.cols(), 0);

	for (int i = 0; i < A.rows(); i++)
		for (int j = 0; j < A.cols(); j++)
			if (vstr[i * A.cols() + j].size() > maxlengthcols[j])
				maxlengthcols[j] = vstr[i * A.cols() + j].size();

// finally display it!
	for (int i = 0; i < A.rows(); i++)
	{
		os << std::setw(static_cast<int>(maxlengthcols[0])) << std::right
				<< vstr[i * A.cols()]; // display first column
		for (int j = 1; j < A.cols(); j++) // then the rest
			os << std::setw(static_cast<int>(maxlengthcols[j] + 2))
					<< std::right << vstr[i * A.cols() + j];
		if (i < A.rows() - 1)
			os << std::endl;
	}
}

// Displays a complex number in friendly form
inline void disp(const types::cplx c, unsigned int precision = 4, double chop =
		ct::chop, std::ostream& os = std::cout)
{
// put the complex number inside an Eigen matrix
	Eigen::MatrixXcd tmp(1, 1);
	tmp(0, 0) = c;
	disp(tmp, precision, chop, os);
}

// save matrix to a binary file in double precision
template<typename Derived>
void save(const Eigen::MatrixBase<Derived> & A, const std::string& fname)
{
	std::fstream fout;
	fout.open(fname.c_str(), std::ios::out | std::ios::binary);

	if (fout.fail())
	{
		throw std::runtime_error(
				"save: Error writing output file \"" + std::string(fname)
						+ "\"!");
	}
// we write a header to the file
	const char _header[] = "TYPE::Eigen::Matrix";
	fout.write(_header, sizeof(_header));

	size_t rows = A.rows();
	size_t cols = A.cols();
	fout.write((char*) &rows, sizeof(rows));
	fout.write((char*) &cols, sizeof(cols));
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			fout.write((char*) &A(i, j), sizeof(A(i, j)));
	fout.close();
}

// load matrix from binary file
template<typename Derived>
Derived load(const std::string& fname)
{
	std::fstream fin;
	fin.open(fname.c_str(), std::ios::in | std::ios::binary);

	if (fin.fail())
	{
		throw std::runtime_error(
				"load: Error opening input file \"" + std::string(fname)
						+ "\"!");
	}

	const char _header[] = "TYPE::Eigen::Matrix"; // we write the header to the file
	char *_fheader = new char[sizeof(_header)]; // include the zero-termiation string

// read the header
	fin.read(_fheader, sizeof(_header));
	if (strcmp(_fheader, _header))
	{
		delete[] _fheader;
		throw std::runtime_error(
				"load: Input file \"" + std::string(fname)
						+ "\" is corrupted!");
	}
	delete[] _fheader;

	size_t rows, cols;
	fin.read((char*) &rows, sizeof(rows));
	fin.read((char*) &cols, sizeof(cols));

	Derived A(rows, cols);
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			fin.read((char*) &A(i, j), sizeof(A(i, j)));
	fin.close();
	return A;
}




}

#endif /* IO_H_ */
