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
		07		15sep25	add GetMask

*/

#pragma once

#include "BoundArray.h"
#include "stdint.h"
#include <string>

#define MAX_PITCH_COUNT 12
typedef CBoundArray<int, MAX_PITCH_COUNT> CPitchClassSetBase;

class CPitchClassSet : public CPitchClassSetBase {
public:
// Types
	// A set ID represents a pitch class set as intervals rather than pitches.
	// Each inverval is stored in a nibble, and the set is aligned so that
	// the first interval is in the least significant nibble. For example,
	// the set [0,1,3,5,7,9,A] has a set ID of 0x2221221. Every possible
	// pitch class set has a unique set ID.
	typedef uint64_t SET_ID;	// 64 bits is enough for up to 16 members
	enum {
		SET_ID_MAX_MEMBERS = sizeof(SET_ID) * 8 / 4,
		INTERVALS = 6,
		OCTAVE = 12,
		PRIME_FORMS = 223,	// number of prime forms
		FORTE_FIRST = 7,	// skip sets with less than three members
		FORTE_CODES = 208,	// sets with between three and nine members
		FORTE_LAST = FORTE_FIRST + FORTE_CODES - 1,
	};
	typedef int INTERVAL_VECTOR[INTERVALS];

// Construction
	CPitchClassSet();
	CPitchClassSet(const CPitchClassSet& set);
	CPitchClassSet(const int *pPitch, int nCount);
	CPitchClassSet(SET_ID id);

// Attributes
	void	GetIntervals(CPitchClassSet& set) const;
	void	SetFromId(SET_ID id);
	static	int		GetIntervalCount(SET_ID id);
	void	GetIntervalVector(INTERVAL_VECTOR vec) const;
	int		GetIntervalVector() const;
	SET_ID	GetId() const;
	LPCTSTR	GetForte() const;
	static	SET_ID	GetPrimeId(int iPrime);
	static	LPCTSTR	GetForte(int iPrime);
	bool	operator==(const CPitchClassSet& set) const;
	bool	operator!=(const CPitchClassSet& set) const;
	WORD	GetMask() const;
	WORD	GetMask(int nTranspose) const;

// Operations
	void	Dump() const;
	std::string	FormatSet() const;
	std::string	FormatIntervalVector() const;
	SET_ID	FindNormalForm() const;
	SET_ID	FindPrimeFormFast(SET_ID *pInverse = NULL) const;
	SET_ID	FindPrimeForm(SET_ID *pInverse = NULL);
	void	Normalize();
	void	TransposeToC();
	void	Transpose(int nOffset);
	void	RemoveDuplicates();
	void	MakePrimeForm();
	static	int		FindForte(SET_ID id);
	static	int		FindForte(LPCTSTR pszForte);

protected:
	struct PRIME_FORM {
		SET_ID	idPrime;	// set ID of prime form
		LPCTSTR pszForte;	// prime form's Forte code
	};
	static const PRIME_FORM m_arrPrimeForm[PRIME_FORMS];
};

inline CPitchClassSet::CPitchClassSet()
{
}

inline CPitchClassSet::CPitchClassSet(const CPitchClassSet& Set) : CPitchClassSetBase(Set)
{
}

inline CPitchClassSet::CPitchClassSet(const int *pPitch, int nCount) : CPitchClassSetBase(pPitch, nCount)
{
}

inline CPitchClassSet::CPitchClassSet(SET_ID id)
{
	SetFromId(id);
}

inline CPitchClassSet::SET_ID CPitchClassSet::GetPrimeId(int iPrime)
{
	ASSERT(iPrime >= 0 && iPrime < PRIME_FORMS);
	return m_arrPrimeForm[iPrime].idPrime;
}

inline LPCTSTR CPitchClassSet::GetForte(int iPrime)
{
	ASSERT(iPrime >= 0 && iPrime < PRIME_FORMS);
	return m_arrPrimeForm[iPrime].pszForte;
}

inline bool CPitchClassSet::operator!=(const CPitchClassSet& set) const
{
	return !operator==(set);
}
