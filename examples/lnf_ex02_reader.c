/* 

 Copyright (c) 2013-2015, Tomas Podermanski
    
 This file is part of libnf.net project.

 Libnf is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Libnf is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with libnf.  If not, see <http://www.gnu.org/licenses/>.

*/

/* Simple reader of nfdump files.  To demostrate functionality */ 
/* records are matched against two filters */

#include <libnf.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define FILENAME "./test-file.tmp"
#define FILTER1 "src port > 80"
#define FILTER2 "in if 2"

#define LLUI long long unsigned int

int main(int argc, char **argv) {

	lnf_file_t *filep;
	lnf_rec_t *recp;
	lnf_filter_t *filterp1, *filterp2;
	lnf_brec1_t brec;
	char *filter1 = FILTER1;
	char *filter2 = FILTER2;
	uint32_t input, output;
	char buf[LNF_MAX_STRING];
	int res;

	int i = 0;
	int match1 = 0;
	int match2 = 0;
	int if1 = 0;
	int if2 = 0;

    int print = 1;
    int filter = 1;
    int fget = 1;
    int loop_read = 0;
    int flags = 0;
    char *filename = FILENAME;
    int c;

	while ((c = getopt (argc, argv, "pPFGlf:1:2:")) != -1) {
		switch (c) {
			case 'p':
				print = 0;
				break;
			case 'P':
				print = 0;
				break;
			case 'G':
				fget = 0;
				break;
			case 'F':
				filter = 0;
				break;
			case 'f':
				filename = optarg;
				break;
			case 'l':
				loop_read = 1;
				break;
			case '1':
				filter1 = optarg;
				break;
			case '2':
				filter2 = optarg;
				break;
			case '?':
				printf("Usage: %s [ -p ] [ -f <input file name> ] [ -l ] [ -1 <filter1> ] [ -2 <filter2> ]\n", argv[0]);
				printf(" -p : do not print records to stdout\n");
				printf(" -F : do not use filters\n");
				printf(" -G : do not use lng_rec_fget\n");
				printf(" -l : loop read\n");
				exit(1);
		}
	}


	flags = LNF_READ;
	if (loop_read) {
		flags = LNF_READ|LNF_READ_LOOP;
	}
	
	if (lnf_open(&filep, filename, flags, NULL) != LNF_OK) {
		fprintf(stderr, "Can not open file %s\n", filename);
		exit(1);
	}


	if ((res = lnf_filter_init(&filterp1, filter1)) != LNF_OK) {
		fprintf(stderr, "Can not init filter1 '%s'\n", filter1);
		if (res == LNF_ERR_OTHER_MSG) {
			lnf_error(buf, LNF_MAX_STRING);
			fprintf(stderr, "%s\n", buf);
		}
		exit(1);
	}

	if ((res = lnf_filter_init(&filterp2, filter2)) != LNF_OK) {
		fprintf(stderr, "Can not init filter2 '%s'\n", filter2);
		lnf_error(buf, LNF_MAX_STRING);
		if (res == LNF_ERR_OTHER_MSG) {
			lnf_error(buf, LNF_MAX_STRING);
			fprintf(stderr, "%s\n", buf);
		}
		exit(1);
	}

	lnf_rec_init(&recp);

	while (lnf_read(filep, recp) != LNF_EOF) {

		if (fget) {
			lnf_rec_fget(recp, LNF_FLD_INPUT, &input);
		
			lnf_rec_fget(recp, LNF_FLD_BREC1, &brec);
			lnf_rec_fget(recp, LNF_FLD_INPUT, &input);
			lnf_rec_fget(recp, LNF_FLD_OUTPUT, &output);
		}
		i++;

		match1 = 0;
		match2 = 0;

		if (filter) {
			if (lnf_filter_match(filterp1, recp)) {
				if1++;
				match1 = 1;
			}
			if (lnf_filter_match(filterp2, recp)) {
				if2++;
				match2 = 1;
			}
		}

		if (print) {
			char sbuf[INET6_ADDRSTRLEN];
			char dbuf[INET6_ADDRSTRLEN];
	
			inet_ntop(AF_INET6, &brec.srcaddr, sbuf, INET6_ADDRSTRLEN);
			inet_ntop(AF_INET6, &brec.dstaddr, dbuf, INET6_ADDRSTRLEN);

			printf(" %s :%d -> %s :%d %d -> %d %llu %llu %llu [%d %d]\n", 
					sbuf, brec.srcport, 
					dbuf, brec.dstport,  
					input, output, 
					(LLUI)brec.pkts, (LLUI)brec.bytes, (LLUI)brec.flows, 
					match1, match2);
		}
	}

	printf("Total records: %d\n", i);
	printf("%d records matched by filter1 '%s'\n", if1, filter1);
	printf("%d records matched by filter2 '%s'\n", if2, filter2);

	lnf_rec_free(recp);
	lnf_filter_free(filterp1);
	lnf_filter_free(filterp2);
	lnf_close(filep);

	return 0;
}


