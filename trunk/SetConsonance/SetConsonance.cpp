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

// SetConsonance.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdint.h"	// standard sizes
#include "string"
#include "vector"
#include "PitchClassSet.h"
#include "BgSet.h"
#include "IntervalSet.h"
extern "C" { 
#include "_generate.h"
};

#define CONSOLE_NATTER 1

using namespace std;

typedef vector<CIntervalSet> CIntervalSetArray;

enum {
	#define FORTEDEF(id, name, sym) FN_##sym,
	#include "ForteDef.h"
};

void GetPermutations(const CIntervalSet& set, CIntervalSetArray& arrSet)
{
	int	nPerms = set.GetPermutationCount();
	arrSet.resize(nPerms);
	for (int iPerm = 0; iPerm < nPerms; iPerm++) {
		CIntervalSet	setPerm(set);
		setPerm.Unpack(iPerm);
		arrSet[iPerm] = setPerm;
	}
}

void GetPermutations(const CIntervalSet& set, const CIntervalSet& spacing, CIntervalSetArray& arrSet)
{
	GetPermutations(set, arrSet);
	int	nPlaces = set.GetSize();
	int	nPerms = static_cast<int>(arrSet.size());
	for (int iPerm = 0; iPerm < nPerms; iPerm++) {
		CIntervalSet&	setPerm = arrSet[iPerm];
		int	nOffset = 0;
		for (int iPlace = 0; iPlace < nPlaces; iPlace++) {
			int	nSpacedPlace = setPerm[iPlace] + nOffset;
			ASSERT(nSpacedPlace < MAX_PITCH_COUNT);
			setPerm[iPlace] = nSpacedPlace;
			nOffset += set.GetRange(iPlace) + spacing[iPlace];
		}
	}
}

void TestPitchClassSet()
{
	int	test[] = {5, 7, 9, 10, 0, 2, 4};
	CPitchClassSet	set(test, _countof(test));
	set.MakePrimeForm();
	printf("%s %s\n", set.FormatSet().c_str(), set.GetForte());
}

void TestIntervalSetPacking()
{
	static const CIntervalSet::SET rngTest = {3, 4, 5};
	CIntervalSet	set;
	set.Alloc(rngTest);
	set[0] = 1;
	set[1] = 2;
	set[2] = 3;
	set.Dump();
	int	nPackedVal = set.Pack();
	CIntervalSet	set2(set);
	set2.Clear();
	set2.Unpack(nPackedVal);
	set2.Dump();
}

void TestIntervalSetPermutation()
{
	static const CIntervalSet::SET rngTest = {2, 3, 4};
	CIntervalSet	set;
	set.Alloc(rngTest);
	int	nPerms = set.GetPermutationCount();
	printf("%d permutations\n", nPerms);
	for (int iPerm = 0; iPerm < nPerms; iPerm++) {
		set.Unpack(iPerm);
		set.Dump();
	}
}

enum {
	C, Db, D, Eb, E, F, Gb, G, Ab, A, Bb, B,
	NOTES
};

enum {
	IONIAN,
	DORIAN,
	PHRYGIAN,
	LYDIAN,
	MIXOLYDIAN,
	AOLIAN,
	LOCRIAN,
	MODES
};

enum {
	MAJOR,
	MELODIC_MINOR,
	HARMONIC_MINOR,
	HARMONIC_MAJOR,
	ASYMMETRIC_DIM,
	DIMINISHED,
	WHOLE_TONE,
	HUNGARIAN_MAJOR,
	SCALES
};

enum {
	OCTAVE = 12,
	NUM_HARMS = 2,
};

enum {	// harmonic functions
	HF_TONIC,
	HF_SUBDOM,
	HF_DOM,
	HARMONIC_FUNCTIONS
};

struct HARMONIZATION {
	int		nKey;		// 0..11
	int		iScale;	
	int		nMode;
	int		arrTone[OCTAVE];
	LPCTSTR	pszName;
};

struct PCS_ALIAS {
	int		iPrime;	// index of prime form
	int		iHarmFunc;	// index of harmonic function
	LPCTSTR	pszAlias;	// chord symbol
	HARMONIZATION	arrHarm[NUM_HARMS];	// second element harmonizes inverted set if it's different
};

struct SCALE {
	int		arrTone[NOTES];
};

struct SCALE_INFO {
	LPCSTR	pszName;
	SCALE	scale;
	int		nLen;
};

const LPCTSTR m_arrNoteName[NOTES] = {
	"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"
};

const LPCTSTR m_arrModeName[MODES] = {
	"Ionian", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Aolian", "Locrian"
};

const SCALE_INFO m_arrScaleInfo[SCALES] = {
	{"Major",			{0, 2, 4, 5, 7, 9, 11},		7},
	{"Melodic Minor",	{0, 2, 3, 5, 7, 9, 11},		7},
	{"Harmonic Minor",	{0, 2, 3, 5, 7, 8, 11},		7},
	{"Harmonic Major",	{0, 2, 4, 5, 7, 8, 11},		7},
	{"Asymmetric Dim",	{0, 3, 5, 6, 8, 9, 11},		7},
	{"Diminished",		{0, 2, 3, 5, 6, 8, 9, 11},	8},
	{"Whole Tone",		{0, 2, 4, 6, 8, 10},		6},
	{"Hungarian Major",	{0, 1, 3, 4, 6, 7, 9},		7},
};

#define USE_HEPTATONIC_ONLY 1

