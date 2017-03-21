/* Copyright 2007-2009 Ben Hutchings.
 * Copyright 2008 Petter Reinholdtsen.
 * See the file "COPYING" for licence details.
 */
/* Source that reads a DIF ("raw DV") file */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <netinet/in.h>

#include "config.h"
#include "dif.h"
#include "frame_timer.h"
#include "protocol.h"
#include "socket.h"

static struct option options[] = {
    {"host",   1, NULL, 'h'},
    {"port",   1, NULL, 'p'},
    {"tally",  0, NULL, 'a'},
    {"loop",   0, NULL, 'l'},
    {"help",   0, NULL, 'H'},
    {"timings",0, NULL, 't'},
    {NULL,     0, NULL, 0}
};

static char * mixer_host = NULL;
static char * mixer_port = NULL;
static int do_tally = 0;

static void handle_config(const char * name, const char * value)
{
    if (strcmp(name, "MIXER_HOST") == 0)
    {
	free(mixer_host);
	mixer_host = strdup(value);
    }
    else if (strcmp(name, "MIXER_PORT") == 0)
    {
	free(mixer_port);
	mixer_port = strdup(value);
    }
}

static unsigned int get_file_size_bytes(int fd) 
{
    struct stat filestat;
    if (fstat(fd, &filestat) < 0)
    {
        fputs("ERROR: Failed to read file size\n", stderr);
        return 0;
    }
    return filestat.st_size;
}

static void usage(const char * progname)
{
    fprintf(stderr,
	    "\
Usage: %s [-h HOST] [-p PORT] [-a] [-l] [-t] FILE\n",
	    progname);
}

struct transfer_params {
    int            file;
    int            sock;
    bool           opt_loop;
    bool           timings;
};

static ssize_t read_retry(int fd, void * buf, size_t count)
{
    ssize_t chunk, total = 0;

    do
    {
	chunk = read(fd, buf, count);
	if (chunk <= 0)
	{
	    if (total == 0)
		return chunk;
	    break;
	}
	total += chunk;
	buf = (char *)buf + chunk;
	count -= chunk;
    }
    while (count);

    return total;
}

static void transfer_frames(struct transfer_params * params)
{
    const struct dv_system * last_system = 0, * system;
    static uint8_t buf[DIF_MAX_FRAME_SIZE];
    uint64_t frame_timestamp = 0;
    unsigned int frame_interval = 0;
    unsigned int frame_number = 0;
    unsigned int num_frames = 0;
    off_t file_size = 0;
    double video_length_sec = 0;

    if (params->timings)
        file_size = get_file_size_bytes(params->file);

    frame_timer_init();

    for (;;)
    {
	ssize_t size = read_retry(params->file, buf, DIF_SEQUENCE_SIZE);
	if (size == 0)
	{
	    // End of file; exit or loop
	    if (!params->opt_loop)
	    {
                if (params->timings)
                {
                    printf ("\n");
                    fflush(stdout);
                }
                return;
            }

	    if (lseek(params->file, 0, 0) == 0)
            {
                file_size = get_file_size_bytes(params->file);
                frame_number = 0;
                num_frames = 0;
                video_length_sec = 0;
                continue;
            }
            perror("ERROR: lseek");
            exit(1);
	}
	if (size != (ssize_t)DIF_SEQUENCE_SIZE)
	{
	    if (size < 0)
		perror("ERROR: read");
	    else
		fputs("ERROR: Failed to read complete frame\n", stderr);
	    exit(1);
	}

	system = dv_buffer_system(buf);

	/* (Re)set the timer according to this frame's video system. */
	if (system != last_system)
	{
	    last_system = system;
	    frame_timestamp = frame_timer_get();
	    frame_interval = (1000000000 / system->frame_rate_numer
			      * system->frame_rate_denom);
	}

	size = read_retry(params->file, buf + DIF_SEQUENCE_SIZE,
			  system->size - DIF_SEQUENCE_SIZE);
	if (size != (ssize_t)(system->size - DIF_SEQUENCE_SIZE))
	{
	    if (size < 0)
		perror("ERROR: read");
	    else
		fputs("ERROR: Failed to read complete frame\n", stderr);
	    exit(1);
	}
	frame_number++;
	if (write(params->sock, buf, system->size) != (ssize_t)system->size)
	{
	    perror("ERROR: write");
	    exit(1);
	}

	frame_timestamp += frame_interval;

	if (params->timings)
        {
	    if (video_length_sec == 0 && file_size > 0)
            {
		num_frames = (file_size / system->size);
	        video_length_sec = 
		    num_frames * (double) frame_interval / 1000000000;
	    }

	    /* No need to print out EVERY frame */
	    if (frame_number % 3 == 0 && num_frames > 0 ) 
            {
	        double video_position_seconds = 
		    frame_number * (double) frame_interval / 1000000000;
	        double video_percent_complete = 
		    video_position_seconds / video_length_sec * 100;
	        printf ("\rINFO: frame %6d/%6d. %6.2f/%6.2f seconds. %6.2f%%", 
		    frame_number, 
		    num_frames, 
		    video_position_seconds, 
		    video_length_sec, 
		    video_percent_complete);
	        fflush(stdout);
	    }
        }
	frame_timer_wait(frame_timestamp);
    }
}

