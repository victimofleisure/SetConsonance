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

#include "stdafx.h"
#include "BGSet.h"
#include "BoundArray.h"

CBGSet::CBGSet()
{
	m_nCode = 0;
	m_nDigits = 0;
	m_nRange = 0;
	m_nStates = 0;
	m_nBalance = 0;
	m_nMaxTrans = 0;
	m_nMaxSpan = 0;
	m_bProven = false;
}

CString	CBGSet::GetName(UINT nCode)
{
	CString	sName;
	sName.Format(_T("%X"), nCode);
	return sName;
}

bool CBGSet::GetCode(LPCTSTR sSet, UINT &nCode)
{
	if (_stscanf_s(sSet, _T("%X"), &nCode) != 1)
		return false;
	return true;
}

CString	CBGSet::GetName() const
{
	return GetName(m_nCode);
}

void CBGSet::DumpAttributes() const
{
	_tprintf(_T("%s\t%d\t%d\t%d\t%d\t%d\t%d\n"), GetName().GetString(), m_nDigits, m_nRange, m_nStates, m_nBalance, m_nMaxTrans, m_nMaxSpan);
}

void CBGSet::DumpRows() const
{
	for (int iDigit = 0; iDigit < m_nDigits; iDigit++) {
		for (int iState = 0; iState < m_nStates; iState++) {
			printf("%d ", m_arrRow[iDigit][iState]);
		}
		printf("\n");
	}
}

CString	CBGSet::GetRowCSV(int iDigit, int nOffset, TCHAR cSeparator) const
{
	CString	sRow;
	for (int iState = 0; iState < m_nStates; iState++) {
		CString	sDigit;
		sDigit.Format(_T("%d"), m_arrRow[iDigit][iState] + nOffset);
		if (iState)
			sRow += cSeparator;
		sRow += sDigit;
	}
	return sRow;
}

void CBGSet::CSetIDArray::SetCode(UINT nSetCode)
{
	RemoveAll();
	for (int iDigit = 0; iDigit < MAX_SET_DIGITS; iDigit++) {
		int	nDigit = nSetCode & 0xf;	// one digit per nibble
		if (!nDigit)
			break;
		Add(nDigit);
		nSetCode >>= 4;	// next nibble
	}
	MakeReverse();
}

UINT CBGSet::CSetIDArray::GetCode() const
{
	int	nDigits = GetSize();
	UINT	nSetCode = 0;
	for (int iDigit = 0; iDigit < nDigits; iDigit++) {
		int	nDigit = GetAt(iDigit);
		nSetCode <<= 4;	// next nibble
		nSetCode += nDigit;
	}
	return nSetCode;
}

void CBGSet::Canonicalize(const CSetIDArray& arrSetID, CSetIDArray& arrCanonicalSetID, CSetIDArray& arrCanonicalDigitIdx)
{
	arrCanonicalSetID = arrSetID;
	arrCanonicalSetID.Sort();
	CSetIDArray arrSetIDSortedCopy(arrCanonicalSetID);
	int nDigits = arrSetID.GetSize();
	arrCanonicalDigitIdx.SetSize(nDigits);
	for (int iDigit = 0; iDigit < nDigits; iDigit++) {
		int	iPos = arrSetIDSortedCopy.Find(arrSetID[iDigit]);
		ASSERT(iPos >= 0);
		arrCanonicalDigitIdx[iDigit] = iPos;
		arrSetIDSortedCopy[iPos] = 0;	// mark digit used
	}
}

bool CBGSet::ReadSetData(UINT nSetCode, LPCTSTR pszSetFolderPath)
{
	m_arrSetID.SetCode(nSetCode);
	CSetIDArray	arrCanonicalSetID, arrCanonicalDigitIdx;
	Canonicalize(m_arrSetID, arrCanonicalSetID, arrCanonicalDigitIdx);
	int	nDigits = m_arrSetID.GetSize();
	CString	sSetPath(pszSetFolderPath);
	CString	sFileName, sSetName;
	sSetName = CBGSet::GetName(arrCanonicalSetID.GetCode());
	sFileName = _T("BalaGray ") + sSetName + _T(".txt");
	sSetPath += '\\' + sFileName;
	// read balanced gray data file into memory
	CString	sLine;
	CStringArray	arrLine;
	CStdioFile	fData(sSetPath, CFile::modeRead);
	while (fData.ReadString(sLine)) {
		arrLine.Add(sLine);
	}
	// assume last iteration of set has optimal balance and span length
	int	nLines = static_cast<int>(arrLine.GetSize());
	int	iStart = -1;
	// find last iteration of set by searching backwards for header
	for (int iLine = nLines - 1; iLine >= 0; iLine--) {
		if (arrLine[iLine].Left(8) == _T("balance ")) {
			iStart = iLine;
			break;
		}
	}
	if (iStart < 0) {	// if header not found
		printf("missing header\n");
		return false;
	}
	// compute number of states
	int	nRange = 0;
	int	nStates = 1;
	CByteArray	arrDigitRange;
	arrDigitRange.SetSize(nDigits);
	for (int iDigit = 0; iDigit < nDigits; iDigit++) {
		int	nDigitRange = m_arrSetID[iDigit];
		nRange += nDigitRange;
		nStates *= nDigitRange;
		arrDigitRange[iDigit] = nDigitRange;
	}
	// read set header
	int	nBalance, nMaxTrans, nMaxSpan;
	int	nConvs = _stscanf_s(arrLine[iStart], _T("balance = %d, maxtrans = %d, maxspan = %d\n"), &nBalance, &nMaxTrans, &nMaxSpan);
	if (nConvs != 3) {
		printf("invalid header format at line %d\n", iStart + 1);
		return false;
	}
	// read set data rows
	m_arrRow.SetSize(nDigits);
	for (int iDigit = 0; iDigit < nDigits; iDigit++) {	// for each digit
		int	iMappedDigit = arrCanonicalDigitIdx[iDigit];	// digits may not be in canonical order
		int	iLine = iStart + 1 + iMappedDigit;
		CString	sDataLine(arrLine[iLine]);
		int	iStartChar = 0;
		m_arrRow[iDigit].SetSize(nStates);
		int	nDigitRange = arrDigitRange[iDigit];
		for (int iState = 0; iState < nStates; iState++) {	// for each state
			CString	sToken(sDataLine.Tokenize(_T(" "), iStartChar));
			if (sToken.IsEmpty()) {
				printf("invalid data format at line %d\n", iLine + 1);
				return false;
			}
			int	nVal;
			if (_stscanf_s(sToken, _T("%d"), &nVal) != 1) {
				printf("missing state value at line %d\n", iLine + 1);
				return false;
			}
			if (nVal < 0 || nVal >= nDigitRange) {
				printf("state value out of range at line %d\n", iLine + 1);
				return false;
			}
			m_arrRow[iDigit][iState] = nVal;
		}
	}
	m_nCode = nSetCode;
	m_nDigits = nDigits;
	m_nRange = nRange;
	m_nStates = nStates;
	m_nBalance = nBalance;
	m_nMaxTrans = nMaxTrans;
	m_nMaxSpan = nMaxSpan;
	return true;
}