static const PCS_ALIAS m_arrPCSAlias[] = {

// Trichords
{FN_3_2,	HF_SUBDOM,	"-",		{{Ab, MAJOR, DORIAN, {2, 3, 4}}, {G, MAJOR, DORIAN, {1, 2, 3}}}},
{FN_3_3,	HF_DOM,		"7",		{{Db, MELODIC_MINOR, LOCRIAN, {1, 2, 4}}, {A, MELODIC_MINOR, PHRYGIAN, {1, 5, 7}}}},
{FN_3_4,	HF_TONIC,	"",			{{Ab, MAJOR, LYDIAN, {1, 3, 7}}, {G, MAJOR, LYDIAN, {1, 5, 7}}}},
{FN_3_5,	HF_DOM,		"7",		{{Db, MAJOR, MIXOLYDIAN, {3, 4, 7}}, {G, MAJOR, MIXOLYDIAN, {3, 6, 7}, ""}}},
{FN_3_6,	HF_TONIC,	"",			{{G, MAJOR, LYDIAN, {1, 2, 3}}, {-1}}},
{FN_3_7,	HF_SUBDOM,	"-",		{{C, MAJOR, DORIAN, {1, 3, 7}}, {F, MAJOR, DORIAN, {1, 3, 4}}}},
{FN_3_8,	HF_DOM,		"7",		{{G, MAJOR, MIXOLYDIAN, {1, 3, 7}}, {Db, MAJOR, LYDIAN, {1, 3, 4}, ""}}},
{FN_3_9,	HF_SUBDOM,	"sus",		{{C, MAJOR, MIXOLYDIAN, {1, 4, 5}}, {-1}}},
{FN_3_10,	HF_DOM,		"dim",		{{Db, MAJOR, LOCRIAN, {1, 3, 5}}, {-1}}},
{FN_3_11,	HF_TONIC,	"min, maj",	{{Bb, MAJOR, DORIAN, {1, 3, 5}, "-"}, {C, MAJOR, LYDIAN, {1, 3, 5}, ""}}},
{FN_3_12,	HF_DOM,		"aug",		{{A, MELODIC_MINOR, PHRYGIAN, {1, 3, 5}}, {-1}}},

// Tetrachords
{FN_4_3,	HF_DOM,		"-maj7",	{{Db, MELODIC_MINOR, IONIAN, {1, 2, 3, 7}}, {-1}}},
{FN_4_7,	HF_DOM,		"maj7#9",	{{F, HARMONIC_MINOR, AOLIAN, {1, 2, 3, 7}}, {-1}}},
{FN_4_8,	HF_DOM,		"maj7#4",	{{Db, MAJOR, LYDIAN, {1, 4, 5, 7}}, {-1}}},
#if !USE_HEPTATONIC_ONLY
{FN_4_9,	HF_DOM,		"13#9",		{{G, DIMINISHED, DORIAN, {3, 4, 7, 8}}, {-1}}},
#else
{FN_4_9,	HF_DOM,		"13#9",		{{C, HUNGARIAN_MAJOR, IONIAN, {1, 2, 5, 6}}, {-1}}},
#endif
{FN_4_10,	HF_DOM,		"-11",		{{Bb, MAJOR, DORIAN, {1, 2, 3, 4}}, {-1}}},
{FN_4_11,	HF_DOM,		"maj7#4",	{{Db, MAJOR, LYDIAN, {4, 5, 6, 7}}, {G, MAJOR, LYDIAN, {1, 5, 6, 7}, "maj7"}}},
{FN_4_12,	HF_DOM,		"7#4",		{{Eb, MELODIC_MINOR, LYDIAN, {3, 4, 5, 7}}, {G, MELODIC_MINOR, MIXOLYDIAN, {3, 5, 6, 7}, "7b13"}}},
{FN_4_13,	HF_DOM,		"-6",		{{Db, MAJOR, DORIAN, {1, 3, 6, 7}}, {G, MAJOR, MIXOLYDIAN, {3, 5, 6, 7}, "13"}}},
{FN_4_14,	HF_SUBDOM,	"-add2",	{{Bb, MAJOR, DORIAN, {1, 2, 3, 5}}, {F, MAJOR, LYDIAN, {1, 2, 5, 7}, "maj9"}}},
{FN_4_Z15,	HF_DOM,		"7#4",		{{Db, MELODIC_MINOR, LYDIAN, {1, 4, 5, 7}}, {A, MELODIC_MINOR, LOCRIAN, {1, 3, 4, 7}, "7#9"}}},
{FN_4_Z29,	HF_DOM,		"maj7#4",	{{Ab, MAJOR, LYDIAN,	{1, 2, 4, 7}}, {C, MAJOR, MIXOLYDIAN, {2, 3, 4, 7}, "9(4)"}}},
{FN_4_16,	HF_DOM,		"13",		{{Ab, MAJOR, MIXOLYDIAN, {2, 3, 6, 7}}, {C, MAJOR, MIXOLYDIAN, {1, 3, 4, 7}, "7(4)"}}},
#if !USE_HEPTATONIC_ONLY
{FN_4_17,	HF_DOM,		"13b9#4",	{{E, DIMINISHED, DORIAN, {2, 5, 7, 8}}, {-1}}},
{FN_4_18,	HF_DOM,		"13b9",		{{Db, DIMINISHED, DORIAN, {2, 4, 7, 8}}, {C, HARMONIC_MINOR, MIXOLYDIAN, {2, 3, 4, 7}, "7b9(4)"}}},
#else
{FN_4_17,	HF_DOM,		"13b9#4",	{{E, HARMONIC_MINOR, AOLIAN, {1, 2, 3, 5}}, {-1}}},
{FN_4_18,	HF_DOM,		"13b9",		{{E, ASYMMETRIC_DIM, LOCRIAN, {2, 3, 6, 7}}, {C, HARMONIC_MINOR, MIXOLYDIAN, {2, 3, 4, 7}, "7b9(4)"}}},
#endif
{FN_4_19,	HF_DOM,		"-maj7",	{{Db, MELODIC_MINOR, IONIAN, {1, 3, 5, 7}}, {A, MELODIC_MINOR, PHRYGIAN, {1, 3, 5, 7}, "maj7#5"}}},
{FN_4_20,	HF_TONIC,	"maj7",		{{Ab, MAJOR, LYDIAN, {1, 3, 5, 7}}, {-1}}},
{FN_4_21,	HF_DOM,		"maj7#4",	{{G, MAJOR, LYDIAN, {1, 2, 3, 4}}, {-1}}},
{FN_4_22,	HF_SUBDOM,	"add2",		{{G, MAJOR, LYDIAN, {1, 2, 3, 5}}, {Eb, MAJOR, DORIAN, {1, 3, 4, 5}, "-add4"}}},
{FN_4_23,	HF_SUBDOM,	"sus7",		{{C, MAJOR, MIXOLYDIAN, {1, 4, 5, 7}}, {-1}}},
{FN_4_24,	HF_DOM,		"+7",		{{F, MELODIC_MINOR, LOCRIAN, {1, 4, 6, 7}}, {-1}}},
#if !USE_HEPTATONIC_ONLY
{FN_4_25,	HF_DOM,		"+7",		{{C, WHOLE_TONE, IONIAN, {1, 2, 4, 5}}, {-1}}},
#else
{FN_4_25,	HF_DOM,		"+7",		{{Eb, MELODIC_MINOR, LOCRIAN, {1, 4, 5, 7}}, {-1}}},
#endif
{FN_4_26,	HF_TONIC,	"-7",		{{Eb, MAJOR, DORIAN, {1, 3, 5, 7}}, {-1}}},
{FN_4_27,	HF_DOM,		"-7b5",		{{Eb, MAJOR, LOCRIAN, {1, 3, 5, 7}}, {F, MAJOR, MIXOLYDIAN, {1, 3, 5, 7}, "7"}}},
#if !USE_HEPTATONIC_ONLY
{FN_4_28,	HF_DOM,		"o7",		{{C, DIMINISHED, IONIAN, {1, 3, 5, 7}}, {-1}}},
#else
{FN_4_28,	HF_DOM,		"o7",		{{Db, HARMONIC_MINOR, LOCRIAN, {1, 3, 5, 7}}, {-1}}},
#endif

// Pentachords
{FN_5_Z12,	HF_DOM,		"-6",		{{Db, MAJOR, DORIAN, {1, 2, 3, 6, 7}}, {-1}}},
{FN_5_16,	HF_DOM,		"o7",		{{Ab, HARMONIC_MAJOR, DORIAN, {2, 3, 4, 5, 6}}, {A, HARMONIC_MINOR, AOLIAN, {1, 2, 3, 4, 5}, "maj7#9"}}},
{FN_5_Z17,	HF_DOM,		"maj7#5",	{{Db, MELODIC_MINOR, PHRYGIAN, {1, 3, 5, 6, 7}}, {-1}}},
{FN_5_Z18,	HF_DOM,		"7b9",		{{F, HARMONIC_MINOR, MIXOLYDIAN, {1, 2, 3, 4, 5}}, {C, HARMONIC_MINOR, LYDIAN, {1, 2, 3, 4, 5}, "-7#11"}}},
{FN_5_19,	HF_DOM,		"13#9",		{{C, HUNGARIAN_MAJOR, PHRYGIAN, {1, 3, 4, 6, 7}}, {F, HUNGARIAN_MAJOR, DORIAN, {1, 3, 4, 5, 7}, "oM7"}}},
{FN_5_20,	HF_TONIC,	"maj7",		{{Db, MAJOR, LYDIAN, {1, 2, 4, 5, 7}}, {G, MAJOR, LYDIAN, {1, 3, 4, 5, 7}, "maj7"}}},
{FN_5_21,	HF_DOM,		"-maj7",	{{F, HARMONIC_MINOR, IONIAN, {1, 3, 5, 6, 7}}, {C, HARMONIC_MAJOR, IONIAN, {1, 3, 5, 6, 7}, "maj7(b6)"}}},
{FN_5_22,	HF_DOM,		"-maj7",	{{F, HARMONIC_MINOR, IONIAN, {2, 3, 5, 6, 7}}, {-1}}},
{FN_5_23,	HF_SUBDOM,	"-7",		{{Bb, MAJOR, DORIAN, {1, 2, 3, 4, 5}}, {F, MAJOR, LYDIAN, {1, 2, 5, 6, 7}, "maj7"}}},
{FN_5_24,	HF_TONIC,	"maj7",		{{Ab, MAJOR, LYDIAN, {1, 2, 3, 4, 7}}, {C, MAJOR, LYDIAN, {1, 2, 3, 4, 5}, "maj7"}}},
{FN_5_25,	HF_DOM,		"-6",		{{Eb, MAJOR, DORIAN, {1, 3, 5, 6, 7}}, {F, MAJOR, MIXOLYDIAN, {1, 3, 5, 6, 7}, "13"}}},
{FN_5_26,	HF_DOM,		"7b9",		{{F, MELODIC_MINOR, LOCRIAN, {1, 2, 4, 6, 7}}, {F, MELODIC_MINOR, PHRYGIAN, {1, 2, 3, 5, 7}, "maj7#5"}}},
{FN_5_27,	HF_TONIC,	"maj7",		{{Ab, MAJOR, LYDIAN, {1, 2, 3, 5, 7}}, {C, MAJOR, LYDIAN, {2, 3, 4, 5, 7}, "maj7"}}},
{FN_5_28,	HF_DOM,		"7#4",		{{Eb, MELODIC_MINOR, LYDIAN, {1, 3, 4, 5, 7}}, {G, MELODIC_MINOR, LYDIAN, {1, 3, 4, 6, 7}, "7#4"}}},
{FN_5_29,	HF_DOM,		"7",		{{Db, MAJOR, MIXOLYDIAN, {1, 3, 4, 5, 7}}, {G, MAJOR, MIXOLYDIAN, {2, 3, 5, 6, 7}, "13"}}},
{FN_5_30,	HF_DOM,		"7#4",		{{Db, MELODIC_MINOR, LYDIAN, {1, 2, 4, 5, 7}}, {A, MELODIC_MINOR, LYDIAN, {2, 4, 4, 6, 7}, "7#4"}}},
{FN_5_31,	HF_DOM,		"7b9",		{{Bb, HARMONIC_MINOR, MIXOLYDIAN, {2, 3, 5, 6, 7}}, {G, HARMONIC_MAJOR, LOCRIAN, {1, 3, 4, 5, 7}, "o7"}}},
{FN_5_32,	HF_DOM,		"7b9",		{{Db, HARMONIC_MINOR, MIXOLYDIAN, {2, 3, 4, 6, 7}}, {E, HARMONIC_MAJOR, PHRYGIAN, {1, 3, 4, 5, 7}, "#9"}}},
{FN_5_33,	HF_DOM,		"+7",		{{A, MELODIC_MINOR, MIXOLYDIAN, {1, 2, 3, 6, 7}}, {-1}}},
{FN_5_34,	HF_TONIC,	"maj6",		{{G, MAJOR, LYDIAN, {1, 2, 3, 4, 6}}, {-1}}},
{FN_5_35,	HF_TONIC,	"maj6",		{{G, MAJOR, LYDIAN, {1, 3, 5, 6, 2}}, {-1}}},

// Hexachords
{FN_6_Z19,	HF_DOM,		"13b9",		{{Ab, HARMONIC_MAJOR, MIXOLYDIAN, {1, 2, 3, 4, 6, 7}}, {A, HARMONIC_MINOR, IONIAN, {1, 2, 3, 5, 6, 7}, "-maj7b6"}}},
// 6-20 (Ode to Napoleon) can't be harmonized without chromatic cluster or dual minor thirds
{FN_6_Z23,	HF_DOM,		"7#4",		{{Eb, MELODIC_MINOR, LYDIAN, {1, 3, 4, 5, 6, 7}}, {-1}}},
{FN_6_Z24,	HF_SUBDOM,	"-maj7",	{{Db, MELODIC_MINOR, IONIAN, {1, 2, 3, 4, 5, 7}}, {A, MELODIC_MINOR, PHRYGIAN, {1, 3, 4, 5, 6, 7}, "maj7#5"}}},
{FN_6_Z25,	HF_TONIC,	"maj7",		{{Db, MAJOR, LYDIAN, {1, 2, 4, 5, 6, 7}}, {G, MAJOR, LYDIAN, {1, 3, 4, 5, 6, 7}}}},
{FN_6_Z26,	HF_TONIC,	"maj7",		{{Ab, MAJOR, LYDIAN, {1, 2, 3, 4, 5, 7}}, {-1}}},
{FN_6_27,	HF_DOM,		"7b9",		{{C, HUNGARIAN_MAJOR, IONIAN, {1, 2, 3, 4, 5, 7}}, {Eb, ASYMMETRIC_DIM, IONIAN, {1, 2, 3, 4, 5, 6}, "o7"}}},
{FN_6_Z28,	HF_SUBDOM,	"maj7#5",	{{Bb, HARMONIC_MINOR, PHRYGIAN, {1, 2, 3, 4, 5, 7}}, {-1}}},
{FN_6_Z29,	HF_DOM,		"7#9",		{{G, HARMONIC_MAJOR, PHRYGIAN, {2, 3, 4, 5, 6, 7}}, {-1}}},
{FN_6_30,	HF_DOM,		"13#9",		{{Gb, HUNGARIAN_MAJOR, LOCRIAN, {1, 2, 3, 4, 6, 7}}, {C, ASYMMETRIC_DIM, LOCRIAN, {1, 2, 3, 4, 5, 7}, "7b9"}}},
{FN_6_31,	HF_SUBDOM,	"maj7#9",	{{F, HARMONIC_MAJOR, IONIAN, {1, 2, 3, 5, 6, 7}}, {C, HARMONIC_MINOR, IONIAN, {1, 3, 4, 5, 6, 7}, "-maj7b6"}}},
{FN_6_32,	HF_TONIC,	"-7",		{{C, MAJOR, DORIAN, {1, 2, 3, 4, 5, 7}}, {-1}}},
{FN_6_33,	HF_TONIC,	"maj7",		{{Bb, MAJOR, LYDIAN, {1, 2, 3, 4, 6, 7}}, {Bb, MAJOR, LYDIAN, {1, 2, 3, 4, 5, 6}}}},
{FN_6_34,	HF_DOM,		"7#9",		{{Bb, MELODIC_MINOR, LOCRIAN, {1, 3, 4, 5, 6, 7}}, {C, MELODIC_MINOR, LYDIAN, {1, 2, 3, 4, 5, 7}, "7#4"}}},
{FN_6_35,	HF_DOM,		"+7",		{{C, WHOLE_TONE, IONIAN, {1, 2, 3, 4, 5, 6}}, {-1}}},

// Scales
{FN_6_35,	-1,	"whole tone",			{{-1},{-1}}},
{FN_7_20,	-1,	"Persian",				{{-1},{-1}}},
{FN_7_22,	-1,	"double harmonic",		{{-1},{-1}}},
{FN_7_30,	-1,	"Neapolitan minor",		{{-1},{-1}}},
{FN_7_31,	-1,	"Hungarian major",		{{-1},{-1}}},
{FN_7_32,	-1,	"harmonic minor, harmonic major",	{{-1},{-1}}},
{FN_7_33,	-1,	"Neapolitan major",		{{-1},{-1}}},
{FN_7_34,	-1,	"melodic minor",		{{-1},{-1}}},
{FN_7_35,	-1,	"major",				{{-1},{-1}}},
{FN_8_28,	-1,	"octatonic diminished",	{{-1},{-1}}},

};

