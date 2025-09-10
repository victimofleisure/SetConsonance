// Copyleft 2023 Chris Korda
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or any later version.
/*
        chris korda
 
		revision history:
		rev		date	comments
        00      16jan23	initial version

*/

#pragma once

#include "BoundArray.h"

class CBGSet {	// balanced Gray code set, computed via BalaGray app
public:
	enum {
		MAX_SET_DIGITS = 8
	};
	class CSetIDArray : public CBoundArray<BYTE, MAX_SET_DIGITS> {
	public:
		CSetIDArray() {};
		CSetIDArray(UINT nSetCode) { SetCode(nSetCode); }
		void	SetCode(UINT nSetCode);
		UINT	GetCode() const;
	};
	CBGSet();
	CArray<CByteArray, CByteArray&> m_arrRow;	// array of balanced Gray sequences
	CSetIDArray	m_arrSetID;	// set identifier; bound array of digit ranges
	UINT	m_nCode;		// set identifier; one digit range per nibble
	int		m_nDigits;		// total number of digits
	int		m_nRange;		// total range; sum of digit ranges
	int		m_nStates;		// number of states; product of digit ranges
	int		m_nBalance;		// imbalance between digits; zero is ideal
	int		m_nMaxTrans;	// maximum number of transitions
	int		m_nMaxSpan;		// maximum span length
	bool	m_bProven;		// true if optimality is proven
	void	DumpAttributes() const;
	void	DumpRows() const;
	CString	GetName() const;
	static CString GetName(UINT nCode);
	static bool GetCode(LPCTSTR sName, UINT& nCode);
	CString	GetRowCSV(int iDigit, int nOffset = 0, TCHAR cSeparator = ',') const;
	bool	ReadSetData(UINT nSetCode, LPCTSTR pszSetFolderPath);
	static	void	Canonicalize(const CSetIDArray& arrSetID, CSetIDArray& arrCanonicalSetID, CSetIDArray& arrCanonicalDigitIdx);
};

typedef CArray<CBGSet, CBGSet&> CBGSetArray;
