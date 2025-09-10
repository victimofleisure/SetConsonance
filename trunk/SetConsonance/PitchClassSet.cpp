// Copyleft 2015 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      08jun15	initial version
        01      27dec18	refactor and add Forte codes
		02		08apr19	add format interval vector
		03		22oct19	overload FindForte
		04		17jan23	improve portability
		05		26jul24	FindForte now returns index of prime form
		06		19sep24	add symbol column to Forte definitions
		07		06aug25	use square brackets instead of parentheses

*/

#include "stdafx.h"
#include "PitchClassSet.h"

const CPitchClassSet::PRIME_FORM CPitchClassSet::m_arrPrimeForm[PRIME_FORMS] = {
	#define FORTEDEF(id, name, sym) {id, _T(name)},
	#include "ForteDef.h"
};

void CPitchClassSet::GetIntervals(CPitchClassSet& set) const
{
	int	nSize = GetSize();
	ASSERT(nSize > 0);
	set.SetSize(nSize);
	for (int iElem = 0; iElem < nSize - 1; iElem++) {
		int	delta = GetAt(iElem + 1) - GetAt(iElem);
		ASSERT(delta > 0);	// notes must be in ascending order
		set[iElem] = delta;
	}
	set[nSize - 1] = OCTAVE - GetAt(nSize - 1);
}

void CPitchClassSet::Dump() const
{
	int	nSize = GetSize();
	fputs("[", stdout);
	for (int iElem = 0; iElem < nSize; iElem++) {
		if (iElem)
			fputc(',', stdout);
		printf("%X", GetAt(iElem));
	}
	fputs("]\n", stdout);
}

std::string CPitchClassSet::FormatSet() const
{
	int	nSize = GetSize();
	std::string	sResult, sElem;
	sResult = '[';
	char	szVal[4];
	for (int iElem = 0; iElem < nSize; iElem++) {
		if (iElem)
			sResult += ',';
		sprintf_s(szVal, "%X", GetAt(iElem));
		sElem = szVal;
		sResult += sElem;
	}
	sResult += ']';
	return sResult;
}

std::string CPitchClassSet::FormatIntervalVector() const
{
	INTERVAL_VECTOR	vec;
	GetIntervalVector(vec);
	TCHAR	szIntVec[INTERVALS + 1] = {0};
	for (int i = 0; i < INTERVALS; i++) {
		szIntVec[i] = vec[i] >= 10 ? vec[i] - 10 + 'A' : vec[i] + '0';
	}
	return szIntVec;
}

CPitchClassSet::SET_ID CPitchClassSet::FindNormalForm() const
{
	int	nSize = GetSize();
	ASSERT(nSize < SET_ID_MAX_MEMBERS);
	SET_ID	norm = 0;
	for (int iRot = 0; iRot < nSize; iRot++) {
		SET_ID	sum = 0;
		SET_ID	place = 1;
		for (int iElem = 0; iElem < nSize; iElem++) {
			int	i = iRot + iElem;
			if (i >= nSize)
				i -= nSize;
			sum += GetAt(i) * place;
			place <<= 4;	// next nibble
		}
		if (sum > norm)
			norm = sum;
	}
	return(norm);
}

int CPitchClassSet::GetIntervalCount(SET_ID id)
{
	SET_ID	mask = 0xf;
	for (int iPlace = 0; iPlace < SET_ID_MAX_MEMBERS; iPlace++) {
		if (!(id & mask))
			return(iPlace);
		mask <<= 4;	// next nibble
	}
	return(SET_ID_MAX_MEMBERS);
}

void CPitchClassSet::SetFromId(SET_ID id)
{
	int	nSize = GetIntervalCount(id);
	SetSize(nSize);
	int	note = 0;
	for (int iElem = 0; iElem < nSize; iElem++) {
		SetAt(iElem, note);
		note += int(id & 0xf);	// mask off low-order nibble
		id >>= 4;	// next nibble
	}
}

