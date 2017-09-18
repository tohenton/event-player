#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define DEV_PATH_LEN 64

struct input_event {
    struct timeval time;
    unsigned short type;
    unsigned short code;
    unsigned int value;
};

struct Record {
    struct timeval wait;
    unsigned short type;
    unsigned short code;
    unsigned int value;
    struct Record* next;
};

struct RecordList {
    struct Record* head;
    struct Record* tail;
};

int parse_records(FILE* fp, struct RecordList* list, char* devpath) {
    char* ret;
    char readline[256];
    char work[16];
    int len = 0;
    struct Record* record = NULL;

    ret = fgets(readline, DEV_PATH_LEN, fp);
    if (ret == NULL) {
        fprintf(stderr, "Error: Cannot get device path\n");
        return -1;
    }
    strcpy(devpath, readline);
    len = strlen(devpath);
    devpath[len-1] = '\0';

    while (fgets(readline, 256, fp) != NULL) {
        // Chomp
        len = strlen(readline);
        readline[len-1] = '\0';

        if (readline[0] == '#') {
            continue;
        }

        record = malloc(sizeof(struct Record));
        if (record == NULL) {
            fprintf(stderr, "Error: Cannot allocate memory area\n");
            exit(1);
        }
        memset(record, 0, sizeof(struct Record));

        if (strncmp(readline, "wait:", sizeof("wait:") - 1) == 0) { // Wait identifier found
            parse_wait_record(record, readline);
        } else if (readline[0] >= '0' && readline[0] <= '9') { // Event line found
            parse_event_record(record, readline);
        }

        record->next = NULL;

        if (list->head == NULL) {
            list->head = record;
            list->tail = record;
        } else {
            list->tail->next = record;
            list->tail = record;
        }
    }

    return 0;
}

int parse_event_record(struct Record* record, char* readline) {
    char work[16] = {0};

    // Type
    strncpy(work, readline, 6);
    work[6] = '\0';
    record->type = strtoul(work, NULL, 16);

    // Code
    strncpy(work, readline+7, 6);
    work[6] = '\0';
    record->code = strtoul(work, NULL, 16);

    // value
    strncpy(work, readline+14, 10);
    work[11] = '\0';
    record->value = strtoul(work, NULL, 16);
}

int parse_wait_record(struct Record* record, char* readline) {
    char work[16] = {0};
    char* ptr = NULL;
    int len = 0;
    int wait_len = 5; // strlen("wait:");

    // Sec
    ptr = strchr(readline, ',');
    len = ptr - readline - wait_len;
    strncpy(work, readline + wait_len, len);
    work[len] = '\0';
    record->wait.tv_sec = atoi(work);

    // Usec
    strcpy(work, readline + wait_len + len + 1);
    record->wait.tv_usec = atoi(work);

    record->type = 0xffff;
    record->code = 0xffff;
    record->value = 0xffffffff;
}

int isEventRecord(struct Record* record) {
    if (record != NULL) {
        if (record->type == 0xffff && record->code == 0xffff && record->value == 0xffffffff) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char** argv) {
    struct input_event *pEv = NULL;
    FILE* fp = NULL;
    struct RecordList list = {0};
    struct Record* r = NULL;
    struct input_event event = {0};
    char devpath[DEV_PATH_LEN] = {0};
    int ret = 0;
    int fd = -1;

    int record_fd = -1;

    if (argc < 2) {
        fprintf(stderr, "Error: No record file given\n");
        exit(1);
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to open record file\n");
    }

    ret = parse_records(fp, &list, devpath);
    if (ret < 0) {
        exit(1);
    }

    fd = open(devpath, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Erorr: Failed to open touch device\n");
    }

    for (r = list.head; r != NULL; r = r->next) {
        if(isEventRecord(r)) {
            event.type  = r->type;
            event.code  = r->code;
            event.value = r->value;
            write(fd, &event, sizeof(struct input_event));
        } else {
            if (r->wait.tv_sec != 0) {
                sleep(r->wait.tv_sec);
            } else {
                usleep(r->wait.tv_usec);
            }
        }
    }

    return 0;
}
