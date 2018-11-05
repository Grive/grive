#include <sys/ioctl.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>

#include "ProgressBar.hh"

namespace gr
{

ProgressBar::ProgressBar(): showProgressBar(false), last(1000)
{
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::setShowProgressBar(bool showProgressBar)
{
	this->showProgressBar = showProgressBar;
}

unsigned short int ProgressBar::determineTerminalSize()
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
}

void ProgressBar::printBytes(u64_t bytes)
{
	if (bytes >= 1024*1024*1024)
		printf("%.3f GB", (double)bytes/1024/1024/1024);
	else if (bytes >= 1024*1024)
		printf("%.3f MB", (double)bytes/1024/1024);
	else
		printf("%.3f KB", (double)bytes/1024);
}

void ProgressBar::reportProgress(u64_t total, u64_t processed)
{
	if (showProgressBar && total)
	{
		// libcurl seems to process more bytes then the actual file size :)
		if (processed > total)
			processed = total;
		double fraction = (double)processed/total;

		int point = fraction*1000;
		if (this->last < 1000 || point != this->last)
		{
			// do not print 100% progress multiple times (it will duplicate the progressbar)
			this->last = point;

			// 10 for prefix of percent and 26 for suffix of file size
			int availableSize = determineTerminalSize() - 36;
			int totalDots;
			if (availableSize > 100)
				totalDots = 100;
			else if (availableSize < 0)
				totalDots = 10;
			else
				totalDots = availableSize;

			int dotz = round(fraction * totalDots);
			int count = 0;
			// delete previous output line
			printf("\r  [%3.0f%%] [", fraction * 100);
			for (; count < dotz - 1; count++)
				putchar('=');
			putchar('>');
			for (; count < totalDots - 1; count++)
				putchar(' ');
			printf("] ");
			printBytes(processed);
			putchar('/');
			printBytes(total);
			printf("\33[K\r");
			if (point == 1000)
				putchar('\n');
			fflush(stdout);
		}
	}
}

}
