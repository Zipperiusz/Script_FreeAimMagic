#pragma once
#include "Script.h"

gSScriptInit & GetScriptInit();
#pragma warning( push )
#pragma warning( disable : 4127 )
#pragma warning( pop )

#include "Game.h"


class mCTest :
	public eCEngineComponentBase
{
	public: virtual void	Process(void);
public: virtual			~mCTest(void);

	private:
		static bTPropertyObject<mCTest, eCEngineComponentBase> ms_PropertyObjectInstance_mCTest;

	public:
		mCTest(void);

	private:
	mCTest(mCTest const &);
	GEBool testKeyPressed;
	GEBool aimKeyPressed;
	GEBool actionKeyPressed;

	mCTest const &	operator = (mCTest const &);
};

