#pragma once

#include "util/Progress.hh"

namespace gr {

class ProgressBar: public Progress
{
public:
	ProgressBar();
	virtual ~ProgressBar();

	void reportProgress(u64_t total, u64_t processed);
	void setShowProgressBar(bool showProgressBar);

private:
	static void printBytes(u64_t bytes);
	static unsigned short int determineTerminalSize();

	bool showProgressBar;
	int last;
};

}
;