static const int FindAlias(int iPrime)
{
	for (int iAlias = 0; iAlias < _countof(m_arrPCSAlias); iAlias++) {
		if (m_arrPCSAlias[iAlias].iPrime == iPrime)
			return iAlias;
	}
	return -1;
}

void InvertSet(CPitchClassSet& pcs)
{
	// assume set is in prime form
	int	nSize = pcs.GetSize();
	for (int i = 0; i < nSize; i++) {
		pcs[i] = (NOTES - pcs[i]) % NOTES;
	}
}

bool EquivalentSet(const CPitchClassSet& setRef, const CPitchClassSet& setTest, int nTranspose = 0)
{
	int	nSize = setRef.GetSize();
	if (setTest.GetSize() != nSize)
		return false;
	for (int i = 0; i < nSize; i++) {
		if (setRef.Find((setTest[i] + nTranspose) % NOTES) < 0)
			return false;
	}
	return true;
}

void TestIntervalSetForte()
{
//	static const CIntervalSet::SET rngTest = {3, 4, 3, 2};	// this should match the output of the SetPerm app for [3432]
	static const CIntervalSet::SET rngTest = {3, 2, 3, 2};
	CIntervalSet	setTest;
	setTest.Alloc(rngTest);
	CIntervalSetArray	arrSet, arrSetSpaced;
	GetPermutations(setTest, arrSet);
	CIntervalSet	spacing;
	spacing.SetSize(setTest.GetSize());
	spacing[0] = 0;
	spacing[1] = 1;
	spacing[2] = 1;
	spacing[3] = 0;
	GetPermutations(setTest, spacing, arrSetSpaced);
	int	nPerms = setTest.GetPermutationCount();
	for (int iPerm = 0; iPerm < nPerms; iPerm++) {
		CPitchClassSet	pcs(arrSetSpaced[iPerm].GetData(), setTest.GetSize());
		CPitchClassSet	pcsPrime(pcs);
		pcsPrime.MakePrimeForm();
		int	iPrime = CPitchClassSet::FindForte(pcsPrime.GetId());
		int	iAlias = FindAlias(iPrime);
		LPCTSTR	pszAlias = iAlias >= 0 ? m_arrPCSAlias[iAlias].pszAlias : _T("");
		printf("%d\t%s\t%s\t%s\t%s\t%s\n", iPerm, arrSet[iPerm].FormatSet().c_str(), pcs.FormatSet().c_str(), pcsPrime.FormatSet().c_str(), pcsPrime.GetForte(), pszAlias);
	}
}

int FindHarmony(const CPitchClassSet& pcs, int& iHarm, int& nKeyShift)
{
	iHarm = 0;
	nKeyShift = 0;
	CPitchClassSet	pcsPrime(pcs);
	pcsPrime.MakePrimeForm();
	int	iPrime = CPitchClassSet::FindForte(pcsPrime.GetId());
	int	iAlias = FindAlias(iPrime);
	if (iAlias < 0 || m_arrPCSAlias[iAlias].arrHarm[0].nKey < 0)
		return -1;
	LPCTSTR	pszAlias = m_arrPCSAlias[iAlias].pszAlias;
	CPitchClassSet	pcsTest(pcsPrime);
	for (int iPass = 0; iPass < NUM_HARMS; iPass++) {
		iHarm = iPass;
		for (int iNote = 0; iNote < NOTES; iNote++) {
			if (EquivalentSet(pcs, pcsTest, iNote)) {
				if (iPass) {
					if (m_arrPCSAlias[iAlias].arrHarm[1].nKey < 0)
						return -1;
				}
				nKeyShift = iNote;
				return iAlias;
			}
		}
		InvertSet(pcsTest);
	}
	return -1;
}

bool FindForte(const CIntervalSet& set, int& iAlias, int& iHarm, int& nKey, int& nRoot)
{
	CPitchClassSet	pcs(set.GetData(), set.GetSize());
	int	nKeyShift;
	iAlias = FindHarmony(pcs, iHarm, nKeyShift);
	if (iAlias < 0)
		return false;
	const PCS_ALIAS& alias = m_arrPCSAlias[iAlias];
	const HARMONIZATION& harm = alias.arrHarm[iHarm];
	const int nTranspose = 0;
	nKey = (harm.nKey + nKeyShift) % NOTES;
	nRoot = (m_arrScaleInfo[harm.iScale].scale.arrTone[harm.nMode] + nKey + nTranspose) % NOTES;	
	return true;
}

bool ForteReport(const CIntervalSet& set)
{
	int iAlias, iHarm, nKey, nRoot;
	if (FindForte(set, iAlias, iHarm, nKey, nRoot)) {
		LPCTSTR	pszChordType;
		const PCS_ALIAS& alias = m_arrPCSAlias[iAlias];
		const HARMONIZATION& harm = alias.arrHarm[iHarm];
		if (harm.pszName != NULL)
			pszChordType = harm.pszName;
		else
			pszChordType = alias.pszAlias;
		CString	sChordName;
		sChordName = m_arrNoteName[nRoot];
		sChordName += pszChordType;
		printf("%s\t%s\t%s\t%s\t%s\n", CPitchClassSet::GetForte(alias.iPrime), sChordName.GetString(), m_arrNoteName[nKey],
			m_arrScaleInfo[harm.iScale].pszName, m_arrModeName[harm.nMode]);
	} else {
		printf("%s NOT FOUND\n", set.FormatSet().c_str());
		return false;
	}
	return true;
}

bool ForteReport(const CIntervalSetArray& arrPerm)
{
	int	nPerms = static_cast<int>(arrPerm.size());
	for (int iPerm = 0; iPerm < nPerms; iPerm++) {
		if (!ForteReport(arrPerm[iPerm]))
			return false;
	}
	return true;
}

typedef CBoundArray<BYTE, 96> CUniqueKey;

bool CalcOptimalSetSpacing(const CIntervalSet::SET& rngTest, CIntervalSet& m_setBestSpacing, int iOverride = -1, bool bSkipDups = false)
{
	CIntervalSet	setTest;
	setTest.Alloc(rngTest);
	CIntervalSetArray	arrSet;
	int	nRangeSum = setTest.GetRangeSum();
	int	nSpaceAvail = MAX_PITCH_COUNT - nRangeSum;
	CIntervalSet::SET	rngInit;
	rngInit.dw = 0;
	for (int iPlace = 0; iPlace < setTest.GetSize(); iPlace++) {
		rngInit.b[iPlace] = nSpaceAvail + 1;
	}
	CIntervalSet	range;
	range.Alloc(rngInit);
	CIntervalSetArray	arrSpacingPerm;
	GetPermutations(range, arrSpacingPerm);
	int	nSpacePerms = static_cast<int>(arrSpacingPerm.size());
	int	nMostTonics = 0;
	int	nMostSubdoms = 0;
	CIntervalSetArray	arrMostConsonant;
	CIntervalSet	setBestSpacing;
	int	nValidSpacePerms = 0;
	int	nKnownBest = 5;
	vector<CUniqueKey>	arrUniqueKey;
	if (CONSOLE_NATTER) {
		printf("iOverride = %d, bSkipDups = %d\n", iOverride, bSkipDups);
		printf("index\tscore\tspacing\n");
	}
	bool	bOverrideFound = false;
	for (int iSpacePerm = 0; iSpacePerm < nSpacePerms; iSpacePerm++) {
		CIntervalSet&	spacing = arrSpacingPerm[iSpacePerm];
		int	nSpacingSum = spacing.GetSum();
		if (nRangeSum + nSpacingSum <= MAX_PITCH_COUNT) {
			CIntervalSetArray	arrPerm;
			GetPermutations(setTest, spacing, arrPerm);
			int	nPerms = static_cast<int>(arrPerm.size());
			int	nTonics = 0;
			int	nSubdoms = 0;
			CUniqueKey	arrPrimeIdx;
			for (int iPerm = 0; iPerm < nPerms; iPerm++) {
				CPitchClassSet	pcs(arrPerm[iPerm].GetData(), setTest.GetSize());
				CPitchClassSet	pcsPrime(pcs);
				pcsPrime.MakePrimeForm();
				int	iPrime = CPitchClassSet::FindForte(pcsPrime.GetId());
				ASSERT(iPrime >= 0 && iPrime <= BYTE_MAX);
				arrPrimeIdx.Add(iPrime);
				int	iAlias = FindAlias(iPrime);
				ASSERT(iAlias >= 0);
				switch (m_arrPCSAlias[iAlias].iHarmFunc) {
				case HF_TONIC:
					nTonics++;
					break;
				case HF_SUBDOM:
					nSubdoms++;
					break;
				}
			}
			// two permutations collide if they generate the same key (sorted list of prime form indices)
			if (bSkipDups) {	// if skipping duplicates
				arrPrimeIdx.Sort();	// sort prime indices to produce stable key
				vector<CUniqueKey>::iterator it;
				it = find(arrUniqueKey.begin(), arrUniqueKey.end(), arrPrimeIdx);
				if (it != arrUniqueKey.end()) {	// if key was previously encountered
					continue;	// collision; skip duplicate
				}
				arrUniqueKey.push_back(arrPrimeIdx);
			}
//			spacing.Dump();
			if (CONSOLE_NATTER) {
				printf("%d\t%d, %d\t", nValidSpacePerms, nTonics, nSubdoms);
				spacing.Dump();
			}
//			arrPerm[0].Dump();
/*			if (nConsonant == nKnownBest) {
				ForteReport(arrPerm);
			}*/
			if (nValidSpacePerms == iOverride)
				bOverrideFound = true;
			if (nTonics > nMostTonics || (nTonics == nMostTonics && nSubdoms > nMostSubdoms) || bOverrideFound) {
				nMostTonics = nTonics;
				nMostSubdoms = nSubdoms;
				arrMostConsonant = arrPerm;
				setBestSpacing = spacing;
			}
			nValidSpacePerms++;
			if (bOverrideFound)
				break;
		}
	}
	if (bOverrideFound) {
		if (CONSOLE_NATTER) {
			printf("override: using permutation %d\n", iOverride);
		}
	} else {
		if (iOverride >= nValidSpacePerms) {
			printf("override index is out of range\n");
			return false;
		}
		if (CONSOLE_NATTER) {
			printf("permutations = %d\n", nValidSpacePerms);
		}
	}
	if (setBestSpacing.GetSize()) {
		int	nPerms = static_cast<int>(arrMostConsonant.size());
		if (CONSOLE_NATTER) {
			printf("best:\t%d, %d (%.0f%%, %.0f%%)\t", nMostTonics, nMostSubdoms,
				double(nMostTonics) / nPerms * 100, double(nMostSubdoms) / nPerms * 100);
			setBestSpacing.Dump();
//			ForteReport(arrMostConsonant);
		}
/*		for (int iPerm = 0; iPerm < nPerms; iPerm++) {
			arrMostConsonant[iPerm].Dump();
		}*/
		m_setBestSpacing = setBestSpacing;
		return true;
	}
	return false;
}

