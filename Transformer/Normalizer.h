/*
 * Normalizer.h
 *
 *  Created on: Dec 17, 2013
 *      Author: Biniam Fisseha Demissie
 */

#ifndef NORMALIZER_H_
#define NORMALIZER_H_

class Normalizer {
public:
	Normalizer(std::string filename) : file(filename)

	{}

	void setFile(const char *filename)
	{
		this->file = filename;
	}

	bool Transform()
	{
		if (this->file.length() != 0) {
			NormalizerMain();
			return true;
		}
		else
			return false;
	}

//	~Normalizer();

private:
	std::string file;

	void NormalizerMain();
};

#endif /* NORMALIZER_H_ */
