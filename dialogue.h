#pragma once
#include <vector>
#include <string>
#include <cstddef>
#include <memory>
#include <new>
#include "window.h"

struct DialogueBoxData
{
	DWORD style; // Ej DS_SETFONT
	short w;
	short h;
	std::wstring title;
};

namespace DlgItemClass
{
	enum DlgItemClass_t : uint16_t
	{
		button = 0x80,
		edit,
		label, // static går ej
		listBox,
		scrollBar,
		comboBox
	};
}

struct DialogueBoxItemData
{
	DWORD style;
	short x;
	short y;
	short w;
	short h;
	WORD id;
	uint16_t wndClass;
	std::wstring title;
};

struct DialogueBox
{
	DialogueBoxData db;
	std::vector<DialogueBoxItemData> dbis;
	DLGPROC dlgProc;
};

INT_PTR displayDialogueBox(const DialogueBox& db, HWND hWnd);