bool TestHarmonizations()
{
	int	nErrors = 0;
	for (int iAlias = 0; iAlias < _countof(m_arrPCSAlias); iAlias++) {
		const PCS_ALIAS	pcs = m_arrPCSAlias[iAlias];
		CPitchClassSet	setRef(CPitchClassSet::GetPrimeId(pcs.iPrime));
		for (int iHarm = 0; iHarm < NUM_HARMS; iHarm++) {
			const HARMONIZATION& harm = pcs.arrHarm[iHarm];
			if (harm.nKey >= 0) {
				CBoundArray<int, OCTAVE> arrChordTone;
				int	nChordTones = setRef.GetSize();
				arrChordTone.SetSize(nChordTones);
				const SCALE_INFO&	info = m_arrScaleInfo[harm.iScale];
				int	nScaleLen = info.nLen;
				for (int iTone = 0; iTone < nChordTones; iTone++) {
					int	iScaleTone = (harm.arrTone[iTone] - 1 + harm.nMode) % nScaleLen;
					arrChordTone[iTone] = (info.scale.arrTone[iScaleTone] + harm.nKey) % NOTES;
				}
				arrChordTone.Sort();
				CPitchClassSet	setTest(arrChordTone.GetData(), nChordTones);
				bool	bFound;
				if (!iHarm) {
					bFound = (setTest == setRef);
				} else {
					InvertSet(setRef);
					bFound = EquivalentSet(setRef, setTest);
				}
				if (!bFound) {
					printf("NG %s %s\n", CPitchClassSet::GetForte(pcs.iPrime), setRef.FormatSet().c_str());
					for (int iTone = 0; iTone < nChordTones; iTone++) {
						printf("%d ", arrChordTone[iTone]);
					}
					printf("\n");
					nErrors++;
				}
			}
		}
	}
	return !nErrors;
}

bool StringToPitchClassSet(CString sSet, CBoundArray<int, CPitchClassSet::OCTAVE>& arrDigit)
{
	arrDigit.SetSize(0);
	CString	sDigit;
	int	iStart = 0;
	while (!(sDigit = sSet.Tokenize(",", iStart)).IsEmpty()) {	// for each comma-separated token
		int	nDigit;
		if (sscanf_s(sDigit, "%x", &nDigit) != 1 || nDigit < 0 || nDigit > 0xb) {	// if token isn't a valid digit
			return false;
		}
		arrDigit.Add(nDigit);	// add digit to array
	}
	return true;
}

bool ValidateSetClasses(LPCTSTR pszCSVInPath)
{
	// input file is Wikipedia's List_of_set_classes page converted to CSV format via convertcsv.com
	// NOTE: Wikipedia and CPitchClassSet both use Rahn's packing, hence a perfect match is expected
	CStdioFile	fIn(pszCSVInPath, CFile::modeRead);
	CString	sLine;
	int	nPrimes = 0;
	int	nInversions = 0;
	while (fIn.ReadString(sLine)) {
		int	iStart = 0;
		CString	sName = sLine.Tokenize(",", iStart);
		if (!sName.IsEmpty()) {	// if valid name token
			int	nGroup, nSeq;
			bool	bIsZForm = false;
			int	nConvs = sscanf_s(sName, "%d-%d", &nGroup, &nSeq);
			if (nConvs != 2) {	// if scan failed
				bIsZForm = true;
				nConvs = sscanf_s(sName, "%d-Z%d", &nGroup, &nSeq);	// try again with Z before sequence number
			}
			if (nConvs == 2) {	// if group and sequence number were scanned
				CString	sOrigName(sName);
				TCHAR	c = sName[sName.GetLength() - 1];
				bool	bIsInversion = (c == 'B');	// B indicates inversion of prime form
				if (c == 'A' || c == 'B')
					sName.Delete(sName.GetLength() - 1);	// remove A/B designator from name
				if (nGroup >= 1 && nGroup <= 12 && nSeq >= 1 && nSeq <= 50) {	// if group and sequence numbers in range
					iStart++;
					CString	sSet = sLine.Tokenize("[]\"", iStart);
					if (!sSet.IsEmpty()) {	// if valid set token
						if (bIsInversion)	// if inversion
							nInversions++;
						else	// prime form
							nPrimes++;
						sSet.Replace('T', 'A');	// T for Ten; replace with hex digit
						sSet.Replace('E', 'B');	// E for Eleven; replace with hex digit
						iStart = 0;
						CString	sDigit;
						CBoundArray<int, CPitchClassSet::OCTAVE>	arrDigit;
						StringToPitchClassSet(sSet, arrDigit);
						CPitchClassSet	setWP(arrDigit.GetData(), arrDigit.GetSize());
						int	iPrime = CPitchClassSet::FindForte(sName);
						if (iPrime < 0) {	// if Forte lookup fails
							printf("unknown Forte set '%s'\n", sName.GetString());
							return false;
						}
						CPitchClassSet	setCK(CPitchClassSet::GetPrimeId(iPrime));
						if (bIsInversion) {	// if inversion
							CPitchClassSet	setTemp(setCK);
							CPitchClassSet::SET_ID	idInverse;
							CPitchClassSet::SET_ID	id = setTemp.FindPrimeForm(&idInverse);
							setCK = CPitchClassSet(idInverse);	// replace prime form with its inversion
						}
						if (setCK != setWP) {	// if our set doesn't match Wikipedia reference
							printf("mismatch in set %s\n", sName.GetString());
							setWP.Dump();
							setCK.Dump();
							return false;
						}
					}
				}
			}
		}
	}
	if (nPrimes != CPitchClassSet::PRIME_FORMS) {
		printf("wrong number of prime forms\n");
		return false;
	}
	printf("nPrimes = %d, nInversions = %d\n", nPrimes, nInversions);
	return true;
}

enum {
	MAX_CHORD_SIZE = 8
};

struct CHORD_INFO {
	int		arrNote[MAX_CHORD_SIZE];	// chord in normalized notes
	int		nKey;			// key of scale
	int		nType;			// type of scale
	int		nMode;			// which mode of scale
};

class CChord {
public:
	CHORD_INFO	m_SongChord;
	CHORD_INFO	m_ChordTone;
	SCALE_INFO	m_ScaleTone;
	SCALE_INFO	m_CommonTone;
	SCALE_INFO	m_UnusedTone;
	int		m_ApprUp;
	int		m_ApprDown;
	int		m_iAlias;
	int		m_iHarm;
	CString	GetName() const;
	int		GetMode() const;
	int		GetScale() const;
};

CString CChord::GetName() const
{
	const PCS_ALIAS& alias = m_arrPCSAlias[m_iAlias];
	const HARMONIZATION& harm = alias.arrHarm[m_iHarm];
	LPCTSTR	pszChordType;
	if (harm.pszName != NULL)
		pszChordType = harm.pszName;
	else
		pszChordType = alias.pszAlias;
	CString	sChordName;
	sChordName = m_arrNoteName[m_ScaleTone.scale.arrTone[harm.nMode]];
	sChordName += pszChordType;
	return sChordName;
}

int CChord::GetMode() const
{
	const PCS_ALIAS& alias = m_arrPCSAlias[m_iAlias];
	const HARMONIZATION& harm = alias.arrHarm[m_iHarm];
	return harm.nMode;
}

int CChord::GetScale() const
{
	const PCS_ALIAS& alias = m_arrPCSAlias[m_iAlias];
	const HARMONIZATION& harm = alias.arrHarm[m_iHarm];
	return harm.iScale;
}

CArray<CChord, CChord&>	m_arrChord;
int	m_nChordSize;

static const int m_arrMajorScale[] = { C, D, E, F, G, A, B };

CBGSet	m_setBG;
CIntervalSet	m_setBestSpacing;
CIntervalSet::SET	m_setSpan;

void Init(int nSongLen, int nChordSize)
{
	m_arrChord.SetSize(nSongLen);
	m_nChordSize = nChordSize;
}

int	Find(int nTarget, const int* parr, int nSize)
{
	for (int iElem = 0; iElem < nSize; iElem++) {
		if (parr[iElem] == nTarget)
			return iElem;
	}
	return -1;
}

int Wrap(int nVal, int nMod)
{
	int	nResult = nVal % nMod;
	if (nVal < 0)
		nResult += nMod;
	return nResult;
}

void MakeApproaches(const int* arrTarget)
{
	int	nChords = static_cast<int>(m_arrChord.GetSize());
	for (int iChord = 0; iChord < nChords; iChord++) {
		CChord&	chord = m_arrChord[iChord];
		const SCALE_INFO& scale = m_arrScaleInfo[chord.m_SongChord.nType];
		const int* arrTone = chord.m_ScaleTone.scale.arrTone;
		int	iNext = iChord + 1;
		if (iNext >= nChords)
			iNext = 0;
		const CChord& chordNext = m_arrChord[iNext];
		const int* arrToneNext = chordNext.m_ScaleTone.scale.arrTone;
		int	nTarget = arrToneNext[arrTarget[iNext]];
		int	iUp = Find(nTarget, arrTone, scale.nLen);
		int	iDown = iUp;
		if (iUp < 0)
			iUp = Find(Wrap(nTarget + 1, OCTAVE), arrTone, scale.nLen);
		if (iDown < 0)
			iDown = Find(Wrap(nTarget - 1, OCTAVE), arrTone, scale.nLen);
		chord.m_ApprUp = iUp;
		chord.m_ApprDown = iDown;
	}
}

