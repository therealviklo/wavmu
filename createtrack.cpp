#include "createtrack.h"

using namespace std::literals;

const DialogueBox createTrack{
	DialogueBoxData{
		WS_CAPTION | WS_POPUPWINDOW,
		110,
		50,
		L"Skapa spår"s
	},
	std::vector<DialogueBoxItemData>{
		{
			WS_CHILD | WS_VISIBLE | SS_LEFT,
			5,
			5,
			100,
			10,
			ID::instrNameLbl,
			DlgItemClass::label,
			L"Intrumentnamn (tom för pip)"s
		},
		{
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP,
			5,
			20,
			100,
			10,
			ID::instrNameBox,
			DlgItemClass::edit,
			L""s
		},
		{
			WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
			70,
			35,
			30,
			10,
			ID::createTrackBtn,
			DlgItemClass::button,
			L"Skapa"s
		}
	},
	&createTrackDlgProc
};

INT_PTR __stdcall createTrackDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static UHandle<HFONT, &DeleteObject> font(CreateFontW(
		-12,
		0,
		0,
		0,
		FW_DONTCARE,
		FALSE,
		FALSE,
		FALSE,
		ANSI_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		L"Calibri"
	));
	switch (msg)
	{
		// struct CreateTrackDlgHotKey
		// {
		// 	enum CreateTrackDlgHotKey_t : int
		// 	{
		// 		esc
		// 	};
		// };
		case WM_INITDIALOG:
		{
			auto setFont = [&](int item){
				SendMessageW(
					GetDlgItem(hDlg, item),
					WM_SETFONT,
					WPARAM(font.get()),
					TRUE
				);
			};
			setFont(ID::instrNameLbl);
			setFont(ID::instrNameBox);
			setFont(ID::instrNameLbl);
			// RegisterHotKey(
			// 	hDlg,
			// 	CreateTrackDlgHotKey::esc,
			// 	0,
			// 	VK_ESCAPE
			// );
		}
		return TRUE;
		// case WM_DESTROY:
		// {
		// 	UnregisterHotKey(
		// 		hDlg,
		// 		CreateTrackDlgHotKey::esc
		// 	);
		// }
		// return TRUE;
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_ESCAPE:
				{
					EndDialog(hDlg, 0);
				}
				return TRUE;
			}
		}
		break;
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID::createTrackBtn:
				{
					try
					{
						constexpr int bufsize = MAX_PATH;
						std::unique_ptr<wchar_t[]> buf = std::make_unique<wchar_t[]>(bufsize);
						SetLastError(0);
						if (GetDlgItemTextW(
							hDlg,
							ID::instrNameBox,
							buf.get(),
							bufsize
						) != 0)
						{
							EndDialog(hDlg, (INT_PTR)buf.release());
							return TRUE;
						}
						else if (!GetLastError())
						{
							buf[0] = L'\0';
							EndDialog(hDlg, (INT_PTR)buf.release());
							return TRUE;
						}
					}
					catch (...) {}
					EndDialog(hDlg, 0);
				}
				return TRUE;
			}
		}
		break;
		case WM_CLOSE:
		{
			EndDialog(hDlg, 0);
		}
		return TRUE;
	}
	return FALSE;
}