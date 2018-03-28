/*
	Copyright (c) 2018 Dominic Dwyer <dom@itsallbroken.com>
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.

	clang -lpmc pmc-crash.c -o pmc-crash
*/

#include <stdio.h>
#include <stdlib.h>
#include <pmc.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (pmc_init() != 0)
	{
		printf("pmc_init %s", strerror(errno));
		exit(-1);
	}

	pmc_id_t pmc_id = 0;
	if (pmc_allocate("instructions", PMC_MODE_TC, 0, PMC_CPU_ANY, &pmc_id) != 0)
	{
		printf("pmc_allocate %s", strerror(errno));
		exit(-1);
	}

	printf("attaching to self\n");
	if (pmc_attach(pmc_id, 0) != 0)
	{
		printf("pmc_attach %s", strerror(errno));
		exit(-1);
	}

	if (pmc_start(pmc_id) != 0)
	{
		printf("pmc_start %s", strerror(errno));
		exit(-1);
	}

	pmc_value_t v = 0;
	for (int i = 0; i < 100; i++)
	{
		pmc_read(pmc_id, &v);
	}
	pmc_stop(pmc_id);

	printf("success, detaching...\n");
	pmc_detach(pmc_id, 0);
	pmc_release(pmc_id);

	printf("ok?");
}