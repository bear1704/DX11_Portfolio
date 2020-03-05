#pragma once
#include <windows.h>
#include <tchar.h>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <memory>
#include <cmath>

using namespace std;

#pragma comment(lib, "msimg32")

#define PCORE_RUN(title,x,y,w,h) \
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)\
{\
 Sample sample;\
 sample.InitWindow(hInstance, title,x, y, w, h);\
 sample.Run();\
}



const float kFloatEpsilon = static_cast<float>(0.001f);
const float kPI = static_cast<float>(3.141592654f);

#define DEGREE_TO_RADIAN(degree) (degree*(kPI/180.0f))
#define RADIAN_TO_DEGREE(radian) (radian*(180.0f/kPI))

typedef std::wstring unicode_string;
typedef std::basic_string<TCHAR> multibyte_string;
typedef std::basic_string<char> c_string;
using strkey = std::wstring;

template<typename T>
using Sptr = std::shared_ptr<T>;
template<typename T>
using Uptr = std::unique_ptr<T>;
static std::wstring multibyte_to_unicode_str(std::string str)
{
	std::wstring ret = std::wstring(str.begin(), str.end());
	return ret;
}
static std::string unicode_to_multibyte_str(std::wstring str)
{
	std::string ret = std::string(str.begin(), str.end());
	return ret;
}

static void StringToWstring(std::string str, std::wstring& wstr)
{
	wstr = std::wstring(str.begin(), str.end());
}
static void WstringToString(std::wstring wstr, std::string& str)
{
	str = std::string(wstr.begin(), wstr.end());
}

static std::string multibyte_to_string(multibyte_string mstr)
{
	std::string ret = std::string(mstr.begin(), mstr.end());
	return ret;
}

static multibyte_string string_to_multibyte(std::string str)
{
	multibyte_string ret;
	ret.assign(str.begin(), str.end());
	return ret;
}


enum class KEYSTAT
{
	KEY_FREE,
	KEY_PUSH,
	KEY_HOLD,
	KEY_UP
};

enum class ObjectLoadType //로드 시 중복방지
{
	CHARACTER,
	UI,
	MAP,
	OBJECT,
	ETC_SPRITE,
};
enum class Type
{
	PLAYER,
	OTHER_PLAYER,
	MONSTER,
	NPC,
	MAP,
	BOSS_MONSTER,

};


enum class SIDE
{
	LEFT,
	RIGHT,
};

enum class PXY
{
	X,
	Y,

};


enum class FSM_State { IDLE, MOVE, ATTACK, JUMP, HIT, DEAD, REVIVE,BOSS_IDLE, BOSS_SKILL1, BOOL_SKILL2, BOSS_DEAD , BOSS_HIT,ERR };
enum class FSM_Event { INPUT_NONE, INPUT_MOVE, INPUT_ATTACK, ATTACK_END, HPEMPTY,  TIME_OUT,
					INPUT_JUMP, HIT , MOB_TIME_OUT, MOB_JUMP_TIME_OUT, MOB_CHASE ,JUMP_END, 
						};


enum class ANIMATIONTYPE
{
	IDLE,
	MOVE,
	ATTACK,
	JUMP,
	HIT,
	DEAD,
	SKILLONE,
	SKILLTWO,
	SKILLTHREE,
	ERROR_OCCUR,

};

enum class ObjectType
{
	PLAYER_CHARACTER,
	MONSTER,
	NPC,
	BOSS_MONSTER,
	ITEM,
	ERROR_OCCUR,
};



struct PInputActionMap
{
	KEYSTAT aKey;
	KEYSTAT sKey;
	KEYSTAT dKey;
	KEYSTAT fKey;
	KEYSTAT qKey;
	KEYSTAT wKey;
	KEYSTAT eKey;
	KEYSTAT leftClick;
	KEYSTAT rightClick;
	KEYSTAT middleClick;
	KEYSTAT exitKey;
	KEYSTAT jumpKey;
	KEYSTAT leftArrowKey;
	KEYSTAT rightArrowKey;
	KEYSTAT upArrowKey;
	KEYSTAT downArrowKey;
	KEYSTAT attackKey;
	KEYSTAT HomeKey;
	KEYSTAT EndKey;
	KEYSTAT DeleteKey;
	KEYSTAT PageUpKey;
	KEYSTAT PageDownKey;
	KEYSTAT InsertKey;
	
};




