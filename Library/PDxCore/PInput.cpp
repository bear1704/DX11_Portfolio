#include "PInput.h"
#include <iostream>

POINT g_MousePos;
PInputActionMap g_InputActionMap;


PInput::PInput()
{

}

bool PInput::Init()
{
	std::fill_n(keyboardKeyState, 256, KEYSTAT::KEY_FREE);
	std::fill_n(mouseKeyState, 3, KEYSTAT::KEY_FREE);
	std::fill_n(mouseBeforeState, 3, KEYSTAT::KEY_FREE);
	return false;
}

bool PInput::Frame()
{
	g_InputActionMap.aKey = KeyCheck('A');
	g_InputActionMap.sKey = KeyCheck('S');
	g_InputActionMap.dKey = KeyCheck('D');
	g_InputActionMap.fKey = KeyCheck('F');
	g_InputActionMap.qKey = KeyCheck('Q');
	g_InputActionMap.wKey = KeyCheck('W');
	g_InputActionMap.eKey = KeyCheck('E');
	g_InputActionMap.HomeKey = KeyCheck(VK_HOME);
	g_InputActionMap.DeleteKey = KeyCheck(VK_DELETE);
	g_InputActionMap.EndKey = KeyCheck(VK_END);
	g_InputActionMap.PageUpKey = KeyCheck(VK_PRIOR);
	g_InputActionMap.PageDownKey = KeyCheck(VK_NEXT);
	g_InputActionMap.upArrowKey = KeyCheck(VK_UP);
	g_InputActionMap.downArrowKey = KeyCheck(VK_DOWN);
	g_InputActionMap.leftArrowKey = KeyCheck(VK_LEFT);
	g_InputActionMap.rightArrowKey = KeyCheck(VK_RIGHT);
	g_InputActionMap.InsertKey = KeyCheck(VK_INSERT);
	g_InputActionMap.attackKey = KeyCheck(VK_CONTROL);
	

	g_InputActionMap.exitKey = KeyCheck(VK_ESCAPE);
	g_InputActionMap.jumpKey = KeyCheck(VK_SPACE);

	GetCursorPos(&mousePos);
	ScreenToClient(g_hWnd, &mousePos);
	g_MousePos = mousePos;

	for (int i = 0; i < 3; i++)
	{
		if (mouseBeforeState[i] == KEYSTAT::KEY_PUSH)
		{
			if (mouseKeyState[i] == KEYSTAT::KEY_PUSH)
			{
				mouseKeyState[i] = KEYSTAT::KEY_HOLD;
			}
		}
		mouseBeforeState[i] = mouseKeyState[i];
	}
	g_InputActionMap.leftClick = mouseKeyState[0];
	g_InputActionMap.middleClick = mouseKeyState[1];
	g_InputActionMap.rightClick = mouseKeyState[2];


	return true;
}

bool PInput::Release()
{
	return false;
}

bool PInput::Render()
{
	return false;
}

LRESULT PInput::MsgProc(MSG msg)
{
	mouseKeyState[0] = KEYSTAT::KEY_FREE;
	mouseKeyState[1] = KEYSTAT::KEY_FREE;
	mouseKeyState[2] = KEYSTAT::KEY_FREE;

	switch (msg.message)
	{
	case WM_LBUTTONDOWN:
		mouseKeyState[0] = KEYSTAT::KEY_PUSH;
		break;
	case WM_LBUTTONUP:
		mouseKeyState[0] = KEYSTAT::KEY_UP;
		break;
	case WM_MBUTTONDOWN:
		mouseKeyState[1] = KEYSTAT::KEY_PUSH;
		break;
	case WM_MBUTTONUP:
		mouseKeyState[1] = KEYSTAT::KEY_UP;
		break;
	case WM_RBUTTONDOWN:
		mouseKeyState[2] = KEYSTAT::KEY_PUSH;
		break;
	case WM_RBUTTONUP:
		mouseKeyState[2] = KEYSTAT::KEY_UP;
		break;
	}
	return 1;
	
}


PInput::~PInput()
{
}


KEYSTAT PInput::KeyCheck(DWORD key)
{
	INT sKey = GetAsyncKeyState(key);

	if (sKey & 0x8000)
	{
		if (keyboardKeyState[key] == KEYSTAT::KEY_FREE || keyboardKeyState[key] == KEYSTAT::KEY_UP)
		{
			keyboardKeyState[key] = KEYSTAT::KEY_PUSH;
		}
		else
		{
			keyboardKeyState[key] = KEYSTAT::KEY_HOLD;
		}
	}
	else
	{
		if (keyboardKeyState[key] == KEYSTAT::KEY_PUSH || keyboardKeyState[key] == KEYSTAT::KEY_HOLD)
		{
			keyboardKeyState[key] = KEYSTAT::KEY_UP;
		}
		else
		{
			keyboardKeyState[key] = KEYSTAT::KEY_FREE;
		}

	}
	return keyboardKeyState[key];
}

DWORD PInput::Key(DWORD key_)
{
	int key = GetAsyncKeyState(key_);
	return key;	
}
