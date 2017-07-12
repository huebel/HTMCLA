#include "Classifier.h"
#include <cassert>

namespace htm {

Classifier::Classifier(QString &_id, int _numItems, QString _regionID, QString _inputspaceID, QStringList &_labels)
    : DataSpace(_id), regionID(_regionID), inputspaceID(_inputspaceID), numItems(_numItems), labels(_labels)
{
}

int Classifier::GetSizeX()
{
	return 1;
}

int Classifier::GetSizeY()
{
	return numItems;
}

int Classifier::GetNumValues()
{
	return 1;
}

int Classifier::GetHypercolumnDiameter() 
{
	return 1;
}

bool Classifier::GetIsActive(int _x, int _y, int _index)
{
	assert(_x == 0);
	assert((_y >= 0) && (_y < numItems));
	assert(_index == 0);

	// TEMP
	return false;
}

}