void MakeScalesAndChords()
{
	// compute scale tones and chord tones
	int	iChord;
	int	nChords = static_cast<int>(m_arrChord.GetSize());
	for (iChord = 0; iChord < nChords; iChord++) {
		int	iTone;
		CChord&	chord = m_arrChord[iChord];
		const SCALE_INFO& scale = m_arrScaleInfo[chord.m_SongChord.nType];
		int* arrTone = chord.m_ScaleTone.scale.arrTone;
		chord.m_ScaleTone.nLen = scale.nLen;
		arrTone[7] = B;	// for octatonic
		for (iTone = 0; iTone < 7; iTone++) {
			arrTone[iTone] = scale.scale.arrTone[iTone];
		}
		for (iTone = 0; iTone < scale.nLen; iTone++) {
			arrTone[iTone] = (arrTone[iTone] + chord.m_SongChord.nKey) % OCTAVE;
		}
//		qsort(arrTone, scale.nLen, sizeof(int), SortCompare);
/*		printf("%d: ", iChord);
		for (iTone = 0; iTone < scale.nLen; iTone++) {
			printf("%s ", m_arrNoteName[arrTone[iTone]]);
		}
		printf("\n");*/
		for (iTone = 0; iTone < m_nChordSize; iTone++) {
			int	iScaleTone = Find(chord.m_SongChord.arrNote[iTone], arrTone, scale.nLen);
			ASSERT(iScaleTone >= 0);
			if (iScaleTone >= 0)
				chord.m_ChordTone.arrNote[iTone] = iScaleTone;
		}
	}
	// compute scale tones unused by each chord
	for (iChord = 0; iChord < nChords; iChord++) {
		CChord&	chord = m_arrChord[iChord];
		const SCALE_INFO& scale = m_arrScaleInfo[chord.m_SongChord.nType];
		const int* arrTone = chord.m_ScaleTone.scale.arrTone;
		int	iTone;
		for (iTone = 0; iTone < scale.nLen; iTone++) {
			int	iPos = Find(arrTone[iTone], chord.m_SongChord.arrNote, m_nChordSize);
			if (iPos < 0) {
				chord.m_UnusedTone.scale.arrTone[chord.m_UnusedTone.nLen++] = iTone;
			}
		}
	}
	// compute common tones between adjacent chord scales
#if CONSOLE_NATTER
	int	nTotalCommon = 0;
	for (iChord = 0; iChord < nChords; iChord++) {
		CChord&	chord = m_arrChord[iChord];
		const SCALE_INFO& scale = m_arrScaleInfo[chord.m_SongChord.nType];
		const int* arrTone = chord.m_ScaleTone.scale.arrTone;
		int	iNext = iChord + 1;
		if (iNext >= nChords)
			iNext = 0;
		CBoundArray<int, OCTAVE>	arrCommonTone;
		const CChord& chordNext = m_arrChord[iNext];
		const int* arrToneNext = chordNext.m_ScaleTone.scale.arrTone;
		int	nNextScaleLen = chordNext.m_ScaleTone.nLen;
		for (int iTone = 0; iTone < scale.nLen; iTone++) {
			int	iPos = Find(arrTone[iTone], arrToneNext, nNextScaleLen);
			if (iPos >= 0) {
				arrCommonTone.Add(arrTone[iTone]);
			}
		}
		arrCommonTone.Sort();
		printf("%d [", iChord + 1);
		int	nCTs = arrCommonTone.GetSize();
		for (int iCT = 0; iCT < nCTs; iCT++) {
			printf("%s ", m_arrNoteName[arrCommonTone[iCT]]);
			chord.m_CommonTone.scale.arrTone[iCT] = arrCommonTone[iCT];
		}
		printf("]\n");
		chord.m_CommonTone.nLen = nCTs;
		nTotalCommon += nCTs;
	}
	printf("CTs = %d\n", nTotalCommon);
#endif
/*	for (iChord = 0; iChord < nChords; iChord++) {
		CChord&	chord = m_arrChord[iChord];
		int	nLen = chord.m_CommonTone.nLen;
		printf("%d ", iChord);
		for (int iTarg = 0; iTarg < nLen; iTarg++) {
			int	nTarg = chord.m_CommonTone.scale.arrTone[iTarg];
			int	nHits = 0;
			for (int iChord2 = 0; iChord2 < nChords - 1; iChord2++) {
				int	iNext = iChord + iChord2 + 1;
				if (iNext >= nChords)
					iNext -= nChords;
				const CChord& chordNext = m_arrChord[iNext];
				int	iPos = Find(nTarg, chord.m_CommonTone.scale.arrTone, chordNext.m_CommonTone.nLen);
				if (iPos < 0)
					break;
				nHits++;
			}
			printf("%s %d, ", m_arrNoteName[nTarg], nHits);
		}
		printf("\n");
	}*/
}

void MakeTracks()
{
	const int	nOffset = 64;
	const int	nVelocity = 100;
	const int	nQuant = 120;
	const int	nMaxScaleTones = 8;
	const bool	bOutputCommonTones = 1;
	const bool	bOutputNoteChangeFlags = 0;
	const bool	bMergeDuplicates = 0;
	LPCTSTR	pszOutPath = _T("chords.csv");
	CStdioFile	f(pszOutPath, CFile::modeWrite | CFile::modeCreate);
	f.WriteString("Name,Type,Channel,Note,Quant,RangeType,RangeStart,Length,Steps,Mods\n");	// output track header
	CString	s, t;
	const int	nChords = static_cast<int>(m_arrChord.GetSize());
	int	nTracks = 0;
	// output list of per-chord alterations for each scale tone
	{
		for (int iTone = 0; iTone < nMaxScaleTones - 1; iTone++) {
			s.Format("alt %d,7,9,64,%d,0,64,%d,\"", iTone + 1, nQuant, nChords);
			for (int iChord = 0; iChord < nChords; iChord++) {
				const CChord&	chord = m_arrChord[iChord];
				if (iChord)
					s += ',';
				const SCALE_INFO& scale = m_arrScaleInfo[chord.m_SongChord.nType];
				t.Format("%d", scale.scale.arrTone[iTone] - m_arrMajorScale[iTone] + nOffset);
				s += t;
			}
			f.WriteString(s + "\"\n");
			nTracks++;
		}
	}
	// output seventh scale tone muting for hexatonic scale
	int	iHexatonicMuteTrack = 0;
	{
		s.Format("hexatonic mute,7,9,64,%d,0,64,%d,\"", nQuant, nChords);
		for (int iChord = 0; iChord < nChords; iChord++) {
			const CChord&	chord = m_arrChord[iChord];
			if (iChord)
				s += ',';
			t.Format("%d", chord.m_SongChord.nType == WHOLE_TONE ? 100 : 0);
			s += t;
		}
		f.WriteString(s + "\"\n");
		iHexatonicMuteTrack = nTracks;
		nTracks++;
	}
	// output eight scale tone muting for octatonic scale
	int	iOctatonicMuteTrack = 0;
	{
		s.Format("octatonic mute,7,9,64,%d,0,64,%d,\"", nQuant, nChords);
		for (int iChord = 0; iChord < nChords; iChord++) {
			const CChord&	chord = m_arrChord[iChord];
			if (iChord)
				s += ',';
			t.Format("%d", chord.m_SongChord.nType == DIMINISHED ? 0 : 100);
			s += t;
		}
		f.WriteString(s + "\"\n");
		iOctatonicMuteTrack = nTracks;
		nTracks++;
	}
	// output keys for each chord
	int	iKeysTrack = nTracks;
	{
		s.Format("keys,7,9,64,%d,0,64,%d,\"", nQuant, nChords);
		for (int iChord = 0; iChord < nChords; iChord++) {
			const CChord&	chord = m_arrChord[iChord];
			if (iChord)
				s += ',';
			t.Format("%d", chord.m_SongChord.nKey + nOffset);
			s += t;
		}
		f.WriteString(s + "\"\n");
		iKeysTrack = nTracks;
		nTracks++;
	}
	// output modes for each chord
	int	iModesTrack = nTracks;
	{
		s.Format("modes,7,9,64,%d,0,64,%d,\"", nQuant, nChords);
		for (int iChord = 0; iChord < nChords; iChord++) {
			const CChord&	chord = m_arrChord[iChord];
			if (iChord)
				s += ',';
			t.Format("%d", chord.m_SongChord.nMode + nOffset);
			s += t;
		}
		f.WriteString(s + "\"\n");
		nTracks++;
	}
	// output major scale tones, one per track
	int	iFirstScaleTrack = nTracks;
	{
		for (int iTone = 0; iTone < nMaxScaleTones; iTone++) {
			int	nNote;
			if (iTone < 7) {
				t.Format("1:%d", iTone);	// alteration note modulation
				if (iTone == 6)
					t += ",0:7";	// hexatonic mute modulation
				nNote = m_arrMajorScale[iTone];
			}
			else {
				nNote = 11;
				t = "0:8";	// octatonic mute modulation
			}
			t += ",1:9";	// key note modulation
			s.Format(_T("scale %d,7,9,64,%d,0,64,1,%d,\"%s\"\n"), iTone + 1, nQuant, nNote + nOffset, t.GetString());
			f.WriteString(s);
			nTracks++;
		}
	}
	// output common tone scales
	if (bOutputCommonTones) {
		int	nPrevTracks = nTracks;
		for (int iTone = 0; iTone < nMaxScaleTones; iTone++) {
			s.Format("common %d,7,9,64,%d,0,64,%d,\"", iTone + 1, nQuant, nChords);
			for (int iChord = 0; iChord < nChords; iChord++) {
				const CChord&	chord = m_arrChord[iChord];
				if (iChord)
					s += ',';
				t.Format("%d", chord.m_CommonTone.scale.arrTone[iTone] + nOffset);
				s += t;
			}
			t.Format("\",0:%d", nPrevTracks + nMaxScaleTones + iTone);
			s += t;
			f.WriteString(s + '\n');
			nTracks++;
		}
		// output common tone scale length mutes
		for (int iTone = 0; iTone < nMaxScaleTones; iTone++) {
			s.Format("common mute %d,7,9,64,%d,0,64,%d,\"", iTone + 1, nQuant, nChords);
			for (int iChord = 0; iChord < nChords; iChord++) {
				const CChord&	chord = m_arrChord[iChord];
				if (iChord)
					s += ',';
				t.Format("%d", iTone < chord.m_CommonTone.nLen ? 0 : 100);
				s += t;
			}
			f.WriteString(s + "\"\n");
			nTracks++;
		}
		// output common tone scale length deltas
		s.Format("common delta,7,9,64,%d,0,64,%d,\"", nQuant, nChords);
		for (int iChord = 0; iChord < nChords; iChord++) {
			const CChord&	chord = m_arrChord[iChord];
			if (iChord)
				s += ',';
			int	iNext = (iChord + 1) % nChords;
			const CChord& chordNext = m_arrChord[iNext];
			t.Format("%d", chordNext.m_CommonTone.nLen - chord.m_CommonTone.nLen + nOffset);
			s += t;
		}
		f.WriteString(s + "\"\n");
		nTracks++;
	}
	// output minor offset for numbers system
	{
		s.Format("is minor,7,9,64,%d,0,64,%d,\"", nQuant, nChords);
		for (int iChord = 0; iChord < nChords; iChord++) {
			const CChord&	chord = m_arrChord[iChord];
			const SCALE_INFO& scale = m_arrScaleInfo[chord.m_SongChord.nType];
			const int* arrTone = chord.m_ScaleTone.scale.arrTone;
			int	nRoot = arrTone[chord.m_SongChord.nMode];
			int	nThird = arrTone[(chord.m_SongChord.nMode + 2) % scale.nLen];
			int	nThirdSize = abs(nThird - nRoot);
			nThirdSize = min(nThirdSize, OCTAVE - nThirdSize);
			if (iChord)
				s += ',';
			t.Format("%d", nThirdSize == 3 ? 100 : 0);
			s += t;
		}
		f.WriteString(s + "\"\n");
		nTracks++;
	}
	// output note change flags
	if (bOutputNoteChangeFlags) {
		for (int iTone = 0; iTone < m_nChordSize; iTone++) {
			s.Format("note change %d,7,9,64,%d,0,64,%d,\"", iTone + 1, nQuant, nChords);
			for (int iChord = 0; iChord < nChords; iChord++) {
				const CChord&	chord = m_arrChord[iChord];
				int	iPrevChord = iChord - 1;
				if (iPrevChord < 0)
					iPrevChord = nChords - 1;
				const CChord&	chordPrev = m_arrChord[iPrevChord];
				bool	bIsChange = chord.m_SongChord.arrNote[iTone] != chordPrev.m_SongChord.arrNote[iTone];
				if (iChord)
					s += ',';
				t.Format("%d", bIsChange ? 100 : 0);
				s += t;
			}
			f.WriteString(s + "\"\n");
			nTracks++;
		}
	}
	// output scale tone indices for each tone of each tetrachord
	int	iFirstChordTrack = nTracks;
	{
		for (int iTone = 0; iTone < m_nChordSize; iTone++) {
			s.Format("chord %d,7,9,64,%d,0,64,%d,\"", iTone + 1, nQuant, nChords);
			for (int iChord = 0; iChord < nChords; iChord++) {
				const CChord&	chord = m_arrChord[iChord];
				if (iChord)
					s += ',';
				t.Format("%d", chord.m_ChordTone.arrNote[iTone] + nOffset);
				s += t;
			}
			f.WriteString(s + "\"\n");
			nTracks++;
		}
	}
	// output scale tones not appearing in chord
	int	iFirstTensionTrack = nTracks;
	{
		for (int iTone = 0; iTone < 4; iTone++) {
			s.Format("tension chord %d,7,9,64,%d,0,64,%d,\"", iTone + 1, nQuant, nChords);
			for (int iChord = 0; iChord < nChords; iChord++) {
				const CChord&	chord = m_arrChord[iChord];
				if (iChord)
					s += ',';
				t.Format("%d", chord.m_UnusedTone.scale.arrTone[iTone] + nOffset);
				s += t;
			}
			s += '"';
			if (iTone == 2) {
				t.Format(",\"0:%d\"", iHexatonicMuteTrack);
				s += t;
			} else if (iTone == 3) {
				t.Format(",\"0:%d\"", iOctatonicMuteTrack);
				s += t;
			}
			f.WriteString(s + "\n");
			nTracks++;
		}
	}
	// output scale approach offsets in tones
	{
		CArray<int, int>	arrTarget;
		arrTarget.SetSize(nChords);
		int	iChord;
		for (iChord = 0; iChord < nChords; iChord++) {
			const CChord&	chord = m_arrChord[iChord];
			arrTarget[iChord] = chord.m_SongChord.nMode;
		}
		MakeApproaches(arrTarget.GetData());
		s.Format("approach root up,7,9,64,%d,0,64,%d,\"", nQuant, nChords);
		for (iChord = 0; iChord < nChords; iChord++) {
			const CChord&	chord = m_arrChord[iChord];
			if (iChord)
				s += ',';
			t.Format("%d", chord.m_ApprUp + nOffset);
			s += t;
		}
		f.WriteString(s + "\"\n");
		nTracks++;
		s.Format("approach root down,7,9,64,%d,0,64,%d,\"", nQuant, nChords);
		for (iChord = 0; iChord < nChords; iChord++) {
			const CChord&	chord = m_arrChord[iChord];
			if (iChord)
				s += ',';
			t.Format("%d", chord.m_ApprDown + nOffset);
			s += t;
		}
		f.WriteString(s + "\"\n");
		nTracks++;
	}
/*	// output approach to unused tone
	{
		int	iChord;
		MakeApproaches(m_arrUnused.GetData());
		s.Format("approach unused,7,9,64,%d,0,64,%d,\"", nQuant, nChords);
		for (iChord = 0; iChord < nChords; iChord++) {
			if (iChord)
				s += ',';
			t.Format("%d", m_arrApprUp[iChord] + nOffset);
			s += t;
		}
		f.WriteString(s + "\"\n");
		nTracks++;
	}*/
	// output plain indices
	int	iFirstIndexTrack = nTracks;
	{
		for (int iTone = 0; iTone < m_nChordSize; iTone++) {
			s.Format("index %d,7,9,64,%d,0,64,1,\"%d\"\n", iTone + 1, nQuant, iTone + nOffset);
			f.WriteString(s);
			nTracks++;
		}
	}
	// output stab notes for above scales and chords
	{
		for (int iTone = 0; iTone < m_nChordSize; iTone++) {
			s.Format("stab %d,0,0,60,%d,1,60,", iTone + 1, nQuant);
			if (bMergeDuplicates) {
				t.Format(_T("%d,\""), nChords);
				s += t;
				for (int iChord = 0; iChord < nChords; iChord++) {
					if (iChord)
						s += ',';
					const CChord&	chord = m_arrChord[iChord];
					int	iNextChord = iChord + 1;
					if (iNextChord >= nChords)
						iNextChord = 0;
					const CChord&	chordNext = m_arrChord[iNextChord];
					bool	bIsTied = chord.m_SongChord.arrNote[iTone] == chordNext.m_SongChord.arrNote[iTone];
					t.Format(_T("%d"), bIsTied ? (nVelocity | 0x80) : nVelocity);
					s += t;
				}
				s += "\",\"";
			} else {
				s += _T("1,\"100\",\"");
			}
			int	nScaleTones = 8;
			for (int iScaleTone = 0; iScaleTone < nScaleTones; iScaleTone++) {
				if (iScaleTone)
					s += ',';
				t.Format("7:%d", iScaleTone + iFirstScaleTrack);	// scale modulator
				s += t;
			}
			for (int iChordTone = 0; iChordTone < m_nChordSize; iChordTone++) {
				t.Format(",8:%d", iChordTone + iFirstChordTrack);	// chord modulator
				s += t;
			}
			t.Format(",9:%d", iTone + iFirstIndexTrack);	// index modulator
			s += t;
			f.WriteString(s + "\"\n");
			nTracks++;
		}
	}
}

