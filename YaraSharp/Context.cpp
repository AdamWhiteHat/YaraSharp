#include "Stdafx.h"

//	CContext
namespace YaraSharp
{
	//	�����������
	CContext::CContext() { ErrorUtility::ThrowOnError(yr_initialize()); }
	//	����������
	CContext::~CContext() { ErrorUtility::ThrowOnError(yr_finalize()); }
}