typedef struct pPoint_
{

	float x;
	float y;
	pPoint_() {};
	pPoint_(float fx, float fy) { x = fx; y = fy; }


}pPoint, Vector2, SizeXY;

struct FLOAT_RECT
{
	float left;
	float top;
	float right;
	float bottom;
};

static ANIMATIONTYPE WstringToAnimationtype(std::wstring wstr)
{
	if (wstr.compare(L"IDLE") == 0)
		return ANIMATIONTYPE::IDLE;
	else if (wstr.compare(L"MOVE") == 0)
		return ANIMATIONTYPE::MOVE;
	else if (wstr.compare(L"ATTACK") == 0)
		return ANIMATIONTYPE::ATTACK;
	else if (wstr.compare(L"JUMP") == 0)
		return ANIMATIONTYPE::JUMP;
	else if (wstr.compare(L"HIT") == 0)
		return ANIMATIONTYPE::HIT;
	else if (wstr.compare(L"DEAD") == 0)
		return ANIMATIONTYPE::DEAD;
	else if (wstr.compare(L"SKILLONE") == 0)
		return ANIMATIONTYPE::SKILLONE;
	else if (wstr.compare(L"SKILLTWO") == 0)
		return ANIMATIONTYPE::SKILLTWO;
	else if (wstr.compare(L"SKILLTHREE") == 0)
		return ANIMATIONTYPE::SKILLTHREE;

	return ANIMATIONTYPE::ERROR_OCCUR;
}

static std::string FsmStateToString(FSM_State state)
{ 
	std::string str;
	
	switch (state)
	{
	case FSM_State::IDLE:
		str = "IDLE";
		break;
	case FSM_State::MOVE:
		str = "MOVE";
		break;
	case FSM_State::ATTACK:
		str = "ATTACK";
		break;
	case FSM_State::JUMP:
		str = "JUMP";
		break;
	case FSM_State::HIT:
		str = "HIT";
		break;
	case FSM_State::DEAD:
		str = "DEAD";
		break;
	case FSM_State::REVIVE:
		str = "REVIVE";
		break;
	}

	return str;

}
static ObjectType StringToObjectType(std::string str)
{
	if (str.compare("player") == 0)
		return ObjectType::PLAYER_CHARACTER;
	else if (str.compare("monster") == 0)
		return ObjectType::MONSTER;
	else if (str.compare("item") == 0)
		return ObjectType::ITEM;
	else if (str.compare("npc") == 0)
		return ObjectType::NPC;

	return ObjectType::ERROR_OCCUR;

}

static FSM_Event FsmStateToFsmEvent(FSM_State state)
{
	FSM_Event fsm_event;

	switch (state)
	{
	case FSM_State::IDLE:
		fsm_event = FSM_Event::INPUT_NONE;
		break;
	case FSM_State::MOVE:
		fsm_event = FSM_Event::INPUT_MOVE;
		break;
	case FSM_State::JUMP:
		fsm_event = FSM_Event::INPUT_JUMP;
		break;
	case FSM_State::HIT:
		fsm_event = FSM_Event::HIT;
		break;
	case FSM_State::REVIVE:
		fsm_event = FSM_Event::MOB_TIME_OUT;
		break;
	case FSM_State::DEAD:
		fsm_event = FSM_Event::HPEMPTY;
		break;
	case FSM_State::ATTACK:
		fsm_event = FSM_Event::INPUT_ATTACK;
		break;
	default:
		fsm_event = FSM_Event::INPUT_NONE;
		break;
	}

	return fsm_event;
}


extern HWND g_hWnd;
extern POINT g_MousePos;
extern float g_fGameTimer;
extern float g_SecondPerFrame;
extern PInputActionMap g_InputActionMap;
extern HDC g_handle_off_screenDC;
extern HDC g_handle_screenDC;
extern HINSTANCE g_hInstance;
extern FLOAT_RECT g_rectangle_client;

template <typename T>
class PSingleton
{
public:
	static T& GetInstance()
	{
		static T singleton;
		return singleton;
	}
public:
	virtual bool Init() = 0;
	virtual bool Frame() = 0;
	virtual bool Render() = 0;
	virtual bool Release() = 0;

};

namespace DX
{

	namespace STD
	{

	}

}