void MakeTracksSimple(int nSet)
{
	const int	nOffset = 64;
	const int	nVelocity = 100;
	const int	nQuant = 240;
	const int	nRoot = 60;
	LPCTSTR	pszOutPath = _T("chords.csv");
	CStdioFile	f(pszOutPath, CFile::modeWrite | CFile::modeCreate);
	f.WriteString("Name,Type,Channel,Note,Quant,Duration,RangeType,RangeStart,Length,Steps,Mods\n");	// output track header
	CString	s, t;
	{
		int	nOffset = 0;
		for (int iPlace = 0; iPlace < m_setBG.m_nDigits; iPlace++) {
			int	nChords = static_cast<int>(m_arrChord.GetSize());
			s.Format("BG {%x} %d,7,0,60,%d,1,1,60,%d,\"", nSet, iPlace + 1, nQuant, nChords);
			for (int iChord = 0; iChord < nChords; iChord++) {
				if (iChord)
					s += ',';
				int	iNote = m_arrChord[iChord].m_SongChord.arrNote[iPlace] - nOffset;
				t.Format("%d", iNote + 64);
				s += t;
			}
			f.WriteString(s + "\"\n");
			nOffset += m_setSpan.b[iPlace] + m_setBestSpacing[iPlace];
		}
	}
	// output stab notes for above scales and chords
	{
		int	nOffset = 0;
		for (int iPlace = 0; iPlace < m_setBG.m_nDigits; iPlace++) {
			s.Format("stab %d,0,0,%d,%d,1,1,60,", iPlace + 1, nRoot + nOffset, nQuant);
			s += _T("1,\"100\",\"1:");
			t.Format(_T("%d"), iPlace);
			f.WriteString(s + t + "\"\n");
			nOffset += m_setSpan.b[iPlace] + m_setBestSpacing[iPlace];
		}
	}
}

BYTE m_arrToneMap[6][12];
bool m_bMapTones;

