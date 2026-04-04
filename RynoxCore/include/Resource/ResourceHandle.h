#pragma once

#include "Base/BaseTypes.h"

namespace Rynox 
{
    struct ResourceHandle
	{
		uint32_t Index = INVALID_UINT32_VALUE;
		uint32_t Gen = INVALID_UINT32_VALUE;

        bool IsValid() const
        {
            return Index != INVALID_UINT32_VALUE;
        }

		bool operator==(const ResourceHandle& other) const
		{
			return Index == other.Index && Gen == other.Gen;
		}
	};
}