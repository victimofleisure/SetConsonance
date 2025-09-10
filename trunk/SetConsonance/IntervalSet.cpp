// Copyleft 2023 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      17jan23	initial version

*/

#include "stdafx.h"
#include "IntervalSet.h"
#include <string>

void CIntervalSet::Alloc(const SET& arrRange)
{
	int	nPlaces = CountPlaces(arrRange);
	SetSize(nPlaces);
	m_arrRange.SetSize(nPlaces);
	for (int iPlace = 0; iPlace < nPlaces; iPlace++) {
		m_arrRange[iPlace] = arrRange.b[iPlace];
		SetAt(iPlace, 0);
	}
}

int	CIntervalSet::CountPlaces(const SET& set)
{
	for (int iPlace = 0; iPlace < MAX_PLACES; iPlace++) {
		if (!set.b[iPlace])
			return iPlace;
	}
	return MAX_PLACES;
}

void CIntervalSet::Clear()
{
	int	nPlaces = GetSize();
	for (int iPlace = 0; iPlace < nPlaces; iPlace++) {
		SetAt(iPlace, 0);
	}
}

int CIntervalSet::Pack() const
{
	int	nPackedVal = 0;
	int	nPlaces = GetSize();
#if PACK_BIG_ENDIAN
	for (int iPlace = 0; iPlace < nPlaces; iPlace++) {
#else
	for (int iPlace = nPlaces - 1; iPlace >= 0; iPlace--) {
#endif
		nPackedVal *= m_arrRange[iPlace];
		nPackedVal += GetAt(iPlace);
	}
	return nPackedVal;
}

void CIntervalSet::Unpack(int nPackedVal)
{
	int	nPlaces = GetSize();
#if PACK_BIG_ENDIAN
	for (int iPlace = nPlaces - 1; iPlace >= 0; iPlace--) {
#else
	for (int iPlace = 0; iPlace < nPlaces; iPlace++) {
#endif
		int	nRange = m_arrRange[iPlace];
		SetAt(iPlace, nPackedVal % nRange);
		nPackedVal /= nRange;
	}
}

void CIntervalSet::Dump() const
{
	printf("[");
	int	nPlaces = GetSize();
	for (int iPlace = 0; iPlace < nPlaces; iPlace++) {
		if (iPlace)
			printf(" ");
		printf("%X", GetAt(iPlace));
	}
	printf("]\n");
}

std::string CIntervalSet::FormatSet() const
{
	int	nPlaces = GetSize();
	std::string	sResult, sPlace;
	sResult = '(';
	char	szVal[4];
	for (int iPlace = 0; iPlace < nPlaces; iPlace++) {
		if (iPlace)
			sResult += ',';
		sprintf_s(szVal, "%X", GetAt(iPlace));
		sPlace = szVal;
		sResult += sPlace;
	}
	sResult += ')';
	return sResult;
}

int CIntervalSet::GetPermutationCount() const
{
	int	nPlaces = GetSize();
	int	nPerms = m_arrRange[0];
	for (int iPlace = 1; iPlace < nPlaces; iPlace++) {
		nPerms *= m_arrRange[iPlace];
	}
	return nPerms;
}

void CIntervalSet::GetRange(SET& range) const
{
	range.dw = 0;
	int	nPlaces = GetSize();
	for (int iPlace = 0; iPlace < nPlaces; iPlace++) {
		range.b[iPlace] = m_arrRange[iPlace];
	}
}

int CIntervalSet::GetSum() const
{
	int	nSum = 0;
	int	nPlaces = GetSize();
	for (int iPlace = 0; iPlace < nPlaces; iPlace++) {
		nSum += GetAt(iPlace);
	}
	return nSum;
}

int CIntervalSet::GetRangeSum() const
{
	int	nSum = 0;
	int	nPlaces = GetSize();
	for (int iPlace = 0; iPlace < nPlaces; iPlace++) {
		nSum += m_arrRange[iPlace];
	}
	return nSum;
}