bool ProcessIntervalSet(UINT nSetCode)
{
#if 0
//	LPCTSTR	pszSetFolderPath = _T("C:\\Chris\\MyProjects\\BalaGray\\BalaGray");
//	LPCTSTR	pszSetFolderPath = _T("C:\\Chris\\MyProjects\\BalaGrayIterSD");
//	LPCTSTR	pszSetFolderPath = _T("C:\\Chris\\MyProjects\\BalaGrayIter - Copy\\BalaGray");
//	LPCTSTR	pszSetFolderPath = _T("D:\\temp\\BalaGray");
//	LPCTSTR	pszSetFolderPath = _T("D:\\temp\\BalaGray rev depth 7");
	LPCTSTR	pszSetFolderPath = _T("C:\\Chris\\MyProjects\\BalaGrayIterSD\\444");
	if (!m_setBG.ReadSetData(nSetCode, pszSetFolderPath)) {
		printf("error reading set %X\n", nSetCode);
		return false;
	}
	CString	sSetName(m_setBG.GetName());
	m_setSpan.dw = 0;
	for (int iPlace = 0; iPlace < m_setBG.m_nDigits; iPlace++) {
		m_setSpan.b[iPlace] = sSetName[iPlace] - '0';
	}
	int	iSpacingOverride = -1;	// if non-negative, index of spacing to select, regardless of optimality
	bool	bSkipDups = 0;	// non-zero to exclude duplicate spacing permutations (based on prime forms they produce)
	if (!CalcOptimalSetSpacing(m_setSpan, m_setBestSpacing, iSpacingOverride, bSkipDups))
		return false;
#else	// special case for chord progression as pitch class sets in CSV format
//	LPCTSTR pszPCSPath = _T("C:\\Chris\\MyProjects\\MidiFilter\\MidiFilter\\534 PCS.txt");
//	LPCTSTR pszPCSPath = _T("C:\\Chris\\MyProjects\\MidiFilter\\MidiFilter\\444 PCS check.txt");
//	LPCTSTR pszPCSPath = _T("C:\\Chris\\MyProjects\\MidiFilter\\MidiFilter\\Fine Teeth C pentachord.txt");
	LPCTSTR pszPCSPath = _T("C:\\Chris\\MyProjects\\MidiFilter\\MidiFilter\\Fine Teeth C hexachord.txt");
	CStdioFile	fCSV(pszPCSPath, CFile::modeRead);
	int	nPlaces = 6;
	m_setBestSpacing.SetSize(nPlaces);
	m_setBestSpacing.Clear();
	CIntervalSet::SET	m_setSpan = {0};
	m_setBG.m_arrRow.SetSize(nPlaces);
	CString	sLine;
	while (fCSV.ReadString(sLine)) {
		int	iStart = 0;
		for (int iTone = 0; iTone < nPlaces; iTone++) {
			CString	sToken = sLine.Tokenize(",", iStart);
			int	iPC;
			if (sscanf_s(sToken, "%x", &iPC) != 1 || iPC < 0 || iPC >= 12) {
				printf("parse error\n");
				return false;
			}
			m_setBG.m_arrRow[iTone].Add(static_cast<BYTE>(iPC));
		}
	}
	m_setBG.m_nDigits = nPlaces;
	m_setBG.m_nStates = static_cast<int>(m_setBG.m_arrRow[0].GetSize());
#endif
	CIntervalSet	set;
	set.SetSize(m_setBG.m_nDigits);
	int	nChordSize = m_setBestSpacing.GetSize();
	Init(m_setBG.m_nStates, nChordSize);
	bool	bIsSetReversed = 0;//@@@
	int	nSetRotation = 0;
	int	nSetTranspose = 0;	// Note: Fine Teeth B is +3
#if 0	// set rotation and tone mapping
	if (nSetCode == 0x354) {
		// new {345} based composition (Renumbering) 1st half
		nSetRotation = 6;
		static const BYTE arrToneMap354[6][12] = {
			{1, 2, 0},
			{1, 2, 3, 4, 0},
			{1, 2, 3, 0},
		};
		memcpy(m_arrToneMap, arrToneMap354, sizeof(m_arrToneMap));
		m_bMapTones = 1;
	} else if (nSetCode == 0x435) {
		// new {345} based composition (Renumbering) 2nd half
		nSetRotation = -18;
		static const BYTE arrToneMap435[6][12] = {
			{1, 2, 3, 0},
			{1, 2, 0},
			{1, 2, 3, 4, 0},
		};
		memcpy(m_arrToneMap, arrToneMap435, sizeof(m_arrToneMap));
		m_bMapTones = 1;
	} else if (nSetCode == 0x444) {
		static const BYTE arrToneMap435[6][12] = {
//			{3, 1, 2, 0},{2, 0, 3, 1},{0, 3, 1, 2} // #12412, 363 CTs
//			{2, 1, 0, 3},{2, 3, 1, 0},{3, 1, 0, 2} // #8492 CTs=368 (3rd, 4th, 10th of 12 variants of 444 1.45774)
//			{3, 1, 0, 2},{3, 2, 0, 1},{1, 2, 0, 3} // #12056 CTs=368 (2nd, 6th of 12 variants of 444 1.45774)
//			{1, 3, 2, 0},{1, 3, 0, 2},{1, 0, 2, 3} // #6582 CTs=367 (5th of 12 variants of 444 1.45774)
//			{2, 0, 1, 3},{1, 3, 2, 0},{2, 0, 1, 3} // #7188 CTs=374 (444 rev. depth 5, max span 5, SD 1.07529)
//			{3, 0, 2, 1},{3, 1, 0, 2},{3, 0, 2, 1} // #11443 CTs=371 (two-hour crawl SD 1.41421)
//			{3, 2, 1, 0},{0, 2, 3, 1},{0, 1, 3, 2} // #13321 CTs=373 (two-hour crawl rev depth 7 SD 1.04583)
		};
		memcpy(m_arrToneMap, arrToneMap435, sizeof(m_arrToneMap));
		m_bMapTones = 1;
	}
#endif
	for (int iPerm = 0; iPerm < m_setBG.m_nStates; iPerm++) {
		int	iVal;
		if (bIsSetReversed)
			iVal = m_setBG.m_nStates - 1 - iPerm;
		else
			iVal = iPerm;
		if (nSetRotation) {
			iVal = (iVal - nSetRotation) % m_setBG.m_nStates;
			if (iVal < 0)
				iVal += m_setBG.m_nStates;
		}
		int	nOffset = 0;
		for (int iPlace = 0; iPlace < m_setBG.m_nDigits; iPlace++) {
			int iTone = m_setBG.m_arrRow[iPlace][iVal];
			if (m_bMapTones) {
				iTone = m_arrToneMap[iPlace][iTone];
			}
			int	iPC = iTone + nOffset;
			if (nSetTranspose) {	// if transposing
				iPC = Wrap(iPC + nSetTranspose, OCTAVE);	// offset and apply bounds
			}
			set[iPlace] = iPC;
			nOffset += m_setSpan.b[iPlace] + m_setBestSpacing[iPlace];
		}
		if (CONSOLE_NATTER) {	// non-zero to display harmonizations on console
			printf("%d\t%s\t", iPerm + 1, set.FormatSet().c_str());
			if (!ForteReport(set))
				return false;
		}
		int	iAlias, iHarm, nKey, nRoot;
		if (!FindForte(set, iAlias, iHarm, nKey, nRoot))
			return false;
		const PCS_ALIAS& alias = m_arrPCSAlias[iAlias];
		const HARMONIZATION& harm = alias.arrHarm[iHarm];
		CChord&	chord = m_arrChord[iPerm];
		chord.m_SongChord.nType = harm.iScale;
		chord.m_SongChord.nKey = nKey;
		chord.m_SongChord.nMode = harm.nMode;
		for (int i = 0; i < set.GetSize(); i++) {
			chord.m_SongChord.arrNote[i] = set[i];
		}
		chord.m_iAlias = iAlias;
		chord.m_iHarm = iHarm;
	}
	const int nTonicRepetions = 0;
	if (nTonicRepetions) {
		int	iChord = 0;
		while (iChord < m_arrChord.GetSize()) {
			const CChord&	chord = m_arrChord[iChord];
			if (m_arrPCSAlias[chord.m_iAlias].iHarmFunc == HF_TONIC) {
				CChord	chordDup(chord);
				for (int iRep = 0; iRep < nTonicRepetions; iRep++) {
					m_arrChord.InsertAt(iChord, chordDup);
					iChord++;
				}
			}
			iChord++;
		}
	}
	return true;
}

int LeastInterval(int nNote1, int nNote2)
{
	int	delta = nNote1 - nNote2;
	if (abs(delta) > Gb) {	// if interval too large, invert it
		if (delta > 0)
			delta -= OCTAVE;
		else
			delta += OCTAVE;
	}
	return(delta);
}

void ReadCSV(LPCTSTR pszPath, CByteArray& arrNote)
{
	CStdioFile fNote(pszPath, CFile::modeRead);
	CString	sNote;
	fNote.ReadString(sNote);
	int	iStart = 0;
	CString	sToken;
	while (!(sToken = sNote.Tokenize(",", iStart)).IsEmpty()) {
		int	nNoteVal = atoi(sToken) - 64;
		arrNote.Add(nNoteVal);
	}
}

void MakeToneMap()
{
	int	nPerms = static_cast<int>(m_arrChord.GetSize());
	int	nMaxPCSWidth = 0;
	for (int iPerm = 0; iPerm < nPerms; iPerm++) {
		const CChord&	chord = m_arrChord[iPerm];
		UINT	nMask = 0xFFF;
		for (int iPlace = 0; iPlace < m_setBG.m_nDigits; iPlace++) {
			int	nTone = chord.m_SongChord.arrNote[iPlace];
			nMask &= ~(1 << nTone);
		}
		CPitchClassSet	pcs(chord.m_SongChord.arrNote, m_setBG.m_nDigits);
		int	nPCSWidth = static_cast<int>(pcs.FormatSet().size());
		if (nPCSWidth > nMaxPCSWidth)
			nMaxPCSWidth = nPCSWidth;
	}
	printf("#   %-*s ", nMaxPCSWidth, "PCS");
	for (int iTone = 0; iTone < OCTAVE; iTone++) {
		printf(" %X", iTone);
	}
	printf("  Chord    Mode       Scale\n");
	CString	sDashes('=', nMaxPCSWidth);
	printf("=== %s ", sDashes.GetString());
	for (int iTone = 0; iTone < OCTAVE; iTone++) {
		printf(" =", iTone);
	}
	int	nOffset = 0; // for works that use multiple sets
	CByteArray	arrBassNote;
#if 0
	ReadCSV("C:\\Chris\\MyProjects\\Polymeter\\docs\\test\\345 bass.csv", arrBassNote);
#else
#endif
	printf("  ======== ========== ==========\n");
	for (int iPerm = 0; iPerm < nPerms; iPerm++) {
		const CChord&	chord = m_arrChord[iPerm];
		UINT	nMask = 0xFFF;
		for (int iPlace = 0; iPlace < m_setBG.m_nDigits; iPlace++) {
			int	nTone = chord.m_SongChord.arrNote[iPlace];
			nMask &= ~(1 << nTone);
		}
		CPitchClassSet	pcs(chord.m_SongChord.arrNote, m_setBG.m_nDigits);
		printf("%-3d %-*s", iPerm + 1 + nOffset, nMaxPCSWidth, pcs.FormatSet().c_str());
		for (int iPlace = 0; iPlace < m_setBG.m_nDigits; iPlace++) {
			int	iNextPlace = (iPlace + 1) % m_setBG.m_nDigits;
			int	nNote1 = chord.m_SongChord.arrNote[iPlace];
			int	nNote2 = chord.m_SongChord.arrNote[iNextPlace];
			if (nNote1 > nNote2) {
				swap(nNote1, nNote2);
			}
			int	nInterval = LeastInterval(nNote1, nNote2);
			int	nAbsInterval = abs(nInterval);
//			printf("%d ", nInterval);
			if (nAbsInterval == 1) {
				nMask &= ~((1 << ((nNote1 + 11) % OCTAVE)) | (1 << ((nNote2 + 1) % OCTAVE)));
			} else if (nAbsInterval == 2) {
				int	nAdjacent = nInterval < 0 ? 1 : 11;	// one after or one before
				nMask &= ~(1 << ((nNote1 + nAdjacent) % OCTAVE));
			}
		}
		if (!arrBassNote.IsEmpty()) {
			int	nBassNote = arrBassNote[iPerm + nOffset];
			if ((nMask & (1 << nBassNote)) == 0) {
				printf("error: bass note uses forbidden tone at permutation %d\n", iPerm);
				return;
			}
			for (int iPlace = 0; iPlace < m_setBG.m_nDigits; iPlace++) {
				int	nChordNote = chord.m_SongChord.arrNote[iPlace];
				if (nBassNote == nChordNote) {
					printf("error: bass note doubles chord tone at permutation %d\n", iPerm);
					return;
				}
			}
		}
/*		for (int iPC = 0; iPC < OCTAVE; iPC++) {
			printf("%x", (nMask & (1 << iPC)) != 0);
		}*/
		BYTE	arrPC[OCTAVE] = {0};
		int	nScaleTones = chord.m_ScaleTone.nLen;
		for (int iTone = 0; iTone < nScaleTones; iTone++) {
			int	iMode = (chord.GetMode() + iTone) % nScaleTones;
			int	iPC = chord.m_ScaleTone.scale.arrTone[iMode];
			arrPC[iPC] = iTone + 1;
		}
		printf(" ");
		for (int iPC = 0; iPC < OCTAVE; iPC++) {
			if (nMask & (1 << iPC)) {
				if (arrPC[iPC]) {
					printf(" %d", arrPC[iPC]);
				} else {
					printf(" *");
				}
			} else {
				printf(" .");
			}
		}
		printf("  %-8s %-10s %s %s", chord.GetName(), 
			m_arrModeName[chord.GetMode()],
			m_arrNoteName[chord.m_ScaleTone.scale.arrTone[0]],
			m_arrScaleInfo[chord.GetScale()].pszName
		);
		printf("\n");
	}
}

