#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <ctype.h>

#define USAGE "Usage: /path/to/alocfind <memory address>\n"

void
outusage() {
	fprintf(stderr, USAGE);
	exit(-1);
}

int
ch2int(char ch) {
	char str[2];
	str[0] = ch;
	str[1] = '\0';

	int retnum = (int) strtol(str, NULL, 10);
	return retnum;
}

int
fodase(char *ph_addr) {
	char buffer[1024];
	char ph_buffer[1024];
	int end = 0;
	long int addr = strtol(ph_addr, NULL, 16);

	addr == 0 ? outusage() : 0;

	FILE *dir = popen("/usr/bin/ls /proc/", "r");
	while (fgets(buffer, sizeof(buffer), dir) != NULL) {
		for (int i = 0; i < strlen(buffer) - 2; i++) {
			if (isalpha(buffer[i]) != 0) {
				end = 1;
				break;
			}
		}

		if (end == 1)
			break;

		if (buffer != NULL) {
			ph_buffer[0] = '\0';
			strcat(ph_buffer, buffer);
		}

		char path[1024] = "/proc/";
		strcat(path, buffer);
		path[strlen(path) - 1] = '\0';
		strcat(path, "/maps");

		FILE *file = fopen(path, "r");
		if (file == NULL)
			continue;

		char map[10000];
		char ph_base[13];
		long int base;
		char ph_fim[13];
		long int fim;

		while (fgets(map, sizeof(map), file) != NULL) {
			for (int i = 0; i < 12; i++) {
				char str[2];
				str[0] = map[i];
				str[1] = '\0';

				strcat(ph_base, str);
			}

			for (int i = 13; i < 25; i++) {
				char str[2];
				str[0] = map[i];
				str[1] = '\0';

				strcat(ph_fim, str);
			}

			base = strtol(ph_base, NULL, 16);
			fim = strtol(ph_fim, NULL, 16);

			if (addr >= base && addr <= fim) {
				char comm[1000] = "/proc/";
				strcat(comm, ph_buffer);
				comm[strlen(comm) - 1] = '\0';
				strcat(comm, "/comm");

				FILE *filecomm = fopen(comm, "r");
				comm[0] = '\0';
				fgets(comm, sizeof(comm), filecomm);
				comm[strlen(comm) - 1] = '\0';
				ph_buffer[strlen(ph_buffer) - 1] = '\0';

				printf("[PID: %s] Alocação encontrada: %lx - %lx (%s)\n", ph_buffer, base, fim, comm);
				exit(0);
			}

			ph_base[0] = '\0';
			ph_fim[0] = '\0';
		}

		fclose(file);
	}

	pclose(dir);
	return 0;
}

int
main(int argc, char *argv[]) {
	if (argc < 2) {
		outusage();
	}

	return fodase(argv[1]);
}
