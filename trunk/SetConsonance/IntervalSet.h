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

#ifndef CINTERVAL_SET
#define	CINTERVAL_SET

#include "BoundArray.h"
#include "stdint.h"	// standard sizes
#include <string>

#define MAX_PLACES 8
typedef CBoundArray<int, MAX_PLACES> CIntervalSetBase;

#define PACK_BIG_ENDIAN 0

class CIntervalSet : public CIntervalSetBase {
public:
	union SET {
		uint8_t		b[MAX_PLACES];
		uint64_t	dw;
	};
	void	Alloc(const SET& arrRange);
	void	Clear();
	int		Pack() const;
	void	Unpack(int nPackedVal);
	void	Dump() const;
	int		GetRange(int iPlace) const;
	void	GetRange(SET& range) const;
	int		GetPermutationCount() const;
	int		GetSum() const;
	int		GetRangeSum() const;
	std::string	FormatSet() const;
	static	int		CountPlaces(const SET& set);

protected:
	CIntervalSetBase	m_arrRange;
};

inline int CIntervalSet::GetRange(int iPlace) const
{
	return m_arrRange[iPlace];
}

#endif