void MakeToneHtmlTbl()
{
	int	nPerms = static_cast<int>(m_arrChord.GetSize());
	int	nOffset = 0;
	CByteArray	arrBassNote;
	CByteArray	arrMelodyNote;
//	ReadCSV("C:\\Chris\\MyProjects\\Polymeter\\docs\\test\\345 bass.csv", arrBassNote);
//	ReadCSV("C:\\Chris\\MyProjects\\Polymeter\\docs\\test\\345 melody.csv", arrMelodyNote);
	ReadCSV("C:\\Chris\\MyProjects\\Polymeter\\docs\\test\\Fine Teeth A bass tones.csv", arrBassNote);
	CStdioFile	fOut("ToneMap.html", CFile::modeCreate | CFile::modeWrite);
	fOut.WriteString("<!DOCTYPE html>\n<html>\n<head>\n<title>Tone Map</title>\n"
		"<style>\n"
		".bgb { background-color:#ffb8b8; }\n"	// bass: red
		".bgc { background-color:#b8ffb8; }\n"	// chord: green
		".bgm { background-color:#b8b8ff; }\n"	// melody: blue
		".bgmc { background-color:#b8ffff; }\n"	// melody and chord: cyan
		".bgmb { background-color:#ffb8ff; }\n"	// melody and bass: magenta
		"</style>\n"
		"</head>\n<body>\n<table border=1 cellspacing=0>\n"
	);
	CString	sOut, sVal;
	sOut = "<tr><th>#</th><th>PCS</th>";
	for (int i = 0; i < OCTAVE; i++) {
		sVal.Format("%x", i);
		sOut += "<th>" + sVal + "</th>";
	}
	sOut += "<th>Chord</th><th>Mode</th><th>Key</th><th>Scale</th></tr>\n";
	fOut.WriteString(sOut);
	for (int iPerm = 0; iPerm < nPerms; iPerm++) {
		const CChord&	chord = m_arrChord[iPerm];
		CPitchClassSet	pcs(chord.m_SongChord.arrNote, m_setBG.m_nDigits);
		CString	sPCS(pcs.FormatSet().c_str());
		sPCS.Remove('[');
		sPCS.Remove(']');
		sOut.Format("<tr><td>%d</td><td>%s</td>", iPerm + 1 + nOffset, sPCS);
/*		for (int iPC = 0; iPC < OCTAVE; iPC++) {
			printf("%x", (nMask & (1 << iPC)) != 0);
		}*/
		BYTE	arrPC[OCTAVE] = {0};
		int	nScaleTones = chord.m_ScaleTone.nLen;
		for (int iTone = 0; iTone < nScaleTones; iTone++) {
			int	iMode = (chord.GetMode() + iTone) % nScaleTones;
			int	iPC = chord.m_ScaleTone.scale.arrTone[iMode];
			arrPC[iPC] = iTone + 1;
		}
		for (int iPC = 0; iPC < OCTAVE; iPC++) {
			CString	sBGColor;
			if (arrPC[iPC])
				sVal.Format("%d", arrPC[iPC]);
			else
				sVal = "&nbsp;";
			BYTE	nBass = arrBassNote.GetSize() ? (arrBassNote[iPerm + nOffset] % 12) : -1;
			BYTE	nMel = arrMelodyNote.GetSize() ? (arrMelodyNote[iPerm] % 12) : -1;
			if (iPC == nBass) {
				if (iPC == nMel) {
					sBGColor = " class=\"bgmb\"";	// melody and bass: magenta
				} else {
					sBGColor = " class=\"bgb\"";	// bass: red
				}
			}
			else if (pcs.Find(iPC) >= 0) {
				if (iPC == nMel) {
					sBGColor = " class=\"bgmc\"";	// melody and chord: cyan
				} else {
					sBGColor = " class=\"bgc\"";	// chord: green
				}
			} else if (iPC == nMel) {
				sBGColor = " class=\"bgm\"";	// melody: blue
			}
			sOut += "<td" + sBGColor + ">" + sVal + "</td>";
		}
		sOut += "<td>" + chord.GetName()
			+ "</td><td>" + m_arrModeName[chord.GetMode()]
			+ "</td><td>" + m_arrNoteName[chord.m_ScaleTone.scale.arrTone[0]]
			+ "</td><td>" + m_arrScaleInfo[chord.GetScale()].pszName + "</td></tr>\n";
		fOut.WriteString(sOut);
	}
	fOut.WriteString("</table>\n</body>\n</html>\n");
}

int Factorial(int n)
{
	int	x = 1;
	for (int i = 1; i <= n; i++) {
		x *= i;
	}
	return x;
}

class CCommonToneCrawler {
public:
	CCommonToneCrawler();
	typedef CArray<CIntervalSet::SET, CIntervalSet::SET&> CPermArray;
	typedef CArray<CPermArray, CPermArray&> CPermArrayArray;
	CPermArrayArray	m_arrCTPerm;
	CDWordArray	 m_arrSpan;
	int		m_nMinCommon;
	int		m_nMaxCommon;
	int		m_nCommonPerms;
	int		m_nDigits;
	int		m_nHexSet;
	CIntervalSet::SET	m_arrSet;
	CStdioFile	m_fOut;
	void	CrawlCommonTones(int iDepth);
};

CCommonToneCrawler::CCommonToneCrawler()
{
	m_nMinCommon = INT_MAX;
	m_nMaxCommon = 0;
	m_nCommonPerms = 0;
	m_nHexSet = 0;
	m_arrSet.dw = 0;
	m_nDigits = 0;
}

void CCommonToneCrawler::CrawlCommonTones(int iDepth)
{
	int	nCTPerms = static_cast<int>(m_arrCTPerm[iDepth].GetSize());
	for (int iPos = 0; iPos < nCTPerms; iPos++) {
		int	nCurSpan = m_arrSet.b[iDepth];
		for (int iVal = 0; iVal < nCurSpan; iVal++) {
			m_arrToneMap[iDepth][iVal] = m_arrCTPerm[iDepth][iPos].b[iVal];
		}
		if (iDepth < m_nDigits - 1) {
			CrawlCommonTones(iDepth + 1);
			continue;
		}
		if (!ProcessIntervalSet(m_nHexSet)) {
			printf("ERROR!\n"); 
			return;
		}
		MakeScalesAndChords();
		int	nPerms = static_cast<int>(m_arrChord.GetSize());
		int	nCommon = 0;
		for (int iPerm = 0; iPerm < nPerms; iPerm++) {
			const CChord&	chord = m_arrChord[iPerm];
			int	iNext = iPerm + 1;
			if (iNext >= nPerms)
				iNext = 0;
			const CChord&	chordNext = m_arrChord[iNext];
			int	nTones = chord.m_ScaleTone.nLen;
			for (int iTone = 0; iTone < nTones; iTone++) {
				if (Find(chord.m_ScaleTone.scale.arrTone[iTone], chordNext.m_ScaleTone.scale.arrTone, nTones) >= 0) {
					nCommon++;
				}
			}
		}
		if (nCommon < m_nMinCommon) {
			m_nMinCommon = nCommon;
		}
		if (nCommon > m_nMaxCommon) {
			m_nMaxCommon = nCommon;
		}
		CString	s, t;
		s.Format("%d CTs=%d ", m_nCommonPerms, nCommon);
		for (int iDigit = 0; iDigit < m_nDigits; iDigit++) {
			if (iDigit)
				s += ',';
			s += '{';
			int	nSpan = m_arrSet.b[iDigit];
			for (int iSpan = 0; iSpan < nSpan; iSpan++) {
				if (iSpan)
					s += ", ";
				t.Format("%d", m_arrToneMap[iDigit][iSpan]);
				s += t;
			}
			s += "}";
		}
		m_fOut.WriteString(s + '\n');
		m_nCommonPerms++;
	}
}

void AnalyzeCommonTones(int nSet)
{
	CCommonToneCrawler	ctc;
	ctc.m_arrSet.dw = 0;	// convert hexadecimal set code to interval set
	ctc.m_nHexSet = nSet;
	int	nPlaces = 0;
	for (int iPlace = MAX_PLACES - 1; iPlace >= 0; iPlace--) {
		int	nRadix = (nSet >> (iPlace * 4)) & 0xf;
		if (nRadix) {
			ctc.m_arrSet.b[nPlaces] = nRadix;
			nPlaces++;
		}
	}
	ctc.m_fOut.Open("AnalCommonTone.txt", CFile::modeCreate | CFile::modeWrite);
	ctc.m_arrCTPerm.SetSize(nPlaces);
	ctc.m_nDigits = nPlaces;
	CByteArray	arrTemp;
	for (int iPlace = 0; iPlace < nPlaces; iPlace++) {
		int	nSpan = ctc.m_arrSet.b[iPlace];
		int	nPerms = Factorial(nSpan);
		ctc.m_arrCTPerm[iPlace].SetSize(nPerms);
		arrTemp.SetSize(nSpan);
		for (int iVal = 0; iVal < nSpan; iVal++) {
			arrTemp[iVal] = iVal;
		}
		int	gen_result = gen_perm_rep_lex_init(nSpan);
		int	iPerm = 0;
		while (gen_result == GEN_NEXT) {
			for (int iVal = 0; iVal < nSpan; iVal++) {
				ctc.m_arrCTPerm[iPlace][iPerm].b[iVal] = arrTemp[iVal];
			}
			gen_result = gen_perm_rep_lex_next(arrTemp.GetData(), nSpan);
			iPerm++;
		}
	}
	m_bMapTones = true;
	ctc.CrawlCommonTones(0);
	CString	s;
	s.Format("common perms = %d, min = %d, max = %d\n", ctc.m_nCommonPerms, ctc.m_nMinCommon, ctc.m_nMaxCommon);
	ctc.m_fOut.WriteString(s);
}

bool Main()
{
//	TestPitchClassSet();
//	TestIntervalSetPacking();
//	TestIntervalSetPermutation();
//	TestIntervalSetForte();
//	ValidateSetClasses("Wikipedia set classes.csv");
	if (!TestHarmonizations()) return false;
//	int	nSet = 0x2233;
//	int	nSet = 0x2332;	// override = 11
//	int	nSet = 0x2222;
//	int	nSet = 0x2224;
//	int	nSet = 0x223;
//	int	nSet = 0x22222;	// B section, override = 13
//	int	nSet = 0x22322; // A section
//	int	nSet = 0x343; // override = 6, reversed
	// latest experiment
//	int	nSet = 0x354;
//	int	nSet = 0x435;
	int	nSet = 0x444;
	if (!ProcessIntervalSet(nSet)) {
		printf("ERROR!\n"); 
		return false;
	}
	MakeScalesAndChords();
//	MakeTracks();
	MakeTracksSimple(nSet);
	MakeToneMap();
//	MakeToneHtmlTbl();
//	AnalyzeCommonTones(nSet);
	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			TRY {
				Main();
			}
			CATCH (CException, e) {
				TCHAR	szErrorMsg[MAX_PATH];
				e->GetErrorMessage(szErrorMsg, _countof(szErrorMsg));
				_tprintf(_T("%s\n"), szErrorMsg);
			}
			END_CATCH
			fgetc(stdin);
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}
	return nRetCode;
}