CPitchClassSet::SET_ID CPitchClassSet::FindPrimeFormFast(SET_ID *pInverse) const
{
	// the input set is assumed to contain intervals, not pitches
	SET_ID	Norm = FindNormalForm();
	CPitchClassSet	InvSet(*this);
	InvSet.MakeReverse();
	SET_ID	InvNorm = InvSet.FindNormalForm();
	if (InvNorm > Norm) {
		if (pInverse != NULL)
			*pInverse = Norm;
		return(InvNorm);
	} else {
		if (pInverse != NULL)
			*pInverse = InvNorm;
		return(Norm);
	}
}

void CPitchClassSet::Normalize()
{
	int	nSize = GetSize();
	for (int iElem = 0; iElem < nSize; iElem++)
		GetAt(iElem) %= OCTAVE;
}

void CPitchClassSet::TransposeToC()
{
	// assume set is already normalized and sorted
	int	nSize = GetSize();
	int	offset = GetAt(0);
	for (int iElem = 0; iElem < nSize; iElem++)
		GetAt(iElem) -= offset;
}

void CPitchClassSet::RemoveDuplicates()
{
	// assume set is sorted
	int	nSize = GetSize();
	for (int iElem = nSize - 1; iElem >= 1; iElem--) {
		if (GetAt(iElem) == GetAt(iElem - 1))
			RemoveAt(iElem);
	}
}

CPitchClassSet::SET_ID CPitchClassSet::FindPrimeForm(SET_ID *pInverse)
{
	if (IsEmpty()) {
		if (pInverse != NULL)
			*pInverse = 0;
		return(0);
	}
	Normalize();
	Sort();
	TransposeToC();
	RemoveDuplicates();
	CPitchClassSet	interval;
	GetIntervals(interval);
	return(interval.FindPrimeFormFast(pInverse));
}

void CPitchClassSet::GetIntervalVector(INTERVAL_VECTOR vec) const
{
	ZeroMemory(vec, sizeof(INTERVAL_VECTOR));
	int	nSize = GetSize();
	for (int i = 0; i < nSize - 1; i++) {
		for (int j = i + 1; j < nSize; j++) {
			int	iInt = GetAt(j) - GetAt(i);
			if (iInt > INTERVALS)
				iInt = OCTAVE - iInt;
			vec[iInt - 1]++;
		}
	}
}

int CPitchClassSet::GetIntervalVector() const
{
	INTERVAL_VECTOR vec;
	GetIntervalVector(vec);
	int	iVec = 0;
	for (int iInt = 0; iInt < INTERVALS; iInt++)
		iVec |= vec[iInt] << ((INTERVALS - 1 - iInt) * 4);
	return(iVec);
}

void CPitchClassSet::MakePrimeForm()
{
	*this = CPitchClassSet(FindPrimeForm());
}

CPitchClassSet::SET_ID CPitchClassSet::GetId() const
{
	CPitchClassSet	set;
	GetIntervals(set);
	int	nSize = GetSize();
	ASSERT(nSize < SET_ID_MAX_MEMBERS);
	SET_ID	id = 0;
	for (int iElem = nSize - 1; iElem >= 0; iElem--) {
		id <<= 4;
		id |= set[iElem];
	}
	return id;
}

int CPitchClassSet::FindForte(SET_ID id)
{
	for (int iPrime = 0; iPrime < _countof(m_arrPrimeForm); iPrime++) {
		if (id == m_arrPrimeForm[iPrime].idPrime)
			return iPrime;
	}
	return -1;
}

int CPitchClassSet::FindForte(LPCTSTR pszForte)
{
	for (int iPrime = 0; iPrime < _countof(m_arrPrimeForm); iPrime++) {
		if (!_tcsicmp(pszForte, m_arrPrimeForm[iPrime].pszForte))
			return iPrime;
	}
	return -1;
}

LPCTSTR	CPitchClassSet::GetForte() const
{
	// assume set is prime form
	return GetForte(FindForte(GetId()));
}

bool CPitchClassSet::operator==(const CPitchClassSet& set) const
{
	int	nSize = GetSize();
	if (set.GetSize() != nSize)
		return false;
	for (int iElem = 0; iElem < nSize; iElem++) {
		if (set.GetAt(iElem) != GetAt(iElem))
			return false;
	}
	return true;
}
