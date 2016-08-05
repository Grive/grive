#pragma once

#include "util/Types.hh"

namespace gr {

class Progress
{
public:
	virtual void reportProgress(u64_t total, u64_t processed) = 0;
};

}
;