static int is_dv_file(int fd)
{
    uint8_t buf[DIF_SIGNATURE_SIZE];
    int is_dv;
    off_t orig = lseek(fd, 0, SEEK_CUR);

    /* Can't check a non-seekable file; assume it's valid */
    if (orig == -1)
	return 1;

    is_dv = (read(fd, buf, DIF_SIGNATURE_SIZE) == DIF_SIGNATURE_SIZE
	     && !memcmp(buf, DIF_SIGNATURE, DIF_SIGNATURE_SIZE));
    lseek(fd, orig, SEEK_SET);
    return is_dv;
}

static void tally(struct transfer_params * params)
{
    // Messages should never be buffered
    setbuf(stdout, NULL);
    
    for (;;)
    {
	char act_msg[ACT_MSG_SIZE];
	ssize_t read_size = read_retry(params->sock, act_msg, ACT_MSG_SIZE);
	if (read_size < ACT_MSG_SIZE)
	{
	    if (read_size < 0)
		perror("ERROR: wtf read");
	    break;
	}

	if (act_msg[ACT_MSG_VIDEO_POS])
	    printf("TALLY: on\n");
	else
	    printf("TALLY: off\n");
    }

}

int main(int argc, char ** argv)
{
    /* Initialise settings from configuration files. */
    dvswitch_read_config(handle_config);

    struct transfer_params params;
    params.opt_loop = false;
    params.timings = false;

    /* Parse arguments. */

    int opt;
    while ((opt = getopt_long(argc, argv, "h:p:alt", options, NULL)) != -1)
    {
	switch (opt)
	{
	case 'h':
	    free(mixer_host);
	    mixer_host = strdup(optarg);
	    break;
	case 'p':
	    free(mixer_port);
	    mixer_port = strdup(optarg);
	    break;
	case 'a':
	    do_tally = 1;
	    break;
	case 'l':
	    params.opt_loop = true;
	    break;
	case 'H': /* --help */
	    usage(argv[0]);
	    return 0;
	case 't':
	    params.timings = true;
	    break;
	default:
	    usage(argv[0]);
	    return 2;
	}
    }

    if (!mixer_host || !mixer_port)
    {
	fprintf(stderr, "%s: mixer hostname and port not defined\n",
		argv[0]);
	return 2;
    }

    if (optind != argc - 1)
    {
	if (optind == argc)
	{
	    fprintf(stderr, "%s: missing filename\n",
		    argv[0]);
	}
	else
	{
	    fprintf(stderr, "%s: excess argument \"%s\"\n",
		    argv[0], argv[optind + 1]);
	}
	usage(argv[0]);
	return 2;
    }

    const char * filename = argv[optind];

    /* Prepare to read the file and connect a socket to the mixer. */
    if (strcmp(filename, "-"))
    {
	printf("INFO: Reading from %s\n", filename);
	params.file = open(filename, O_RDONLY, 0);
    }
    else
    {
	printf("INFO: Reading from STDIN\n");
	params.file = feof(stdin);
    }
    if (params.file < 0)
    {
	perror("ERROR: open");
	return 1;
    }
    if (!is_dv_file(params.file)) {
        fprintf(stderr, "ERROR: %s is not a DV file\n", filename);
	return 1;
    }
    printf("INFO: Connecting to %s:%s\n", mixer_host, mixer_port);
    params.sock = create_connected_socket(mixer_host, mixer_port);
    assert(params.sock >= 0); /* create_connected_socket() should handle errors */
    if (write(params.sock, do_tally ? GREETING_ACT_SOURCE : GREETING_SOURCE, 
		GREETING_SIZE) != GREETING_SIZE)
    {
	perror("ERROR: write");
	exit(1);
    }
    printf("INFO: Connected.\n");

    if (do_tally)
    {
	fflush(NULL);
	int child_pid = fork();

	if (child_pid < 0)
	{
	    perror("ERROR: fork");
	    return 1;
	}
	if (child_pid == 0)
	{
	    tally(&params);
	    _exit(0);
	}
    }

    transfer_frames(&params);

    close(params.sock);
    close(params.file);

    return 0;
}
